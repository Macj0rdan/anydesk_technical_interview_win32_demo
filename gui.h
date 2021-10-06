#ifndef GUI_H
#define GUI_H
#define UNICODE
/* 
UNICODE needs to be defined before windows.h to correctly use widestring functions
Most windows functions have a A and W variant. 
Windows supplies a macro that decides the to use function depending on the UNICODE definition 
*/
#include <windows.h>
#include <string>



class GUI
{
private:

    // Needs to be static because we are supplying this function to the "lpfnWndProc" for the windows message que.
    static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //Button overloads
    void AddButton(int x_pos,int y_pos); // Add button without any settings. Any button created with this will share the same ID. For testing purposes. 
    void AddButton(int x_pos,int y_pos, LPCWSTR button_text, HMENU button_id); // Add button with text and ID at the given pos
    void AddButton(int x_pos,int y_pos, LPCWSTR button_text, HMENU button_id, int width, int height); // Add button with a specific size, text, ID at the given pos
    void AddButton(int x_pos,int y_pos, LPCWSTR button_text, HMENU button_id, int width, int height, int text_size); // Add button with a specific size, text, ID at the given pos
   
    // checkbox
    void AddCheckbox(int x_pos,int y_pos, LPCWSTR checkbox_text, HMENU button_id, int width, int height); // Add button with a specific size, text, ID at the given pos
    
    // Textlabel
    void AddTextLabel(int x_pos,int y_pos, LPCWSTR text, HMENU label_id, int width, int height);
    
    //text box
    void AddEditControl(int x_pos,int y_pos, LPCWSTR text, HMENU control_id, int width, int height);
    
    //Fencebar
    void AddFenceBar(int x_pos, int y_pos);

    // Function to set the font of a hwnd
    void SetFont(HWND control_handle, LPCWSTR font_type,int font_size = 15);

    // Displays a test messagebox
    int DisplayMessageBox(HWND hWnd);

    // Functions to handle message que callbacks
    LRESULT OnCommand(HWND hWnd, WPARAM wParam);
    LRESULT OnDestroy();
    LRESULT OnSize(LPARAM lParam);
    LRESULT OnCTLCOLORSTATIC(WPARAM wParam);
    LRESULT OnMousehover(HWND hWnd);
    LRESULT OnMousemove(HWND hWnd);
    LRESULT OnMouseleave(HWND hWnd);

    // constants for various settings
    const LPCWSTR FONT_TYPE = L"ARIAL";
    const int WIDTH_BUTTON  = 50;
    const int HEIGHT_BUTTON = 30;

    // various window variables
    HBRUSH hbrBkgnd     = NULL; // defined as NULL according to microsoft documentation 
    HINSTANCE hInstance = GetModuleHandle(0);
    HWND m_hWnd;
    WNDCLASS wc;
    MSG msg;

public:

    // main logic
    void CreateMainWindow(int width, int height, LPCWSTR window_name);
    int RunMainLoop();


};

#endif // GUI_H
