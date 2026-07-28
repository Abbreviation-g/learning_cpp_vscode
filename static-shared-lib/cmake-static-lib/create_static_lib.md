# 使用 CMake 创建并导出静态库

本文说明 `cmake-static-lib` 如何生成、安装并导出静态库，使其他 CMake
项目能够通过 `find_package()` 使用它们。

配套的消费端说明见
[在 CMake 项目中使用静态库](../cmake-use-static-lib/use_static_lib.md)。

## 1. 项目概览

`cmake-static-lib` 使用 C++17，生成两个彼此独立的静态库：

| CMake 目标 | 职责 | 公开接口 |
| --- | --- | --- |
| `message_utils` | 构造示例提示信息 | `std::string build_message()` |
| `sort_utils` | 返回排序后的整数副本 | `bubble_sort`、`selection_sort`、`insertion_sort`、`merge_sort`、`quick_sort` |

排序接口位于 `sort_algorithms` 命名空间中，参数和返回值均为
`std::vector<int>`。

本项目与动态库示例完全独立：

- 包名：`cmake-static-lib`
- 默认安装前缀：`C:/install/cmake-static-lib`
- 安装头文件目录：`include/cmake-static-lib`
- 导入目标：`cmake-static-lib::message_utils` 和
  `cmake-static-lib::sort_utils`

因此它可以与动态库包安装在不同前缀中，不会让
`find_package(cmake-static-lib)` 错误加载动态库包。

项目的主要结构如下：

```text
cmake-static-lib/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake-static-lib-config.cmake.in
├── create_static_lib.md
├── .vscode/
│   └── tasks.json
└── src/
    ├── CMakeLists.txt
    └── utils/
        ├── CMakeLists.txt
        ├── message/
        │   ├── CMakeLists.txt
        │   ├── message.h
        │   └── message.cpp
        └── sort/
            ├── CMakeLists.txt
            ├── sort_algorithms.h
            └── sort_algorithms.cpp
```

## 2. 环境要求

- Windows 与 PowerShell 7（`pwsh`）
- CMake 3.25 或更高版本
- MinGW 的 `g++` 和 `mingw32-make` 已加入 `PATH`

根 `CMakeLists.txt` 声明的最低版本是 3.16，但
`CMakePresets.json` 使用 schema version 6；通过本文的 preset 命令构建时，
实际需要 CMake 3.25 或更高版本。

项目还提供 Ninja/Clang preset。无论选择哪套工具链，生产静态库和消费静态库
的工程都应使用 ABI 兼容的编译器、C++ 标准库、目标架构和构建配置。

## 3. CMake 如何生成静态库

### 3.1 根目录与三级 CMake 结构

根 `CMakeLists.txt` 定义项目版本和 C++17 标准，并在进入源码目录前加载
`GNUInstallDirs`：

```cmake
cmake_minimum_required(VERSION 3.16)
project(cmake-static-lib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
add_subdirectory(src)
```

提前加载 `GNUInstallDirs`，可以保证叶子目录配置
`${CMAKE_INSTALL_INCLUDEDIR}` 时，该变量已经有确定值。

`src/CMakeLists.txt` 和 `src/utils/CMakeLists.txt` 负责进入下一级目录；真正
定义目标的是 `message/CMakeLists.txt` 和 `sort/CMakeLists.txt`。这形成
“根工程 → 功能集合 → 具体库目标”的三级结构。

### 3.2 显式创建 `STATIC` 目标

两个叶子目录都显式使用 `STATIC`，因此目标类型不受
`BUILD_SHARED_LIBS` 的值影响：

```cmake
add_library(message_utils STATIC
    message.cpp
    message.h
)

add_library(sort_utils STATIC
    sort_algorithms.cpp
    sort_algorithms.h
)
```

目标的公开头文件路径同时覆盖构建树和安装树：

```cmake
target_include_directories(message_utils
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
```

- `BUILD_INTERFACE` 让同一构建树中的调用方从源码目录找到头文件。
- `INSTALL_INTERFACE` 让安装后的导入目标公开 `<prefix>/include`。
- `sort_utils` 使用相同配置。

## 4. 安装与 CMake 包导出

### 4.1 安装头文件

公开头文件安装到：

```text
<prefix>/include/cmake-static-lib/message.h
<prefix>/include/cmake-static-lib/sort_algorithms.h
```

消费端因此使用：

```cpp
#include <cmake-static-lib/message.h>
#include <cmake-static-lib/sort_algorithms.h>
```

### 4.2 只安装静态归档

两个静态目标加入 `cmake-static-lib-targets` 导出集，并通过
`ARCHIVE DESTINATION` 安装：

```cmake
install(
    TARGETS message_utils sort_utils
    EXPORT cmake-static-lib-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
```

在 MinGW 下，安装产物通常是：

```text
lib/libmessage_utils.a
lib/libsort_utils.a
```

