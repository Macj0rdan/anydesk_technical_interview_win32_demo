#include "gui.h"

int main() {

    int window_width    = 341; //-16 real size after borders and menubar
    int window_height   = 399; // -39
    LPCWSTR window_name = L"Jordans winapi demo";

    GUI gui;
    gui.CreateMainWindow(window_width, window_height, window_name);
    gui.RunMainLoop();
    
    return 0;
};


/*
    This is a small program that demonstrates using the win32 API to display a GUI with Buttons.
    It shows the use of different features of the API and of the C++ language.

    Compiled with GCC:
        -Wall -Wextra -Wno-bool-compare -Werror -std=c++20 -lgdi32 -static -static-libgcc -static-libstdc++

*/
