#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include <windows.h>
#include <string>

class CLIPBOARD
{

public:

    std::string getStringFromStdin();

    void setClipboardToString(HWND hWnd, LPWSTR clipboardString);

};

#endif // CLIPBOARD_H
