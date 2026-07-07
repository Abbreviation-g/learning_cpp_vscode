# cmake_subdir_demo

这是一个在 Windows MinGW 环境下使用 CMake 子目录组织代码的小示例工程。

## 工程结构

根目录的 `CMakeLists.txt` 只负责定义工程和进入源码目录：

```cmake
add_subdirectory(src)
```

`src/CMakeLists.txt` 会进入 `utils` 子目录，创建可执行程序，并链接
`src/utils/CMakeLists.txt` 生成的静态库。

## CMake Preset 配置

`CMakePresets.json` 定义了一个名为 `mingw` 的 configure/build preset。

该 configure preset 使用：

- Generator：`MinGW Makefiles`
- 构建目录：`build/mingw`
- C 编译器：`gcc`
- C++ 编译器：`g++`
- Make 程序：`C:/MinGW/bin/mingw32-make.exe`
- 构建类型：`Debug`

注意：

`MinGW Makefiles` 应该搭配 MinGW 自带的 `mingw32-make.exe`。MSYS 的
`C:/MinGW/msys/1.0/bin/make.exe` 更适合 `MSYS Makefiles` generator；如果和
`MinGW Makefiles` 混用，可能会因为 Windows 路径和反斜杠处理问题导致构建失败。

## 命令行构建

```powershell
cmake --preset mingw
cmake --build --preset mingw
.\build\mingw\src\cmake_subdir_demo.exe
```

如果之前的构建缓存使用了错误的 make 程序或构建类型，可以先删除旧构建目录：

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
cmake --build --preset mingw
```

## 命令行安装

工程已经配置了 CMake install 规则，可以安装可执行程序、静态库、头文件和导出的
CMake targets。

安装到临时目录：

```powershell
cmake --install .\build\mingw --prefix .\build\install
```

安装后的主要内容：

- `bin/cmake_subdir_demo.exe`
- `lib/libmessage_utils.a`
- `lib/libsort_algorithms.a`
- `lib/liblinked_list_utils.a`
- `include/cmake_subdir_demo/...`
- `lib/cmake/cmake_subdir_demo/cmake_subdir_demoTargets.cmake`

## VS Code Tasks 配置

`.vscode/tasks.json` 定义了三个任务：

- `CMake: configure mingw`
  - 执行 `C:\Program Files\CMake\bin\cmake.exe --preset mingw`
- `CMake: build mingw`
  - 执行 `C:\Program Files\CMake\bin\cmake.exe --build --preset mingw`
  - 依赖 `CMake: configure mingw`
  - 是默认构建任务
- `Run: cmake_subdir_demo`
  - 执行 `build\mingw\src\cmake_subdir_demo.exe`
  - 依赖 `CMake: build mingw`

在 VS Code 中可以通过 `Terminal > Run Build Task...` 运行默认构建任务。

## VS Code Launch 配置

`.vscode/launch.json` 定义了一个调试配置：

- 名称：`Debug cmake_subdir_demo`
- 调试器类型：`cppdbg`
- 程序路径：`${workspaceFolder}\build\mingw\src\cmake_subdir_demo.exe`
- GDB 路径：`C:\MinGW\bin\gdb.exe`
- 启动前任务：`CMake: build mingw`

当前 launch 配置让调试会话留在 VS Code 内部：

```json
"externalConsole": false,
"avoidWindowsConsoleRedirection": false,
"internalConsoleOptions": "openOnSessionStart",
"logging": {
    "programOutput": true
}
```

在 Windows + MinGW GDB 环境下，程序的 stdout 可能显示在 Debug Console，
而不是 Terminal 面板。如果希望输出显示在独立控制台窗口，可以把
`externalConsole` 改为 `true`。
