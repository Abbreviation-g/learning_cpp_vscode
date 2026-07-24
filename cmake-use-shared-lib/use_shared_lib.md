# 在 CMake 项目中使用动态库

本文说明 `cmake-use-lib` 如何查找、链接并运行 `cmake-lib` 安装的两个动态库，
以及如何通过 `compile_commands.json` 为 clangd 提供语法和跳转支持。

动态库的创建和导出过程见
[使用 CMake 创建并导出动态库](../cmake-lib/create_shared_lib.md)。

## 1. 消费关系概览

消费流程分为三个阶段：

1. `cmake-lib` 构建并安装头文件、DLL、导入库和 CMake 包配置。
2. `cmake-use-lib` 通过 `find_package(cmake-lib)` 加载导入目标并完成编译、链接。
3. 程序启动时，Windows 从可执行文件目录或 `PATH` 中查找 DLL。

项目使用以下两个导入目标：

| 导入目标 | 提供的功能 |
| --- | --- |
| `cmake-lib::message_utils` | `build_message()` |
| `cmake-lib::sort_utils` | 五种返回排序副本的排序函数 |

## 2. 环境要求

- Windows 与 PowerShell 7（`pwsh`）
- CMake 3.25 或更高版本
- MinGW 的 `gcc`、`g++` 和 `mingw32-make` 已加入 `PATH`
- 使用同一套兼容 MinGW 工具链构建并安装的 `cmake-lib`
- 如需代码补全和跳转：clangd 可执行文件及 VS Code clangd 扩展

虽然两个项目的 `CMakeLists.txt` 声明 CMake 3.16，但
`CMakePresets.json` 使用 schema version 6，因此本文的 preset 命令要求
CMake 3.25 或更高版本。

## 3. 完整构建和运行流程

以下命令从仓库根目录开始执行。

### 3.1 构建并安装动态库

```powershell
Set-Location .\cmake-lib
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix C:/install/cmake-lib
```

安装完成后，至少应存在：

```text
C:/install/cmake-lib/
├── bin/
│   ├── libmessage_utils.dll
│   └── libsort_utils.dll
├── include/
│   └── cmake-lib/
│       ├── message.h
│       └── sort_algorithms.h
└── lib/
    ├── libmessage_utils.dll.a
    ├── libsort_utils.dll.a
    └── cmake/
        └── cmake-lib/
            ├── cmake-lib-config.cmake
            ├── cmake-lib-config-version.cmake
            ├── cmake-lib-targets.cmake
            └── cmake-lib-targets-debug.cmake
```

### 3.2 配置并构建消费程序

```powershell
Set-Location ..\cmake-use-lib
cmake --preset mingw
cmake --build --preset mingwbuild
```

如果 `build/mingw` 以前配置过其他安装前缀，应使用
`cmake --fresh --preset mingw` 重新生成缓存，避免沿用旧的
`cmake-lib_DIR`。

`mingw` preset 已设置：

```json
"CMAKE_PREFIX_PATH": "C:/install/cmake-lib"
```

因此配置阶段会在安装前缀下查找 `cmake-lib` 的包配置。

### 3.3 配置 DLL 搜索路径并运行

```powershell
$env:PATH = "C:\install\cmake-lib\bin;$env:PATH"
.\build\mingw\cmake-use-lib.exe
```

`$env:PATH` 的修改只影响当前 pwsh 进程及其子进程，不会永久修改系统环境变量。

程序的预期输出为：

```text
Hello from a CMake subdirectory project built with MinGW.
原始数组: 5 3 8 1 9 2 7 4 6
快速排序: 1 2 3 4 5 6 7 8 9
冒泡排序: 1 2 3 4 5 6 7 8 9
归并排序: 1 2 3 4 5 6 7 8 9
```

## 4. CMake 如何找到并链接动态库

### 4.1 `CMAKE_PREFIX_PATH` 指向安装前缀

`CMAKE_PREFIX_PATH` 应指向安装根目录，而不是 `bin`、`lib` 或具体的
`.cmake` 文件：

```text
CMAKE_PREFIX_PATH
└── C:/install/cmake-lib
    └── lib/cmake/cmake-lib/cmake-lib-config.cmake
```

### 4.2 `find_package()` 加载包配置

项目通过以下语句加载安装包：

```cmake
find_package(cmake-lib REQUIRED)
```

`cmake-lib-config.cmake` 随后加载 `cmake-lib-targets.cmake`，从而创建：

```cmake
cmake-lib::message_utils
cmake-lib::sort_utils
```

### 4.3 链接导入目标

消费端不硬编码 DLL 或 `.dll.a` 的路径，而是直接链接导入目标：

```cmake
add_executable(cmake-use-lib src/main.cpp)

target_link_libraries(cmake-use-lib PRIVATE
    cmake-lib::message_utils
    cmake-lib::sort_utils
)
```

导入目标会把安装前缀下的 include 目录和 MinGW 导入库传递给
`cmake-use-lib`。由于最终目标是可执行程序，使用 `PRIVATE` 即可，无需继续向
下游传播依赖。

源码因此可以直接包含安装后的头文件：

```cpp
#include <cmake-lib/message.h>
#include <cmake-lib/sort_algorithms.h>
```

