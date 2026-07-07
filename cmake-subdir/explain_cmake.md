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
