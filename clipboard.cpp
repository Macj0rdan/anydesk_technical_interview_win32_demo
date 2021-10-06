#include "clipboard.h"
#include <iostream>
#include <windows.h>

std::string CLIPBOARD::getStringFromStdin()
{
    std::string stringFromStdin;
    std::cin.ignore(); // Ignore 
    getline(std::cin, stringFromStdin);

    return stringFromStdin;
};


void CLIPBOARD::setClipboardToString(HWND hWnd, LPWSTR clipboardString)
{
    
    const size_t len = wcslen(clipboardString) + 1; // defines size for buffer with terminate char
    //allocate memory
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * len);
    wcscpy((WCHAR*)GlobalLock(hMem), clipboardString);
    GlobalUnlock(hMem);

    int clipBoardOpened = OpenClipboard(hWnd);
    if(clipBoardOpened){std::wcout << "Clipboard opened\n";}
    EmptyClipboard();
    std::wcout << "Clipboard emptied\n";
    SetClipboardData(CF_UNICODETEXT, hMem);
    std::wcout << L"Clipboard set to " << clipboardString << std::endl;
    int clipBoardClosed = CloseClipboard();
    std::wcout << "Clipboard closed = " << clipBoardClosed << std::endl;
};