### 4.4 链接阶段与运行阶段不同

- 构建时，链接器使用 `<prefix>/lib/*.dll.a`。
- 运行时，Windows 加载器查找 `<prefix>/bin/*.dll`。
- `CMAKE_PREFIX_PATH` 只参与 CMake 配置，不会改变 Windows 的运行时 DLL
  搜索路径。

因此，即使程序成功链接，运行前仍需要配置 `PATH`，或者把 DLL 放到可执行
文件同一目录。推荐使用当前会话的 `PATH`，避免复制出过期 DLL。

## 5. clangd 语法支持

### 5.1 先生成编译数据库

`CMakePresets.json` 已开启：

```json
"CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
```

成功执行下面的配置命令后：

```powershell
cmake --preset mingw
```

CMake 会生成：

```text
cmake-use-lib/build/mingw/compile_commands.json
```

该文件记录真实编译器、C++17 参数和 include 搜索路径，是 clangd 理解工程的
主要依据。更换依赖路径、编译器或 CMake 配置后，应重新运行配置命令。

### 5.2 `.clangd` 定位编译数据库

项目根目录的 `.clangd` 包含：

```yaml
CompileFlags:
  CompilationDatabase: build/mingw
  Add:
    - "-IC:/install/cmake-lib/include"

Diagnostics:
  Suppress:
    - "pp_file_not_found"

Index:
  Background: Build
```

- `CompilationDatabase` 让 clangd 直接读取 `build/mingw`，无需把
  `compile_commands.json` 复制到项目根目录。
- `Add` 为安装头文件提供额外的兜底搜索路径。
- `Index.Background: Build` 开启后台索引。
- `pp_file_not_found` 被抑制后，头文件缺失诊断可能不会显示；如果跳转或补全
  失败，仍应检查安装路径和编译数据库，而不是把诊断抑制视为依赖已正确加载。

正常完成 CMake 配置后，导入目标的 include 路径也会进入编译数据库。`.clangd`
中的 `Add` 是补充配置，不能代替成功安装库和运行 CMake 配置。

### 5.3 VS Code 设置

`.vscode/settings.json` 禁用了 Microsoft C/C++ 扩展的 IntelliSense：

```json
"C_Cpp.intelliSenseEngine": "disabled"
```

这样可以避免它与 clangd 同时提供诊断和补全。该设置不会自动安装 clangd；
仍需保证 clangd 可执行文件和 VS Code clangd 扩展在本机可用。

同一设置文件还把 PowerShell 7 路径写为
`C:\sw\PowerShell-7.4.17\pwsh.exe`。如果本机安装位置不同，需要调整该终端
路径；它与 clangd 的工作方式无关。

## 6. 使用自定义安装前缀

如果不安装到 `C:/install/cmake-lib`，前两个位置必须保持一致：

1. 安装库时的 `--prefix`
2. 消费端配置时的 `CMAKE_PREFIX_PATH`

为避免兜底搜索路径继续引用旧版本，`.clangd` 中附加的
`-I<prefix>/include` 也应同步修改，或者移除这条冗余的 `Add`。

例如使用 `D:/sdk/cmake-lib`，以下命令从仓库根目录开始执行：

```powershell
$sharedLibPrefix = "D:/sdk/cmake-lib"

Set-Location .\cmake-lib
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix $sharedLibPrefix

Set-Location ..\cmake-use-lib
cmake --fresh --preset mingw "-DCMAKE_PREFIX_PATH=$sharedLibPrefix"
cmake --build --preset mingwbuild

$env:PATH = "$(Join-Path $sharedLibPrefix 'bin');$env:PATH"
.\build\mingw\cmake-use-lib.exe
```

同时把 `.clangd` 更新为：

```yaml
CompileFlags:
  CompilationDatabase: build/mingw
  Add:
    - "-ID:/sdk/cmake-lib/include"
```

成功配置后，导入目标的新 include 路径会进入 `compile_commands.json`。如果
没有同步修改 `.clangd`，clangd 还会额外搜索旧目录；旧安装仍存在时，可能
出现错误跳转或头文件版本混用。

## 7. 常见问题

| 现象 | 原因与处理 |
| --- | --- |
| CMake 提示找不到 `cmake-libConfig.cmake` | 先安装 `cmake-lib`，确认 `CMAKE_PREFIX_PATH` 指向安装根目录；切换前缀后用 `--fresh` 清除旧的 `cmake-lib_DIR` 缓存 |
| `#include <cmake-lib/...>` 飘红 | 确认头文件已安装、重新运行 `cmake --preset mingw`，并核对 `.clangd` 的 `-I` 路径 |
| 没有 `compile_commands.json` | 配置尚未成功，或查看了错误的 build 目录 |
| 修改配置后 clangd 仍显示旧结果 | 重新配置 CMake，并重启 clangd language server |
| 程序启动时提示缺少 `libmessage_utils.dll` 或 `libsort_utils.dll` | 在同一 pwsh 会话中把安装目录的 `bin` 加入 `PATH` |
| 链接时报未定义符号或文件格式错误 | 确认库和程序使用兼容的 MinGW 工具链、架构及构建配置 |
| 使用 MSVC 构建失败 | 当前库没有通用的 Windows 导出宏，现有流程只验证了 MinGW |
