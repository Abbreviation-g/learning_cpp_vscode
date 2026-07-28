# 使用 CMake 创建并导出动态库

本文说明 `cmake-lib` 如何生成、安装并导出动态库，使其他 CMake
项目能够通过 `find_package()` 使用它们。

配套的消费端说明见
[在 CMake 项目中使用动态库](../cmake-use-lib/use_shared_lib.md)。

## 1. 项目概览

`cmake-lib` 使用 C++17，生成两个彼此独立的动态库：

| CMake 目标 | 动态库职责 | 公开接口 |
| --- | --- | --- |
| `message_utils` | 构造示例提示信息 | `std::string build_message()` |
| `sort_utils` | 返回排序后的整数副本 | `bubble_sort`、`selection_sort`、`insertion_sort`、`merge_sort`、`quick_sort` |

排序接口位于 `sort_algorithms` 命名空间中，参数和返回值均为
`std::vector<int>`。

项目的主要结构如下：

```text
cmake-lib/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake-lib-config.cmake.in
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

## 2. 本文 MinGW 演示环境

- Windows 与 PowerShell 7（`pwsh`）
- CMake 3.25 或更高版本
- MinGW 的 `gcc`、`g++` 和 `mingw32-make` 已加入 `PATH`

这些是本文端到端示例的环境要求，不是 CMake 工程本身只能运行在 Windows
上的限制。工程还提供了 Ninja/Clang preset；在其他平台上，`SHARED` 目标会
生成该平台对应的动态库格式。

根 `CMakeLists.txt` 声明的最低版本是 CMake 3.16，但
`CMakePresets.json` 使用 schema version 6。通过本文的 preset 命令构建时，
实际需要 CMake 3.25 或更高版本。

`mingw` preset 的显示名称中包含旧的 GCC 版本号，这只是界面标签，不是
编译器版本限制。

## 3. CMake 如何生成动态库

### 3.1 根目录：定义工程并进入源码目录

根 `CMakeLists.txt` 定义项目版本、C++17 标准，并通过
`add_subdirectory(src)` 进入源码目录：

```cmake
cmake_minimum_required(VERSION 3.16)
project(cmake-lib VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
```

### 3.2 中间目录：自动加入功能子目录

`src/CMakeLists.txt` 和 `src/utils/CMakeLists.txt` 扫描直接子目录，并对
每个目录调用 `add_subdirectory()`。因此真正定义目标的是最末级的
`message/CMakeLists.txt` 和 `sort/CMakeLists.txt`。

### 3.3 叶子目录：显式创建 `SHARED` 目标

两个目标都显式使用 `SHARED`，因此不依赖 `BUILD_SHARED_LIBS`：

```cmake
add_library(message_utils SHARED
    ${MESSAGE_UTILS_SOURCES}
)

add_library(sort_utils SHARED
    ${SORT_UTILS_SOURCES}
)
```

目标使用 `PUBLIC` include 目录：

```cmake
target_include_directories(message_utils
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
```

- 在当前源码树中构建时，调用方可以从库的源码目录找到头文件。
- `INSTALL_INTERFACE` 表达了安装后使用 include 目录的意图。
- `sort_utils` 使用相同方式配置。

当前根 `CMakeLists.txt` 在 `add_subdirectory(src)` 之后才加载
`GNUInstallDirs`，因此全新配置时，叶子目录中的
`${CMAKE_INSTALL_INCLUDEDIR}` 可能尚未初始化。最终导出目标仍能获得
`<prefix>/include`，是因为根安装规则同时使用了
`INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}`。如果以后调整 CMake
代码，宜把 `include(GNUInstallDirs)` 移到 `add_subdirectory(src)` 之前。

## 4. 安装与 CMake 包导出

根 `CMakeLists.txt` 使用 `GNUInstallDirs` 和
`CMakePackageConfigHelpers`，完成四类安装内容。

### 4.1 安装头文件

公开头文件被安装到：

```text
<prefix>/include/cmake-lib/message.h
<prefix>/include/cmake-lib/sort_algorithms.h
```

因此消费端使用：

```cpp
#include <cmake-lib/message.h>
#include <cmake-lib/sort_algorithms.h>
```

### 4.2 安装动态库和导入库

两个目标被加入 `cmake-lib-targets` 导出集：

```cmake
install(
    TARGETS message_utils sort_utils
    EXPORT cmake-lib-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
```

在 MinGW/Windows 下：

- `RUNTIME` 是运行时加载的 `.dll`，安装到 `<prefix>/bin`。
- `ARCHIVE` 是链接阶段使用的 `.dll.a` 导入库，安装到 `<prefix>/lib`。

### 4.3 导出带命名空间的目标

导出集安装到 `<prefix>/lib/cmake/cmake-lib`，并添加
`cmake-lib::` 命名空间：

```cmake
install(
    EXPORT cmake-lib-targets
    FILE cmake-lib-targets.cmake
    NAMESPACE cmake-lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake-lib
)
```

消费端最终链接的不是文件路径，而是两个导入目标：

```cmake
cmake-lib::message_utils
cmake-lib::sort_utils
```

导出文件记录了头文件目录、导入库位置和 DLL 位置。它不会自动复制 DLL，
也不会修改 Windows 的运行时搜索路径；运行阶段仍需按第 7 节配置。

### 4.4 生成包配置和版本文件

`cmake-lib-config.cmake.in` 会加载导出的 targets 文件：

```cmake
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/cmake-lib-targets.cmake")

check_required_components(cmake-lib)
```

项目还会生成 `cmake-lib-config-version.cmake`，版本兼容规则为
`SameMajorVersion`。这些文件让消费端可以使用：

```cmake
find_package(cmake-lib REQUIRED)
```

`project(... VERSION 1.0.0)` 当前只用于生成包版本文件。两个动态库目标没有
设置 `VERSION` 或 `SOVERSION`，因此项目版本不会自动转换成动态库文件名或
soname 的二进制版本。

## 5. 使用 MinGW 构建和安装

在 `cmake-lib` 目录中运行：

```powershell
cmake --preset mingw
cmake --build --preset mingwbuild
cmake --install build/mingw --prefix C:/install/cmake-lib
```

三条命令分别完成：

1. 使用 `MinGW Makefiles` 配置 Debug 构建，并生成
   `build/mingw/compile_commands.json`。
2. 编译 `message_utils` 和 `sort_utils`。
3. 把库、头文件和 CMake 包配置安装到 `C:/install/cmake-lib`。

未安装前，MinGW 构建产物位于各目标自己的构建子目录，例如：

```text
build/mingw/src/utils/message/
├── libmessage_utils.dll
└── libmessage_utils.dll.a

build/mingw/src/utils/sort/
├── libsort_utils.dll
└── libsort_utils.dll.a
```

安装后的典型结构如下：

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

## 6. 使用 Ninja/Clang 构建库

库项目还提供了 `ninja-clang` 和 `clangbuild`：

```powershell
cmake --preset ninja-clang
cmake --build --preset clangbuild
cmake --install build/ninja-clang --prefix C:/install/cmake-lib-clang
```

该流程要求 `clang`、`clang++` 和 `ninja` 已加入 `PATH`。当前
`cmake-use-lib` 只提供 MinGW preset，因此本文不把 Clang 产物描述为已验证的
MinGW 消费流程。该 preset 同样生成 Debug 配置；示例使用独立安装前缀，避免
覆盖 MinGW 产物。安装和消费时应让生产者与消费者使用 ABI 兼容的编译器、
标准库、目标架构和构建配置。

## 7. Windows 和 ABI 注意事项

### 7.1 链接成功不代表运行时能找到 DLL

`.dll.a` 只用于链接。程序启动时，Windows 仍必须能找到真正的 DLL。推荐在
当前 pwsh 会话中把安装目录的 `bin` 加入 `PATH`：

```powershell
$env:PATH = "C:\install\cmake-lib\bin;$env:PATH"
```

也可以把 DLL 复制到可执行文件目录，但复制的 DLL 容易与新构建版本不一致。
当前安装规则只安装本项目的两个 DLL，不会自动部署 `libstdc++`、`libgcc`
或线程库等 MinGW 运行时依赖。将程序复制到未安装 MinGW 的机器时，还需一并
处理这些运行时依赖。

### 7.2 保持工具链兼容

公开接口跨 DLL 边界传递 `std::string` 和 `std::vector<int>`。库与消费程序
必须使用兼容的编译器、C++ 标准库、位数和构建配置，尤其不要混用 MinGW 与
MSVC 产物。

### 7.3 当前导出方式的范围

公开头文件目前没有 `__declspec(dllexport/dllimport)` 导出宏，项目也没有
启用 `WINDOWS_EXPORT_ALL_SYMBOLS`。当前 MinGW 流程依赖链接器自动导出符号；
不能据此保证 MSVC 或其他 Windows ABI 组合可直接使用。

## 8. 常见问题

| 现象 | 检查方法 |
| --- | --- |
| 找不到 `gcc`、`g++` 或 `mingw32-make` | 确认 MinGW 工具目录已加入 `PATH` |
| preset 无法读取 | 使用 CMake 3.25 或更高版本 |
| 安装目录没有 DLL | 先成功执行构建，再执行 `cmake --install` |
| 消费端找不到包 | 检查 `<prefix>/lib/cmake/cmake-lib/cmake-lib-config.cmake` 是否存在 |
| 链接时报未定义符号 | 检查工具链、架构和配置是否一致，并检查 DLL 导出宏或自动导出设置 |
| 程序启动时提示缺少 DLL | 把 `<prefix>/bin` 加入当前进程的 `PATH` |
