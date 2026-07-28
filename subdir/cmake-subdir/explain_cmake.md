# CMake 工程结构说明

## 目录结构

```text
cmake-subdir/
|-- CMakeLists.txt
|-- CMakePresets.json
|-- src/
|   |-- CMakeLists.txt
|   |-- main.cpp
|   |-- linked_list/
|   |   |-- CMakeLists.txt
|   |   |-- singly_linked_list.cpp
|   |   `-- singly_linked_list.h
|   `-- utils/
|       |-- CMakeLists.txt
|       |-- message.cpp
|       |-- message.h
|       `-- sort/
|           |-- CMakeLists.txt
|           |-- sort_algorithms.cpp
|           `-- sort_algorithms.h
```

这个工程被拆成多个 CMake 子目录。每个目录负责管理自己目录下的代码和构建目标。

## 根目录 CMakeLists.txt

根目录的 `CMakeLists.txt` 定义整个工程：

```cmake
project(cmake_subdir_demo LANGUAGES CXX)
```

它还把 C++ 标准设置为 C++11：

```cmake
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

最后进入 `src` 目录：

```cmake
add_subdirectory(src)
```

所以根目录文件只负责全局工程配置，以及启动子目录构建树。

## src 模块

`src/CMakeLists.txt` 首先进入两个子模块：

```cmake
add_subdirectory(utils)
add_subdirectory(linked_list)
```

然后创建可执行程序目标：

```cmake
add_executable(cmake_subdir_demo
    main.cpp
)
```

该可执行程序链接两个库：

```cmake
target_link_libraries(cmake_subdir_demo
    PRIVATE
        message_utils
        linked_list_utils
)
```

这里的 `PRIVATE` 表示 `cmake_subdir_demo` 自己使用这些库，但不需要把依赖继续暴露给
其他目标。

## utils 模块

`src/utils/CMakeLists.txt` 首先进入排序子模块：

```cmake
add_subdirectory(sort)
```

然后扫描 `src/utils` 目录直属的源文件：

```cmake
file(GLOB MESSAGE_UTILS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)
```

当前会扫描到 `message.cpp`。这里不会递归扫描 `sort` 目录，因为 `sort` 有自己的
`CMakeLists.txt` 管理。

该模块创建一个静态库：

```cmake
add_library(message_utils STATIC
    ${MESSAGE_UTILS_SOURCES}
)
```

并公开自己的头文件目录：

```cmake
target_include_directories(message_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
```

因为 include 目录是 `PUBLIC`，链接了 `message_utils` 的目标可以直接包含：

```cpp
#include "message.h"
```

工具库还链接排序库：

```cmake
target_link_libraries(message_utils
    PUBLIC
        sort_algorithms
)
```

这里的 `PUBLIC` 有两个作用：

1. `message_utils` 自己可以使用 `sort_algorithms`。
2. 链接 `message_utils` 的目标也会继承 `sort_algorithms` 的使用要求，包括公开头文件目录。

因此，即使可执行程序只直接链接了 `message_utils`，`main.cpp` 仍然可以包含：

```cpp
#include "sort_algorithms.h"
```

## sort 模块

`src/utils/sort/CMakeLists.txt` 扫描 `sort` 目录直属的排序算法源文件：

```cmake
file(GLOB SORT_ALGORITHM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)
```

当前会扫描到 `sort_algorithms.cpp`。

然后创建一个静态库：

```cmake
add_library(sort_algorithms STATIC
    ${SORT_ALGORITHM_SOURCES}
)
```

并公开 `sort` 目录作为头文件目录：

```cmake
target_include_directories(sort_algorithms
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
```

这样，任何直接或间接链接 `sort_algorithms` 的目标都可以包含：

```cpp
#include "sort_algorithms.h"
```

## linked_list 模块

`src/linked_list/CMakeLists.txt` 扫描链表模块目录直属的源文件：

```cmake
file(GLOB LINKED_LIST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
)
```

当前会扫描到 `singly_linked_list.cpp`。

然后创建一个静态库：

```cmake
add_library(linked_list_utils STATIC
    ${LINKED_LIST_SOURCES}
)
```

并公开 `linked_list` 目录作为头文件目录：

```cmake
target_include_directories(linked_list_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
```

因此，链接了 `linked_list_utils` 的目标可以直接包含：

```cpp
#include "singly_linked_list.h"
```

## Library 依赖关系

目标依赖链如下：

```text
cmake_subdir_demo
|-- message_utils
|   `-- sort_algorithms
`-- linked_list_utils
```

对应到 CMake 写法：

```cmake
cmake_subdir_demo PRIVATE message_utils linked_list_utils
message_utils PUBLIC sort_algorithms
```

这表示：

- `cmake_subdir_demo` 直接链接 `message_utils` 和 `linked_list_utils`。
- `message_utils` 直接链接 `sort_algorithms`。
- `sort_algorithms` 的 include 路径会通过 `message_utils` 传递给可执行程序。
- `linked_list_utils` 的 include 路径会直接传递给可执行程序。
- `main.cpp` 可以同时使用 `message.h`、`sort_algorithms.h` 和 `singly_linked_list.h`。

## 运行时调用关系

运行时，`main.cpp` 会调用：

```cpp
build_message()
```

这个函数实现在 `message.cpp` 中，属于 `message_utils` 静态库。

`main.cpp` 还会调用排序函数：

```cpp
sort_algorithms::bubble_sort(...)
sort_algorithms::selection_sort(...)
sort_algorithms::insertion_sort(...)
sort_algorithms::merge_sort(...)
sort_algorithms::quick_sort(...)
```

这些函数实现在 `sort_algorithms.cpp` 中，属于 `sort_algorithms` 静态库。

`main.cpp` 还会创建并测试单向链表：

```cpp
linked_list::SinglyLinkedList list;
list.push_back(...);
list.push_front(...);
list.remove_first(...);
list.reverse();
list.clear();
```

这些成员函数实现在 `singly_linked_list.cpp` 中，属于 `linked_list_utils` 静态库。

最终链接时，可执行程序会链接 `message_utils` 和 `linked_list_utils`，而
`message_utils` 又携带对 `sort_algorithms` 的依赖，因此可执行程序能够解析所有
消息、排序和链表相关函数。

## 构建顺序

CMake 会按照依赖顺序构建目标：

```text
sort_algorithms
message_utils
linked_list_utils
cmake_subdir_demo
```

其中 `sort_algorithms` 必须早于 `message_utils` 构建；`linked_list_utils` 和
`message_utils` 之间没有直接依赖关系，它们都完成后才会链接最终可执行程序。

## 当前工具链和 VS Code 配置

当前工程使用 MSYS2 UCRT64 提供的 MinGW-w64 工具链。系统环境变量 `PATH` 需要包含：

```text
C:\msys64\ucrt64\bin
```

并且 VS Code 需要在修改 PATH 之后重新启动。否则 VS Code task 仍可能拿到旧环境变量，
导致找不到 `gcc`、`g++`、`mingw32-make` 或 `gdb`。

### CMakePresets.json

`CMakePresets.json` 中的 `mingw` configure preset 使用：

```json
{
  "generator": "MinGW Makefiles",
  "binaryDir": "${sourceDir}/build/mingw",
  "cacheVariables": {
    "CMAKE_CXX_COMPILER": "g++",
    "CMAKE_MAKE_PROGRAM": "mingw32-make",
    "CMAKE_C_COMPILER": "gcc",
    "CMAKE_BUILD_TYPE": "Debug"
  }
}
```

这里没有写死 `C:\msys64\ucrt64\bin\g++.exe` 这类绝对路径，而是通过 PATH 查找工具。
这样配置更短，也方便以后移动 MSYS2 或切换同名工具链，但前提是 VS Code 启动时已经能
继承正确 PATH。

关键字段含义：

- `generator`
  - 使用 `MinGW Makefiles` 生成 Makefile。
- `binaryDir`
  - 把 CMake 构建文件生成到 `build/mingw`。
- `CMAKE_CXX_COMPILER`
  - 使用 PATH 中的 `g++` 作为 C++ 编译器。
- `CMAKE_C_COMPILER`
  - 使用 PATH 中的 `gcc` 作为 C 编译器。
- `CMAKE_MAKE_PROGRAM`
  - 使用 PATH 中的 `mingw32-make` 执行构建。
- `CMAKE_BUILD_TYPE`
  - 使用 `Debug`，方便 VS Code/GDB 命中断点。

如果从旧的 `C:\MinGW` 工具链切换到 MSYS2 UCRT64，建议删除旧构建缓存：

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
```

### VS Code tasks

`.vscode/tasks.json` 中的主要任务关系是：

```text
CMake: configure mingw
`-- CMake: build mingw
    |-- Run: cmake_subdir_demo
    `-- CMake: install mingw

CMake: configure mingw
`-- CMake: uninstall mingw
```

说明：

- `CMake: configure mingw`
  - 执行 `cmake --preset mingw`。
- `CMake: build mingw`
  - 先执行 configure，再执行 `cmake --build --preset mingw`。
- `Run: cmake_subdir_demo`
  - 先构建，再运行 `build/mingw/src/cmake_subdir_demo.exe`。
- `CMake: install mingw`
  - 先构建，再安装到 `build/install`。
- `CMake: uninstall mingw`
  - 先确保 configure 已生成 `uninstall` target，再执行
    `cmake --build --preset mingw --target uninstall`。

### VS Code launch

`.vscode/launch.json` 使用 `cppdbg` + `gdb`：

```json
{
  "type": "cppdbg",
  "program": "${workspaceFolder}\\build\\mingw\\src\\cmake_subdir_demo.exe",
  "MIMode": "gdb",
  "miDebuggerPath": "gdb",
  "preLaunchTask": "CMake: build mingw"
}
```

`miDebuggerPath` 设置为 `gdb`，同样依赖 PATH 解析到 MSYS2 UCRT64 中的
`C:\msys64\ucrt64\bin\gdb.exe`。

调试启动前会先执行 `CMake: build mingw`，保证当前 exe 是最新构建结果。

## CMake install 命令行参数

本工程可以通过下面命令安装：

```powershell
cmake --install .\build\mingw --prefix .\build\install
```

这条命令的含义如下：

- `cmake`
  - 调用 CMake 命令行程序。
- `--install`
  - 让 CMake 执行“安装阶段”，而不是 configure 或 build。
- `.\build\mingw`
  - 指定要安装哪个构建目录中的工程。这个目录必须已经执行过 `cmake --preset mingw`
    并生成了构建系统。
- `--prefix .\build\install`
  - 指定安装根目录，也就是安装前缀。
  - 如果不指定 `--prefix`，CMake 会使用 configure 阶段的 `CMAKE_INSTALL_PREFIX`。
  - 在这个例子中，最终会安装到工程目录下的 `build/install`。

安装前通常需要先构建：

```powershell
cmake --preset mingw
cmake --build --preset mingw
cmake --install .\build\mingw --prefix .\build\install
```

安装完成后的主要目录类似：

```text
build/install/
|-- bin/
|   `-- cmake_subdir_demo.exe
|-- include/
|   `-- cmake_subdir_demo/
|       |-- linked_list/
|       |   `-- singly_linked_list.h
|       `-- utils/
|           |-- message.h
|           `-- sort/
|               `-- sort_algorithms.h
`-- lib/
    |-- liblinked_list_utils.a
    |-- libmessage_utils.a
    |-- libsort_algorithms.a
    `-- cmake/
        `-- cmake_subdir_demo/
            |-- cmake_subdir_demoTargets.cmake
            `-- cmake_subdir_demoTargets-debug.cmake
```

## install 相关 CMake 配置

根目录 `CMakeLists.txt` 中启用了标准安装目录变量：

```cmake
include(GNUInstallDirs)
```

`GNUInstallDirs` 会提供一组跨平台安装目录变量，例如：

- `${CMAKE_INSTALL_BINDIR}`
  - 可执行程序目录，Windows/MinGW 下通常是 `bin`。
- `${CMAKE_INSTALL_LIBDIR}`
  - 库文件目录，通常是 `lib`。
- `${CMAKE_INSTALL_INCLUDEDIR}`
  - 头文件目录，通常是 `include`。

使用这些变量比手写 `bin`、`lib`、`include` 更规范，也方便以后适配不同平台。

这些变量不是本工程手动定义的，而是由 CMake 自带模块 `GNUInstallDirs` 在执行下面语句时
定义的：

```cmake
include(GNUInstallDirs)
```

在当前 Windows + MinGW 配置下，这些变量通常是相对路径：

```text
CMAKE_INSTALL_BINDIR      = bin
CMAKE_INSTALL_LIBDIR      = lib
CMAKE_INSTALL_INCLUDEDIR  = include
```

这些值会拼接到安装前缀 `<prefix>` 后面。也就是说，如果执行：

```powershell
cmake --install .\build\mingw --prefix .\build\install
```

那么 `<prefix>` 就是 `.\build\install`。

### `${CMAKE_INSTALL_BINDIR}`

`${CMAKE_INSTALL_BINDIR}` 表示“可执行程序安装目录”，通常是：

```text
bin
```

本工程用它安装可执行程序：

```cmake
install(TARGETS cmake_subdir_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

最终安装路径是：

```text
<prefix>/bin/cmake_subdir_demo.exe
```

使用当前安装命令时，对应：

```text
.\build\install\bin\cmake_subdir_demo.exe
```

### `${CMAKE_INSTALL_LIBDIR}`

`${CMAKE_INSTALL_LIBDIR}` 表示“库文件安装目录”，通常是：

```text
lib
```

本工程用它安装静态库：

```cmake
install(TARGETS message_utils
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
```

最终安装路径是：

```text
<prefix>/lib/libmessage_utils.a
```

本工程还用它安装导出的 CMake targets 文件：

```cmake
install(EXPORT cmake_subdir_demoTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake_subdir_demo
)
```

最终安装路径是：

```text
<prefix>/lib/cmake/cmake_subdir_demo/cmake_subdir_demoTargets.cmake
```

### `${CMAKE_INSTALL_INCLUDEDIR}`

`${CMAKE_INSTALL_INCLUDEDIR}` 表示“头文件安装目录”，通常是：

```text
include
```

本工程用它安装公开头文件：

```cmake
install(FILES message.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils
)
```

最终安装路径是：

```text
<prefix>/include/cmake_subdir_demo/utils/message.h
```

使用当前安装命令时，对应：

```text
.\build\install\include\cmake_subdir_demo\utils\message.h
```

### 安装可执行程序

`src/CMakeLists.txt` 中安装主程序：

```cmake
install(TARGETS cmake_subdir_demo
    EXPORT cmake_subdir_demoTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

参数含义：

- `TARGETS cmake_subdir_demo`
  - 指定要安装的 CMake target，这里是可执行程序。
- `EXPORT cmake_subdir_demoTargets`
  - 把这个 target 加入名为 `cmake_subdir_demoTargets` 的导出集合。
  - 后面 `install(EXPORT ...)` 会把这个集合写成 `.cmake` 文件。
- `RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}`
  - 指定运行时产物的安装位置。
  - 对可执行程序来说，就是把 `cmake_subdir_demo.exe` 安装到 `bin` 目录。

### 安装静态库

每个库模块都有类似配置，例如 `message_utils`：

```cmake
install(TARGETS message_utils
    EXPORT cmake_subdir_demoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

参数含义：

- `TARGETS message_utils`
  - 指定要安装的库 target。
- `EXPORT cmake_subdir_demoTargets`
  - 把该库也加入同一个导出集合。
- `ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}`
  - 指定静态库、导入库等 archive 类型产物的安装位置。
  - 本工程使用 MinGW 静态库，产物是 `.a` 文件，所以会安装到 `lib`。
- `LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}`
  - 指定动态库主体的安装位置。
  - 当前工程生成的是静态库，这一项主要是为了配置完整。
- `RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}`
  - 指定运行时动态库或可执行文件的安装位置。
  - 在 Windows 上，如果以后生成 `.dll`，通常会进入 `bin`。

`sort_algorithms` 和 `linked_list_utils` 也使用同样的安装方式，所以它们最终都会安装到
`lib` 目录，并加入 `cmake_subdir_demoTargets` 导出集合。

### 安装头文件

头文件通过 `install(FILES ...)` 安装，例如：

```cmake
install(FILES message.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils
)
```

参数含义：

- `FILES message.h`
  - 指定要安装的普通文件。
- `DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils`
  - 指定安装目标目录。
  - 这里会安装到 `include/cmake_subdir_demo/utils/message.h`。

其他头文件安装位置：

- `sort_algorithms.h`
  - 安装到 `include/cmake_subdir_demo/utils/sort`。
- `singly_linked_list.h`
  - 安装到 `include/cmake_subdir_demo/linked_list`。

### 构建目录 include 和安装目录 include

库 target 使用了这样的 include 配置：

```cmake
target_include_directories(message_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmake_subdir_demo/utils>
)
```

这里的两个表达式叫 generator expression：

- `$<BUILD_INTERFACE:...>`
  - 在源码树中直接构建时使用。
  - 例如当前工程构建时，`message_utils` 的头文件目录就是 `src/utils`。
- `$<INSTALL_INTERFACE:...>`
  - target 被安装并导出后使用。
  - 例如别人通过安装后的 CMake target 使用 `message_utils` 时，头文件目录就是
    `include/cmake_subdir_demo/utils`。

这样做可以避免把本机源码目录写进安装后的 CMake target 文件中。

### 导出 CMake targets

`src/CMakeLists.txt` 中还有一段：

```cmake
install(EXPORT cmake_subdir_demoTargets
    FILE cmake_subdir_demoTargets.cmake
    NAMESPACE cmake_subdir_demo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake_subdir_demo
)
```

参数含义：

- `EXPORT cmake_subdir_demoTargets`
  - 安装前面所有 `install(TARGETS ... EXPORT cmake_subdir_demoTargets)` 收集到的 target。
- `FILE cmake_subdir_demoTargets.cmake`
  - 指定生成的导出文件名。
- `NAMESPACE cmake_subdir_demo::`
  - 给导出的 target 名称加命名空间。
  - 安装后 target 名称会变成类似 `cmake_subdir_demo::message_utils`。
- `DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmake_subdir_demo`
  - 指定导出文件安装目录。
  - 当前会安装到 `lib/cmake/cmake_subdir_demo`。

这个导出文件的作用是：让其他 CMake 工程可以复用本工程安装后的 target 信息，包括库路径、
头文件路径和 target 依赖关系。

当前工程只安装了 targets 文件，还没有生成完整的
`cmake_subdir_demoConfig.cmake`。如果以后希望其他工程直接使用
`find_package(cmake_subdir_demo)`，还需要额外增加 package config 文件。

## uninstall 配置说明

CMake 默认提供 `install` 机制，但不默认生成 `uninstall` 目标。也就是说，CMake
知道如何把文件安装出去，但不会自动知道用户希望如何删除这些文件。

本工程手动增加了一个 `uninstall` target，用来删除上一次 install 记录下来的文件。

### 根目录 CMakeLists.txt 中的配置

根目录 `CMakeLists.txt` 中增加了：

```cmake
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    @ONLY
)
```

含义如下：

- `configure_file(...)`
  - 在 configure 阶段根据模板文件生成一个实际可执行的 CMake 脚本。
- `${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in`
  - 输入模板文件，位于源码目录的 `cmake` 文件夹下。
- `${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake`
  - 输出脚本文件，生成到当前构建目录中。
  - 当前 preset 下对应 `build/mingw/cmake_uninstall.cmake`。
- `@ONLY`
  - 只替换 `@VAR@` 形式的变量。
  - 这样可以避免误替换 `${VAR}` 形式的 CMake 变量。

接着定义了自定义 target：

```cmake
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
```

含义如下：

- `add_custom_target(uninstall ...)`
  - 创建一个名为 `uninstall` 的构建目标。
  - 之后就可以执行 `cmake --build --preset mingw --target uninstall`。
- `COMMAND ${CMAKE_COMMAND} -P ...`
  - 调用 CMake 自己执行一个脚本文件。
- `-P`
  - 表示以 script mode 运行 CMake 文件。
  - 这种模式不会重新 configure 工程，只执行脚本里的命令。

### uninstall 脚本模板

模板文件是：

```text
cmake/cmake_uninstall.cmake.in
```

首先检查 install manifest 是否存在：

```cmake
if(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: @CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")
endif()
```

`install_manifest.txt` 是 CMake 执行 `cmake --install ...` 后生成的文件，里面记录了本次
安装写出去的所有文件路径。uninstall 正是依靠这个文件知道要删除哪些文件。

如果从未执行过 install，或者构建目录被清理过，那么这个文件可能不存在。此时脚本会报错，
避免误删其他文件。

然后读取安装清单：

```cmake
file(READ "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt" installed_files)
string(REGEX REPLACE "\n" ";" installed_files "${installed_files}")
```

含义如下：

- `file(READ ...)`
  - 把 `install_manifest.txt` 的内容读到变量 `installed_files`。
- `string(REGEX REPLACE "\n" ";" ...)`
  - CMake 列表使用分号 `;` 分隔。
  - 这一步把按行保存的文件路径转换成 CMake 列表。

接着遍历每个已安装文件：

```cmake
foreach(installed_file ${installed_files})
```

如果遇到空行就跳过：

```cmake
if(installed_file STREQUAL "")
    continue()
endif()
```

然后拼出要删除的完整路径：

```cmake
set(full_path "$ENV{DESTDIR}${installed_file}")
```

这里支持 `DESTDIR` 环境变量。`DESTDIR` 常用于打包场景，可以在实际安装路径前面再加一层
临时根目录。普通本地使用时，`DESTDIR` 为空，所以 `full_path` 就等于 manifest 里的路径。

如果文件存在，就删除：

```cmake
if(EXISTS "${full_path}" OR IS_SYMLINK "${full_path}")
    message(STATUS "Uninstalling: ${full_path}")
    execute_process(
        COMMAND "@CMAKE_COMMAND@" -E rm -f "${full_path}"
        RESULT_VARIABLE remove_result
    )
```

含义如下：

- `EXISTS "${full_path}"`
  - 判断普通文件是否存在。
- `IS_SYMLINK "${full_path}"`
  - 判断符号链接是否存在。
- `message(STATUS ...)`
  - 输出正在卸载的文件。
- `execute_process(...)`
  - 执行一个外部命令。
- `"@CMAKE_COMMAND@" -E rm -f`
  - 使用 CMake 自带的跨平台文件删除命令。
  - `-E` 表示执行 CMake helper command。
  - `rm -f` 表示删除文件，如果不存在也不报错。
- `RESULT_VARIABLE remove_result`
  - 保存删除命令的返回值。

如果删除失败，则报错终止：

```cmake
if(NOT remove_result EQUAL 0)
    message(FATAL_ERROR "Failed to remove: ${full_path}")
endif()
```

如果文件已经不存在，则只输出提示：

```cmake
else()
    message(STATUS "Already removed: ${full_path}")
endif()
```

### uninstall 的执行方式

命令行执行方式：

```powershell
cmake --build --preset mingw --target uninstall
```

含义如下：

- `--build --preset mingw`
  - 使用 `mingw` build preset 对应的构建目录。
- `--target uninstall`
  - 不构建默认目标，而是执行名为 `uninstall` 的自定义 target。

VS Code 中也配置了 task：

```text
CMake: uninstall mingw
```

它实际执行的参数是：

```json
[
    "--build",
    "--preset",
    "mingw",
    "--target",
    "uninstall"
]
```

该 task 依赖：

```text
CMake: configure mingw
```

这样可以确保 `uninstall` target 已经被 CMake 生成。

### 当前 uninstall 的行为

当前 uninstall 会删除 `install_manifest.txt` 中记录的已安装文件，例如：

- `bin/cmake_subdir_demo.exe`
- `lib/libmessage_utils.a`
- `lib/libsort_algorithms.a`
- `lib/liblinked_list_utils.a`
- `include/cmake_subdir_demo/...`
- `lib/cmake/cmake_subdir_demo/...`

它只删除文件，不递归删除空目录。因此卸载后可能还会保留空的 `bin`、`lib`、`include`
目录，这是正常行为，也更安全。
