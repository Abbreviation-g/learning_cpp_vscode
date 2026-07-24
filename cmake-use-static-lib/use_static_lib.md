# 在 CMake 项目中使用静态库

本文说明 `cmake-use-static-lib` 如何查找并链接 `cmake-static-lib` 安装的两个
静态库，以及如何通过 `compile_commands.json` 为 clangd 提供语法、补全和
跳转支持。

静态库的创建和导出过程见
[使用 CMake 创建并导出静态库](../cmake-static-lib/create_static_lib.md)。

## 1. 消费关系概览

消费流程分为三个阶段：

1. `cmake-static-lib` 构建并安装头文件、静态归档和 CMake 包配置。
2. `cmake-use-static-lib` 通过 `find_package(cmake-static-lib)` 加载导入目标。
3. 链接器从两个静态归档中按需取出目标代码，写入最终可执行文件。

项目使用以下两个导入目标：

| 导入目标 | 提供的功能 |
| --- | --- |
| `cmake-static-lib::message_utils` | `build_message()` |
| `cmake-static-lib::sort_utils` | 五种返回排序副本的排序函数 |

默认安装前缀是 `C:/install/cmake-static-lib`。包名、命名空间、头文件目录和
安装前缀均与动态库示例分开。

## 2. 环境要求

- Windows 与 PowerShell 7（`pwsh`）
- CMake 3.25 或更高版本
- MinGW 的 `g++` 和 `mingw32-make` 已加入 `PATH`
- 使用兼容 MinGW 工具链构建并安装的 `cmake-static-lib`
- 如需代码补全和跳转：clangd 可执行文件及 VS Code clangd 扩展

两个项目使用 C++17。其 `CMakePresets.json` 使用 schema version 6，因此
本文的 preset 命令实际要求 CMake 3.25 或更高版本。

## 3. 完整构建和运行流程

以下命令从仓库根目录开始执行。

### 3.1 构建并安装静态库

```powershell
Set-Location .\cmake-static-lib
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix C:/install/cmake-static-lib
```

安装完成后，至少应存在：

```text
C:/install/cmake-static-lib/
├── include/
│   └── cmake-static-lib/
│       ├── message.h
│       └── sort_algorithms.h
└── lib/
    ├── libmessage_utils.a
    ├── libsort_utils.a
    └── cmake/
        └── cmake-static-lib/
            ├── cmake-static-lib-config.cmake
            ├── cmake-static-lib-config-version.cmake
            ├── cmake-static-lib-targets.cmake
            └── cmake-static-lib-targets-debug.cmake
```

MinGW 下的 `.a` 是包含目标代码的静态归档，不是动态库使用的 `.dll.a`
导入库。

### 3.2 配置并构建消费程序

```powershell
Set-Location ..\cmake-use-static-lib
cmake --fresh --preset mingw
cmake --build --preset mingwbuild
```

`mingw` preset 默认设置：

```json
"CMAKE_PREFIX_PATH": "C:/install/cmake-static-lib"
```

因此配置阶段会在该前缀中查找 `cmake-static-lib` 包。这里使用 `--fresh`
重新生成 CMake 缓存，避免之前的 `cmake-static-lib_DIR` 或自定义安装前缀
继续生效。

### 3.3 直接运行

```powershell
.\build\mingw\cmake-use-static-lib.exe
```

运行前不需要把 `C:/install/cmake-static-lib/bin` 加入 `PATH`，因为
`message_utils` 和 `sort_utils` 的代码已经在链接时写入可执行文件。

程序的预期输出为：

```text
Hello from a CMake subdirectory project built with MinGW.
原始数组: 5 3 8 1 9 2 7 4 6
快速排序: 1 2 3 4 5 6 7 8 9
冒泡排序: 1 2 3 4 5 6 7 8 9
归并排序: 1 2 3 4 5 6 7 8 9
```

不需要本项目的 DLL，不代表可执行文件完全静态。项目没有添加 MinGW 的
`-static` 选项，程序仍可能依赖 MinGW C++ 运行库、线程库和 Windows 系统
DLL。

## 4. CMake 如何找到并链接静态库

### 4.1 `CMAKE_PREFIX_PATH` 指向安装前缀

`CMAKE_PREFIX_PATH` 应指向安装根目录，而不是 `include`、`lib` 或具体
`.cmake` 文件：

```text
CMAKE_PREFIX_PATH
└── C:/install/cmake-static-lib
    └── lib/cmake/cmake-static-lib/cmake-static-lib-config.cmake
```

### 4.2 `find_package()` 加载导入目标

消费端调用：

```cmake
find_package(cmake-static-lib REQUIRED)
```

包配置会加载 `cmake-static-lib-targets.cmake`，从而创建：

```cmake
cmake-static-lib::message_utils
cmake-static-lib::sort_utils
```

### 4.3 使用 `PRIVATE` 链接

消费端直接链接带命名空间的导入目标：

