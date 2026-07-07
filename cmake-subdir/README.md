# cmake_subdir_demo

This is a small CMake subdirectory demo for MinGW on Windows.

## Build

```powershell
cmake --preset mingw
cmake --build --preset mingw
.\build\mingw\src\cmake_subdir_demo.exe
```

The `mingw` preset uses:

- Generator: `MinGW Makefiles`
- C++ compiler: `g++`
- Make program: `C:/MinGW/bin/mingw32-make.exe`

`MinGW Makefiles` expects MinGW's `mingw32-make.exe`. The MSYS
`C:/MinGW/msys/1.0/bin/make.exe` program is intended for the `MSYS Makefiles`
generator and can fail when it receives Windows-style paths from the MinGW
generator.
