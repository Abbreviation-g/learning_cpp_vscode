# cmake_subdir_demo

这是一个在 Windows + MSYS2 UCRT64 MinGW-w64 环境下使用 CMake 子目录组织代码的小示例工程。

## 工程结构

根目录的 `CMakeLists.txt` 只负责定义工程、设置 C++ 标准、引入安装目录变量，并进入源码目录：

```cmake
add_subdirectory(src)
```

`src/CMakeLists.txt` 会进入 `utils` 和 `linked_list` 子目录，创建可执行程序，并链接这些子目录生成的静态库。

## 工具链要求

当前工程假设 `C:\msys64\ucrt64\bin` 已经加入系统环境变量 `PATH`，并且 VS Code 是在更新环境变量后重新启动的。

需要能在 VS Code 终端中直接找到这些命令：

```powershell
gcc --version
g++ --version
mingw32-make --version
gdb --version
```

如果刚刚修改了系统环境变量，必须关闭所有 VS Code 窗口后重新打开工程，否则 VS Code task 可能仍然拿到旧 PATH。

## CMake Preset 配置

`CMakePresets.json` 定义了一个名为 `mingw` 的 configure/build preset。

该 configure preset 使用：

- Generator：`MinGW Makefiles`
- 构建目录：`build/mingw`
- C 编译器：`gcc`
- C++ 编译器：`g++`
- Make 程序：`mingw32-make`
- 构建类型：`Debug`

注意：

这些工具名依赖 PATH 解析到 MSYS2 UCRT64 目录，例如 `C:\msys64\ucrt64\bin`。不要再使用已经删除的 `C:\MinGW` 路径。`MinGW Makefiles` 应该搭配 `mingw32-make`，不要搭配 MSYS 的 `make.exe`。

## 命令行构建

```powershell
cmake --preset mingw
cmake --build --preset mingw
.\build\mingw\src\cmake_subdir_demo.exe
```

如果之前的构建缓存使用过旧工具链路径，可以先删除旧构建目录：

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
cmake --build --preset mingw
```

## 命令行安装

工程已经配置了 CMake install 规则，可以安装可执行程序、静态库、头文件和导出的 CMake targets。

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

`.vscode/tasks.json` 定义了五个任务：

- `CMake: configure mingw`
  - 执行 `C:\Program Files\CMake\bin\cmake.exe --preset mingw`
- `CMake: build mingw`
  - 执行 `C:\Program Files\CMake\bin\cmake.exe --build --preset mingw`
  - 依赖 `CMake: configure mingw`
  - 是默认构建任务
- `Run: cmake_subdir_demo`
  - 执行 `build\mingw\src\cmake_subdir_demo.exe`
  - 依赖 `CMake: build mingw`
- `CMake: install mingw`
  - 执行 `cmake --install ${workspaceFolder}\build\mingw --prefix ${workspaceFolder}\build\install`
  - 依赖 `CMake: build mingw`
- `CMake: uninstall mingw`
  - 执行 `cmake --build --preset mingw --target uninstall`
  - 依赖 `CMake: configure mingw`

在 VS Code 中可以通过 `Terminal > Run Task...` 选择这些任务。

## VS Code Launch 配置

`.vscode/launch.json` 定义了一个调试配置：

- 名称：`Debug cmake_subdir_demo`
- 调试器类型：`cppdbg`
- 程序路径：`${workspaceFolder}\build\mingw\src\cmake_subdir_demo.exe`
- GDB：`gdb`
- 启动前任务：`CMake: build mingw`

`gdb` 同样依赖 PATH 解析到 `C:\msys64\ucrt64\bin\gdb.exe`。

当前 launch 配置让调试会话留在 VS Code 内部：

```json
"externalConsole": false,
"avoidWindowsConsoleRedirection": false,
"internalConsoleOptions": "openOnSessionStart",
"logging": {
    "programOutput": true
}
```

在 Windows + MinGW GDB 环境下，程序的 stdout 可能显示在 Debug Console，而不是 Terminal 面板。如果希望输出显示在独立控制台窗口，可以把 `externalConsole` 改为 `true`。
