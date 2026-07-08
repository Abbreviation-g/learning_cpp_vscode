# CMake 配置说明

这个项目采用“根目标 + 子目录递归添加源码”的方式组织 CMake。根目录只负责创建最终可执行程序，各个源码目录负责扫描本层文件，并把下级目录加入构建。

## 根目录 CMakeLists.txt

根目录的 `CMakeLists.txt` 做三件事：

```cmake
cmake_minimum_required(VERSION 3.20)
project(cmake_subdir2 LANGUAGES CXX)
add_executable(cmake_subdir2)
```

- `cmake_minimum_required` 指定最低 CMake 版本。
- `project` 声明项目名和语言。
- `add_executable(cmake_subdir2)` 创建最终可执行目标。

随后设置 C++ 标准，并进入 `src`：

```cmake
target_compile_features(cmake_subdir2 PRIVATE cxx_std_17)
add_subdirectory(src)
```

这里的关键点是：目标 `cmake_subdir2` 先在根目录创建，后续所有子目录都通过 `target_sources` 给这个目标追加源码。

## 子目录 CMakeLists.txt

每个源码目录的 `CMakeLists.txt` 都使用相同模式：

```cmake
file(GLOB CURRENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

target_sources(cmake_subdir2 PRIVATE ${CURRENT_SOURCES})
target_include_directories(cmake_subdir2 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
```

含义如下：

- `file(GLOB ... CONFIGURE_DEPENDS ...)` 扫描当前目录的源文件和头文件。
- `target_sources` 把当前目录扫描到的文件加入 `cmake_subdir2`。
- `target_include_directories` 把当前目录加入头文件搜索路径。

如果当前目录下面还有子目录，就继续使用：

```cmake
add_subdirectory(子目录名)
```

例如 `src/CMakeLists.txt` 会加入：

```cmake
add_subdirectory(linked_list)
add_subdirectory(utils)
```

`src/utils/CMakeLists.txt` 会继续加入：

```cmake
add_subdirectory(sort)
```

## include 路径为什么能工作

因为 `src` 被加入了 include 路径，所以可以从 `main.cpp` 中写：

```cpp
#include "utils/message.h"
#include "linked_list/singly_linked_list.h"
#include "utils/sort/sort_algorithms.h"
```

同时，每个子目录也把自己加入了 include 路径，所以子目录内部可以使用更短的相对 include，例如：

```cpp
#include "message.h"
#include "sort_algorithms.h"
```

## 新增目录时怎么做

如果以后新增一个目录，例如：

```text
src/math
```

建议步骤：

1. 在 `src/math` 中添加 `.cpp` 和 `.h` 文件。
2. 在 `src/math` 中新增同样模式的 `CMakeLists.txt`。
3. 在上级 `src/CMakeLists.txt` 中加入：

```cmake
add_subdirectory(math)
```

这样新目录的源码就会被追加到同一个可执行目标 `cmake_subdir2` 中。

## preset 的作用

`CMakePresets.json` 用来保存常用构建配置，避免每次手写 generator、编译器和构建目录。

当前主要使用 `mingw` preset：

```powershell
cmake --preset mingw
cmake --build --preset mingw
```

它会把构建文件生成到：

```text
build/mingw
```

最终程序是：

```text
build/mingw/cmake_subdir2.exe
```