这里的 `.a` 包含库的目标代码，是静态归档；它不是动态库对应的
`.dll.a` 导入库。使用 MSVC 时，对应静态归档通常使用 `.lib` 扩展名。

本项目的安装规则不需要为两个库设置 `RUNTIME` 目的地，也不会生成或安装
项目 DLL。静态库代码会在链接阶段按需合入最终程序。

### 4.3 导出带命名空间的目标

导出集安装到 `<prefix>/lib/cmake/cmake-static-lib`，并添加独立命名空间：

```cmake
install(
    EXPORT cmake-static-lib-targets
    FILE cmake-static-lib-targets.cmake
    NAMESPACE cmake-static-lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake-static-lib
)
```

这会为消费端创建：

```cmake
cmake-static-lib::message_utils
cmake-static-lib::sort_utils
```

导入目标携带静态归档位置和公开 include 路径。消费端无需硬编码 `.a` 或
`.lib` 文件的绝对路径。

### 4.4 包配置和版本文件

`cmake-static-lib-config.cmake.in` 加载导出的 targets 文件：

```cmake
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/cmake-static-lib-targets.cmake")

check_required_components(cmake-static-lib)
```

安装时还会生成 `cmake-static-lib-config-version.cmake`。消费端通过下面一行
同时加载包配置和两个导入目标：

```cmake
find_package(cmake-static-lib REQUIRED)
```

## 5. 使用 MinGW 构建和安装

在 `cmake-static-lib` 目录中运行：

```powershell
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix C:/install/cmake-static-lib
```

三条命令分别完成：

1. 使用 `MinGW Makefiles` 配置 Debug 构建，并生成
   `build/mingw/compile_commands.json`。
2. 编译 `message_utils` 和 `sort_utils` 静态库。
3. 安装静态归档、头文件和 CMake package 文件。

MinGW 构建树中的库通常位于：

```text
build/mingw/src/utils/message/libmessage_utils.a
build/mingw/src/utils/sort/libsort_utils.a
```

安装后的典型结构如下：

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

该安装树不需要保存本项目的 `bin` 目录，也不应包含
`libmessage_utils.dll`、`libsort_utils.dll` 或相应的 `.dll.a` 导入库。

## 6. 使用 Ninja/Clang 构建

生产端还提供 `ninja-clang` 和 `clangbuild`：

```powershell
cmake --preset ninja-clang
cmake --build --preset clangbuild
cmake --install build/ninja-clang --prefix C:/install/cmake-static-lib-clang
```

该流程要求 `clang++` 和 `ninja` 已加入 `PATH`。当前消费示例以
MinGW 为主，因此不要直接把 Clang 生成的静态归档交给 MinGW 消费。示例使用
独立安装前缀，避免覆盖 MinGW 产物。

## 7. 静态库与动态库的区别

| 项目 | 静态库 | 动态库 |
| --- | --- | --- |
| MinGW 链接输入 | `.a` 静态归档 | `.dll.a` 导入库 |
| 项目运行时文件 | 库代码已合入可执行文件 | 还需加载 `.dll` |
| 项目 DLL 搜索路径 | 不需要为这两个库配置 `PATH` | 通常需把安装目录的 `bin` 加入 `PATH` |
| 更新库 | 重新链接程序后生效 | 可在 ABI 兼容时替换 DLL |

“链接本项目的静态库”不等于“生成完全静态的可执行文件”。本工程没有添加
MinGW 的 `-static` 选项；程序仍可能依赖 `libstdc++`、`libgcc`、线程库和
Windows 系统 DLL。将程序复制到其他机器时，应使用依赖检查工具确认实际运行
时依赖。

两个项目内部仍使用通用目标名 `message_utils` 和 `sort_utils`。不要把共享版
和静态版同时通过 `add_subdirectory()` 加入同一顶层构建，否则会发生目标
重名；安装后应分别通过各自的包名和命名空间消费。

## 8. 常见问题

| 现象 | 检查方法 |
| --- | --- |
| 找不到 `g++` 或 `mingw32-make` | 确认 MinGW 工具目录已加入 `PATH` |
| preset 无法读取 | 使用 CMake 3.25 或更高版本 |
| 安装目录没有 `.a` | 先成功构建，再执行 `cmake --install` |
| 安装结果出现 `.dll` 或 `.dll.a` | 检查叶子目标是否确实使用 `STATIC`，并确认安装的是本项目的构建目录 |
| 消费端找不到包 | 检查 `<prefix>/lib/cmake/cmake-static-lib/cmake-static-lib-config.cmake` 是否存在 |
| 链接时报未定义符号或文件格式错误 | 确认生产端和消费端的编译器、C++ 标准库、位数及构建配置兼容 |
| 程序在其他机器上仍提示缺少运行库 DLL | 这是工具链运行时依赖，不是 `message_utils` 或 `sort_utils` 项目 DLL；按部署环境处理 MinGW 运行库 |
