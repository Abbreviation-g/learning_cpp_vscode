# cmake_subdir_demo

This is a small CMake subdirectory demo for MinGW on Windows.

## Project structure

The root `CMakeLists.txt` only defines the project and enters the source
directory:

```cmake
add_subdirectory(src)
```

`src/CMakeLists.txt` adds the `utils` subdirectory, creates the executable, and
links the static library built by `src/utils/CMakeLists.txt`.

## CMake preset

`CMakePresets.json` defines one configure/build preset named `mingw`.

The configure preset uses:

- Generator: `MinGW Makefiles`
- Build directory: `build/mingw`
- C compiler: `gcc`
- C++ compiler: `g++`
- Make program: `C:/MinGW/bin/mingw32-make.exe`
- Build type: `Debug`

Important detail:

`MinGW Makefiles` expects MinGW's `mingw32-make.exe`. The MSYS
`C:/MinGW/msys/1.0/bin/make.exe` program is intended for the `MSYS Makefiles`
generator and can fail when it receives Windows-style paths from the MinGW
generator.

## Command line build

```powershell
cmake --preset mingw
cmake --build --preset mingw
.\build\mingw\src\cmake_subdir_demo.exe
```

If the build cache was created with the wrong make program or build type,
remove the old build directory first:

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
cmake --build --preset mingw
```

## VS Code tasks

`.vscode/tasks.json` defines three tasks:

- `CMake: configure mingw`
  - Runs `C:\Program Files\CMake\bin\cmake.exe --preset mingw`
- `CMake: build mingw`
  - Runs `C:\Program Files\CMake\bin\cmake.exe --build --preset mingw`
  - Depends on `CMake: configure mingw`
  - Is the default build task
- `Run: cmake_subdir_demo`
  - Runs `build\mingw\src\cmake_subdir_demo.exe`
  - Depends on `CMake: build mingw`

In VS Code, use `Terminal > Run Build Task...` to run the default build task.

## VS Code launch

`.vscode/launch.json` defines one debug configuration:

- Name: `Debug cmake_subdir_demo`
- Debugger type: `cppdbg`
- Program: `${workspaceFolder}\build\mingw\src\cmake_subdir_demo.exe`
- Debugger: `C:\MinGW\bin\gdb.exe`
- Pre-launch task: `CMake: build mingw`

The launch config keeps the debug session inside VS Code:

```json
"externalConsole": false,
"avoidWindowsConsoleRedirection": false,
"internalConsoleOptions": "openOnSessionStart",
"logging": {
    "programOutput": true
}
```

With MinGW GDB on Windows, program stdout may appear in the Debug Console
instead of the Terminal panel. If a separate real console window is preferred,
set `externalConsole` to `true`.