```cmake
add_executable(cmake-use-static-lib src/main.cpp)

target_link_libraries(cmake-use-static-lib PRIVATE
    cmake-static-lib::message_utils
    cmake-static-lib::sort_utils
)
```

导入目标把安装前缀下的 include 目录和 `.a` 文件位置传递给链接目标。由于
最终目标是可执行程序，使用 `PRIVATE` 即可，不需要把依赖继续传播给下游。

源码使用安装后的公开头文件：

```cpp
#include <cmake-static-lib/message.h>
#include <cmake-static-lib/sort_algorithms.h>
```

可以在生成的链接命令或安装导出文件中确认，两个导入目标的实际位置指向
`libmessage_utils.a` 和 `libsort_utils.a`，而不是 `.dll.a`。

## 5. clangd 语法支持

### 5.1 生成编译数据库

消费端 preset 开启：

```json
"CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
```

成功执行配置命令后：

```powershell
cmake --fresh --preset mingw
```

CMake 会生成：

```text
cmake-use-static-lib/build/mingw/compile_commands.json
```

该文件记录真实编译器、C++17 参数和导入目标提供的 include 路径。更换安装
前缀、编译器或 CMake 配置后，应重新运行配置命令。

### 5.2 `.clangd` 配置

项目根目录的 `.clangd` 指定编译数据库，并为默认安装头文件提供补充路径：

```yaml
CompileFlags:
  CompilationDatabase: build/mingw
  Add:
    - "-IC:/install/cmake-static-lib/include"

Index:
  Background: Build
```

- `CompilationDatabase` 让 clangd 读取 `build/mingw/compile_commands.json`。
- `Add` 是默认安装目录的兜底 include 路径。
- 成功加载导入目标后，正确的 include 路径也会出现在编译数据库中。

本项目不抑制 `pp_file_not_found`。如果依赖没有安装、CMake 配置失败或路径
错误，clangd 应保留头文件缺失诊断，便于及时发现问题。

`.vscode/settings.json` 只禁用 Microsoft C/C++ 扩展的重复 IntelliSense：

```json
{
    "C_Cpp.intelliSenseEngine": "disabled"
}
```

该设置不会安装 clangd，也不会配置 clangd 可执行文件路径；这些仍属于本机
前置条件。

## 6. 使用自定义安装前缀

自定义前缀时，需要同步更新三个位置：

1. 安装静态库时的 `--prefix`
2. 消费端配置时的 `CMAKE_PREFIX_PATH`
3. `.clangd` 中附加的 `-I<prefix>/include`

例如使用 `D:/sdk/cmake-static-lib`，以下命令从仓库根目录开始执行：

```powershell
$staticLibPrefix = "D:/sdk/cmake-static-lib"

Set-Location .\cmake-static-lib
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix $staticLibPrefix

Set-Location ..\cmake-use-static-lib
cmake --fresh --preset mingw "-DCMAKE_PREFIX_PATH=$staticLibPrefix"
cmake --build --preset mingwbuild
.\build\mingw\cmake-use-static-lib.exe
```

同时把 `.clangd` 更新为：

```yaml
CompileFlags:
  CompilationDatabase: build/mingw
  Add:
    - "-ID:/sdk/cmake-static-lib/include"
```

`--fresh` 很重要：CMake 会缓存找到的包目录。若只修改 preset 或命令行前缀
而沿用旧缓存，`find_package()` 仍可能加载之前安装的静态库。自定义前缀不
需要加入运行时 `PATH`，因为本项目不安装运行时 DLL。

## 7. 常见问题

| 现象 | 原因与处理 |
| --- | --- |
| CMake 提示找不到 `cmake-static-libConfig.cmake` | 先安装静态库，确认 `CMAKE_PREFIX_PATH` 指向安装根目录，并用 `--fresh` 清除旧的包目录缓存 |
| `#include <cmake-static-lib/...>` 飘红 | 确认头文件已安装，重新配置 CMake，并核对 `.clangd` 的 `-I` 路径 |
| 没有 `compile_commands.json` | CMake 配置尚未成功，或查看了错误的 `build/mingw` 目录 |
| 修改前缀后 clangd 仍跳转到旧头文件 | 用 `--fresh` 重新配置、同步更新 `.clangd`，然后重启 clangd language server |
| 链接命令出现 `.dll.a` | 很可能加载了动态库包或旧缓存；核对包名、命名空间、安装前缀并重新配置 |
| 链接时报未定义符号或文件格式错误 | 确认静态库和程序使用兼容的 MinGW 工具链、架构及构建配置 |
| 运行时提示缺少 `libmessage_utils.dll` 或 `libsort_utils.dll` | 当前程序可能链接了动态版或使用了旧构建缓存；静态版不需要这两个 DLL |
| 运行时提示缺少 `libstdc++`、`libgcc` 或线程库 DLL | 这属于 MinGW 运行时依赖；链接项目静态库不等于完全静态链接工具链运行库 |
