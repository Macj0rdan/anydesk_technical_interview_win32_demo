#include "gui.h"
#include "clipboard.h"
#include <windows.h>
#include <cassert>
#include <iostream>
#include <random>
#include <memory>

// Defining IDs for the different controls
constexpr int ID_TEST_BUTTON         = 100;
constexpr int ID_CLIPBOARD_BUTTON    = 101;
constexpr int ID_STATIC_TEXT         = 102;
constexpr int ID_TEXT_EDIT           = 103;
constexpr int ID_MESSAGEBOX_BUTTON   = 104;
constexpr int ID_MOVE_MOUSE_BUTTON   = 105;
constexpr int ID_FREE_CONSOLE_BUTTON = 106;
constexpr int ID_CLEAR_TEXT_BUTTON   = 107;
constexpr int ID_CHECKBOX            = 108;

// function for pointer handling
template<class T, class U, HWND(U::* m_hWnd)> T*
InstanceFromWndProc(HWND hWnd, UINT uMsg, LPARAM lParam)
{
    // Get the pointer to the class instance.
    T* pInstance;
    if (uMsg == WM_NCCREATE)
    {
        // The pointer has been passed via CreateWindowExW and now needs to be saved via SetWindowLongPtrW.
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pInstance = reinterpret_cast<T*>(pCreateStruct->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pInstance));

        // We are handling the first useful window message, so this is the perfect place to also set the hWnd member variable.
        pInstance->*m_hWnd = hWnd;
    }
    else
    {
        // Get the pointer saved via SetWindowLongPtrW above.
        pInstance = reinterpret_cast<T*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    return pInstance;
}

// Callback function for the windowclass lpfnWndProc -> Function that windows calls when a message gets handled.
LRESULT CALLBACK GUI::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Pointer to class instance. This way we can use this function as a static member class and simply use private functions to handle the messages.
    GUI* pGui = InstanceFromWndProc<GUI, GUI, &GUI::m_hWnd>(hWnd, uMsg, lParam);

    // Check for message and call needed functions
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            return pGui->OnDestroy();
        }
        case WM_COMMAND:
        {
            return pGui->OnCommand(hWnd, wParam);
        }
        case WM_SIZE:
        {
            return pGui->OnSize(lParam);
        }
        case WM_CTLCOLORSTATIC:
        {
            return pGui->OnCTLCOLORSTATIC(wParam);
        }
        case WM_MOUSEHOVER:
        {
            return pGui->OnMousehover(hWnd);
        }
        case WM_MOUSELEAVE:
        {
            return pGui->OnMouseleave(hWnd);
        }
        case WM_MOUSEMOVE:
        {
            return pGui->OnMousemove(hWnd);
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Main window functions
void GUI::CreateMainWindow(int width, int height, LPCWSTR window_name)
{
    wc             = {}; // windowclass
    wc.style       = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GUI::MessageHandler; // long pointer function window procedure -> pointer to C function. Supplying static function to get around non class problem
    wc.hInstance   = hInstance;
    wc.hCursor     = LoadCursor(nullptr, IDC_ARROW);

    // This color fits together with the background settings for the static controls. If the color here is changed it needs to be changed for them too.
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH); // needs "-lgdi32" linked to compiler. See "https://stackoverflow.com/a/64842359/12971025"
    wc.lpszClassName = L"MainWindowClass";
    assert(RegisterClass(&wc));


    /*
    Passing a pointer of this class instance to the lParam of CreateWindowEx
    This way I can use a static member function for the lpfnWndProc callback
    Using that I simply return the function I want to call and handle all the messages like that.
    Using modified version I found here -> https://building.enlyze.com/posts/writing-win32-apps-like-its-2020-part-2/
    */
    auto pGUI = std::unique_ptr<GUI>(new GUI);

    // Main window handle
    m_hWnd = CreateWindowEx(0, wc.lpszClassName, window_name, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, pGUI.get()); // In windows every control is its own window bound to this main window

    // Edit control
    AddEditControl(10, 10, L"", (HMENU)ID_TEXT_EDIT, 305, 50);

    // text label
    constexpr int text_label_x      = 95;
    constexpr int text_label_y      = 88;
    constexpr int width_text_label  = 225;
    constexpr int height_text_label = 45;


    // Buttons
    constexpr int FONT_SIZE_CLEAR_BUTTON = 14;
    this->AddButton(248, 63, L"Clear", (HMENU)ID_CLEAR_TEXT_BUTTON, 50, 20, FONT_SIZE_CLEAR_BUTTON);

    constexpr int BUTTON_WIDTH  = 80;
    constexpr int BUTTON_HEIGHT = 45;

    this->AddButton(10, 85, L"Clipboard Test", (HMENU)ID_CLIPBOARD_BUTTON, BUTTON_WIDTH, BUTTON_HEIGHT);
    LPCWSTR text_clipboard_desc = L"Click the button to set the Clipboard to the content of the text box.";
    this->AddTextLabel(text_label_x, text_label_y, text_clipboard_desc, (HMENU)ID_STATIC_TEXT, width_text_label, height_text_label);
    // this->AddFenceBar(95, 133);


    this->AddButton(10, 140, L"Show Messagebox", (HMENU)ID_MESSAGEBOX_BUTTON, BUTTON_WIDTH, BUTTON_HEIGHT);
    LPCWSTR descr_messagebox = L"Show a message box with buttons.";
    this->AddTextLabel(text_label_x, 143, descr_messagebox, (HMENU)ID_STATIC_TEXT, width_text_label, height_text_label);

    this->AddButton(10, 195, L"Move Mouse", (HMENU)ID_MOVE_MOUSE_BUTTON, BUTTON_WIDTH, BUTTON_HEIGHT);
    LPCWSTR descr_mouse_move = L"Move the mouse to a random position on the screen.";
    this->AddTextLabel(text_label_x, 198, descr_mouse_move, (HMENU)ID_STATIC_TEXT, width_text_label, height_text_label);


    this->AddButton(10, 250, L"Detach CMD", (HMENU)ID_FREE_CONSOLE_BUTTON, BUTTON_WIDTH, BUTTON_HEIGHT);
    LPCWSTR descr_detach = L"Detaches the console from the app.";
    this->AddTextLabel(text_label_x, 253, descr_detach, (HMENU)ID_STATIC_TEXT, width_text_label, height_text_label);

    this->AddCheckbox(10, 300, L"Enable move", (HMENU)ID_CHECKBOX, BUTTON_WIDTH, BUTTON_HEIGHT);
    LPCWSTR descr_checkbox = L"When this checkbox is ticked the window will randomly move around when hovering over it.";
    this->AddTextLabel(text_label_x, 303, descr_checkbox, (HMENU)ID_STATIC_TEXT, width_text_label, height_text_label);
    //todo: Replace magic numbers everywhere

}

