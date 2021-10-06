# win32 demo

This is a small program showcasing some usages of the winapi.  
It displays a GUI with buttons and text.

# Compilation

Compile with GCC on windows:

```bash
g++ -Wall -Wextra -Wno-bool-compare -Werror -O2 -g {path}*.cpp -o {outputpath}/win32_demo.exe -lgdi32 -static -static-libgcc -static-libstdc++
```

# Usage

Simply run win32_demo.exe on a windows machine