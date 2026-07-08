# cmake-subdir2

这是一个用于练习 C++ 和分层 CMake 配置的小项目。项目入口在 `src/main.cpp`，功能代码按目录拆分到 `src/utils`、`src/utils/sort` 和 `src/linked_list`。

## 环境要求

- CMake 3.20 或更高版本
- MinGW 工具链：`gcc`、`g++`、`gdb.exe`、`mingw32-make`
- 可选：VS Code、CMake Tools 扩展、C/C++ 扩展

## 命令行构建

项目已经在 `CMakePresets.json` 中配置了 `mingw` preset，推荐直接使用 preset 构建。

先配置项目：

```powershell
cmake --preset mingw
```

再编译项目：

```powershell
cmake --build --preset mingw
```

构建成功后，可执行文件位于：

```text
build/mingw/cmake_subdir2.exe
```

运行程序：

```powershell
.\build\mingw\cmake_subdir2.exe
```

## VS Code 构建

项目包含 `.vscode/tasks.json`：

- `CMake: configure (mingw)`：执行 `cmake --preset mingw`
- `CMake: build (mingw)`：执行 `cmake --build --preset mingw`

在 VS Code 中可以使用：

```text
Terminal -> Run Build Task
```

默认构建任务是 `CMake: build (mingw)`，它会先配置再构建。

## VS Code 调试

项目包含 `.vscode/launch.json`，调试配置名称为：

```text
Debug cmake_subdir2 (MinGW)
```

启动调试前会自动运行 `CMake: build (mingw)`，然后用 `gdb.exe` 调试：

```text
build/mingw/cmake_subdir2.exe
```

如果调试时没有看到程序输出，请确认 VS Code 的 Terminal 面板中是否打开了 `cppdbg` 相关终端。

## Ninja/Clang preset

项目也保留了 `ninja-clang` preset：

```powershell
cmake --preset ninja-clang
cmake --build --preset ninja-clang
```

使用它需要本机 PATH 中存在 `ninja`、`clang++` 和对应的 C/C++ 工具链。