// this function runs the main message loop to handle messages and send them to the callback
int GUI::RunMainLoop()
{
    bool bRet;
    constexpr int show_cmd = 1;
    ShowWindow(m_hWnd, show_cmd);
    UpdateWindow(m_hWnd);

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) // Using this way to keep the loop running. This is recommended by microsoft.
    {
        if (bRet == -1) // GetMessage return value can be nonzero, zero, or -1 so we cant avoid this
        {
            // handle the error and possibly exit
            std::wcout << "An error occurred";
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Return the exit code to the system.
    return msg.wParam;
}

int GUI::DisplayMessageBox(HWND hWnd)
{
    // Display a simply messagebox
    int msgboxID = MessageBoxExW(hWnd, (LPCWSTR)L"This is a textbox!\nWhat do you want to do?", (LPCWSTR)L"Message Box Test!", MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2, 0);
    switch (msgboxID)
    {
        case IDCANCEL:
            std::wcout << "Pressed cancel\n";
            break;
        case IDTRYAGAIN:
            std::wcout << "Pressed Try again\n";
            break;
        case IDCONTINUE:
            std::wcout << "Pressed continue\n";
            break;
    }

    return msgboxID;
}

// Handle message que functions
LRESULT GUI::OnCommand(HWND hWnd, WPARAM wParam)
{
    HWND handle_edit = GetDlgItem(hWnd, ID_TEXT_EDIT);
    switch (LOWORD(wParam))
    {
        case ID_TEST_BUTTON:
        {
            // For test purposes;
            break;
        }

        case ID_CLIPBOARD_BUTTON:
        {

            // Sets clipboard to string in the text edit
            int text_length = GetWindowTextLength(handle_edit);
            if (text_length == 0) { break; }

            // Allocates memory for GetWindowText
            LPTSTR cbString = (LPTSTR)VirtualAlloc((LPVOID)NULL, (DWORD)text_length + 1, MEM_COMMIT, PAGE_READWRITE);
            GetWindowText(handle_edit, cbString, text_length + 1);
            CLIPBOARD cb;
            cb.setClipboardToString(hWnd, cbString);
            break;
        }
        case ID_MESSAGEBOX_BUTTON:
        {
            // Displays a test message box
            int message_box = this->DisplayMessageBox(hWnd);
            if (!message_box)
            {
                std::wcout << GetLastError() << std::endl;
            }
            break;
        }
        case ID_MOVE_MOUSE_BUTTON:
        {
            // Get primary monitor size
            int primary_screen_width = GetSystemMetrics(SM_CXSCREEN);
            int primary_screen_height = GetSystemMetrics(SM_CYSCREEN);

            // Generate random x and y positions
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<> dist_x(0, primary_screen_width);
            std::uniform_int_distribution<> dist_y(0, primary_screen_height);
            int move_mouse_x = dist_x(mt);
            int move_mouse_y = dist_y(mt);

            // moves cursor
            SetCursorPos(move_mouse_x, move_mouse_y);
            break;
        }
        case ID_FREE_CONSOLE_BUTTON:
        {
            // detaches console from program
            FreeConsole();
            break;
        }
        case ID_CLEAR_TEXT_BUTTON:
        {
            // Clears text edit field
            SetWindowText(handle_edit, L"");
            break;
        }
    }
    return 0;
}

LRESULT GUI::OnDestroy()
{
    PostQuitMessage(0);
    return 0;
}

LRESULT GUI::OnSize(LPARAM lParam)
{
    UINT width  = LOWORD(lParam);
    UINT height = HIWORD(lParam);
    std::wcout << width << "\n" << height << std::endl;
    return 0;
}

LRESULT GUI::OnCTLCOLORSTATIC(WPARAM wParam)
{
    //! This runs on any control that sends the WM_CTLCOLORSTATIC message
    //todo: Check if it is possible to get the ID of the hWnd so we can ran this only when the ID is actually of something we want to modify

    // This color is made to fit together with the LTGRAY_BRUSH background brush. If that is changed this needs to be changed as well.

    // For some reason the repaint does not work correctly when launching the app with a debugger attached.
    constexpr int GRAY = 192;
    HDC hdcStatic = (HDC)wParam;

    SetTextColor(hdcStatic, RGB(0,0,0));
    SetBkColor(hdcStatic, RGB(GRAY, GRAY, GRAY));
    if (hbrBkgnd == NULL)
    {
        hbrBkgnd = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    }
    return (INT_PTR)hbrBkgnd;
}

LRESULT GUI::OnMousehover(HWND hWnd)
{
    int checkbox_state = IsDlgButtonChecked(hWnd, ID_CHECKBOX); // Get state of the checkbox. checked or unchecked
    if (checkbox_state)
    {
        // Get primary monitor size
        int primary_screen_width  = GetSystemMetrics(SM_CXSCREEN);
        int primary_screen_height = GetSystemMetrics(SM_CYSCREEN);

        // Generate random x and y positions
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<> dist_x(0, primary_screen_width);
        std::uniform_int_distribution<> dist_y(0, primary_screen_height);
        int move_window_x = dist_x(mt);
        int move_window_y = dist_y(mt);

        // Make sure final coordinates dont move the window out of view
        int final_pos_x = (move_window_x > primary_screen_width - 300) ? (move_window_x - 300) : (move_window_x);
        int final_pos_y = (move_window_y > primary_screen_height - 400) ? (move_window_y - 400) : (move_window_y);

        // Move window do coordinates
        int window_move = SetWindowPos(hWnd, HWND_TOP, final_pos_x, final_pos_y, 0, 0, SWP_NOSIZE | SWP_ASYNCWINDOWPOS);
        if (!window_move){std::wcout << "An error occurred setting the window position. Errnum = " << GetLastError() << std::endl;}
    }
    return 0;
}

LRESULT GUI::OnMousemove(HWND hWnd)
{
    // init mousetracker
    TRACKMOUSEEVENT tme = {};
    tme.cbSize      = sizeof(tme);
    tme.dwFlags     = TME_HOVER | TME_LEAVE;
    tme.hwndTrack   = hWnd;
    tme.dwHoverTime = 0;
    bool tme_bool   = TrackMouseEvent(&tme);
    if(!tme_bool){std::wcout << "An error occurred. Errnum = " << GetLastError() << std::endl;}
    return 0;
}
LRESULT GUI::OnMouseleave(HWND hWnd)
{
    // stop mousetracker when mouse leaves window
    TRACKMOUSEEVENT tme = {};
    tme.cbSize      = sizeof(tme);
    tme.dwFlags     = TME_HOVER | TME_LEAVE | TME_CANCEL;
    tme.hwndTrack   = hWnd;
    tme.dwHoverTime = 0;
    bool tme_bool   = TrackMouseEvent(&tme);
    if(!tme_bool){std::wcout << "An error occurred. Errnum = " << GetLastError() << std::endl;}
    return 0;
}

// Functions to add controls and set font
void GUI::SetFont(HWND control_handle, LPCWSTR font_type, int font_size)
{
    // Sets the font of the supplied hwnd
    HFONT hFont = CreateFont(font_size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, font_type);
    // HWND handle_control_static = GetDlgItem(hWnd, ID_STATIC_TEXT);
    SendMessage(control_handle, WM_SETFONT, WPARAM(hFont), TRUE);
}

void GUI::AddButton(int x_pos, int y_pos)
{
    HWND button_handle = CreateWindow(L"button", L"Button", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x_pos, y_pos, WIDTH_BUTTON, HEIGHT_BUTTON, m_hWnd, (HMENU)ID_TEST_BUTTON, hInstance, 0);
    this->SetFont(button_handle, FONT_TYPE);
}

void GUI::AddButton(int x_pos, int y_pos, LPCWSTR button_text, HMENU button_id)
{
    // creates button with ID and text as arguments
    HWND button_handle = CreateWindow(L"button", button_text, BS_MULTILINE | WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x_pos, y_pos, WIDTH_BUTTON, HEIGHT_BUTTON, m_hWnd, (HMENU)button_id, hInstance, 0);
    this->SetFont(button_handle, FONT_TYPE);
}

void GUI::AddButton(int x_pos, int y_pos, LPCWSTR button_text, HMENU button_id, int width, int height)
{
    // creates button with ID, size and text as arguments
    HWND button_handle = CreateWindow(L"button", button_text, BS_MULTILINE | WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x_pos, y_pos, width, height, m_hWnd, (HMENU)button_id, hInstance, 0);
    this->SetFont(button_handle, FONT_TYPE);
}

void GUI::AddButton(int x_pos, int y_pos, LPCWSTR button_text, HMENU button_id, int width, int height, int font_size)
{
    // creates button with ID, size, text and text size as arguments
    HWND button_handle = CreateWindow(L"button", button_text, BS_MULTILINE | WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, x_pos, y_pos, width, height, m_hWnd, (HMENU)button_id, hInstance, 0);
    this->SetFont(button_handle, FONT_TYPE, font_size);
}

void GUI::AddCheckbox(int x_pos, int y_pos, LPCWSTR checkbox_text, HMENU button_id, int width, int height)
{
    // creates checkbox with ID, size, text and text size as arguments
    HWND button_handle = CreateWindow(L"button", checkbox_text, BS_MULTILINE | WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x_pos, y_pos, width, height, m_hWnd, (HMENU)button_id, hInstance, 0);
    this->SetFont(button_handle, FONT_TYPE);
}

void GUI::AddTextLabel(int x_pos, int y_pos, LPCWSTR text, HMENU label_id, int width, int height)
{
    HWND text_handle = CreateWindow(L"static", text, SS_EDITCONTROL | WS_VISIBLE | WS_CHILD, x_pos, y_pos, width, height, m_hWnd, label_id, 0, 0);
    this->SetFont(text_handle, FONT_TYPE);
}

void GUI::AddEditControl(int x_pos, int y_pos, LPCWSTR text, HMENU control_id, int width, int height)
{
    HWND edit_handle = CreateWindow(L"edit", text, WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, x_pos, y_pos, width, height, m_hWnd, control_id, 0, 0);
    this->SetFont(edit_handle, FONT_TYPE);
}

void GUI::AddFenceBar(int x_pos, int y_pos)
{
    // Use a small static text box as a fence between controls
    CreateWindow(L"static", L"", SS_EDITCONTROL | WS_VISIBLE | WS_CHILD | SS_ETCHEDHORZ, x_pos, y_pos, 225, 5, m_hWnd, 0, 0, 0);
}