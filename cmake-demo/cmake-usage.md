# CMake 使用说明

本文档基于本仓库的两个关键提交，介绍两种在 CMake 中"导入子文件夹源文件"的常用做法：

1. **自动扫描并添加所有子文件夹**（提交 `93f6d40`）
2. **将子文件夹做成静态库，再导入静态库**（提交 `afd9379`）

---

## 一、自动扫描并添加所有子文件夹

> 对应提交：`93f6d40 自动扫描并添加所有子文件夹`

### 1.1 背景

在更早的提交中，子目录是通过**手动列举**的方式加入的，例如：

```cmake
# 旧写法：手动指定每个子目录
add_subdirectory(utils)
# 或
add_subdirectory(sort)
add_subdirectory(message)
```

这种方式在新增子目录时需要手动修改 `CMakeLists.txt`，容易遗漏。提交 `93f6d40` 改为**自动扫描当前目录下的所有子文件夹**，并逐个调用 `add_subdirectory()`。

### 1.2 核心代码

在每个层级的 `CMakeLists.txt` 中，把原来的手动 `add_subdirectory(xxx)` 替换为下面这段通用逻辑：

```cmake
# 自动扫描并添加所有子文件夹
file(GLOB ALL_ENTRIES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

foreach(ENTRY ${ALL_ENTRIES})
    if(IS_DIRECTORY ${ENTRY})
        get_filename_component(DIR_NAME ${ENTRY} NAME)
        add_subdirectory(${DIR_NAME})
    endif()
endforeach()
```

### 1.3 关键命令解析

| 命令 / 变量 | 作用 |
| --- | --- |
| `file(GLOB ...)` | 收集匹配模式的文件/目录列表。这里用 `*` 匹配当前目录下的所有条目。 |
| `CONFIGURE_DEPENDS` | 告诉 CMake 在构建时重新检查 glob 结果是否变化（新增/删除子目录时自动重新配置）。 |
| `${CMAKE_CURRENT_SOURCE_DIR}` | 当前 `CMakeLists.txt` 所在目录的绝对路径。 |
| `foreach(ENTRY ${ALL_ENTRIES})` | 遍历扫描到的每一个条目。 |
| `if(IS_DIRECTORY ${ENTRY})` | 判断该条目是否为目录（跳过普通文件）。 |
| `get_filename_component(DIR_NAME ${ENTRY} NAME)` | 从完整路径中提取目录名（最后一级名称）。 |
| `add_subdirectory(${DIR_NAME})` | 把该子目录加入构建，CMake 会去执行其中的 `CMakeLists.txt`。 |

### 1.4 本层源码的收集

在递归子目录之前，每个 `CMakeLists.txt` 还会先收集**本层**的源文件并加入目标：

```cmake
file(GLOB CURRENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${CURRENT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
```

> 注意：在 `93f6d40` 这个提交中，所有层级（包括 `message`、`sort`）都直接把源文件加入同一个可执行目标 `${PROJECT_NAME}`，尚未拆分静态库。

### 1.5 优缺点

- **优点**：新增子目录无需修改 `CMakeLists.txt`，自动化程度高。
- **缺点**：`GLOB` 不会在"新增文件"时自动触发重新配置（除非加 `CONFIGURE_DEPENDS`，且部分生成器支持有限）；所有源码都堆进同一个目标，模块边界不清晰。

---

## 二、将子文件夹做成静态库，再导入静态库

> 对应提交：`afd9379 将子文件夹做成静态库，然后再导入静态库的方式实现导入子文件夹源文件`

### 2.1 背景

在自动扫描子目录的基础上，提交 `afd9379` 进一步把 `message` 和 `sort` 这类"工具子文件夹"从"直接加入可执行目标"改为**先编译成静态库，再由主程序链接**。这样每个模块有清晰的边界，也更符合大型项目的组织方式。

### 2.2 子目录改为静态库

以 `src/utils/message/CMakeLists.txt` 为例，把原来的 `target_sources(${PROJECT_NAME} ...)` 改为 `add_library(... STATIC ...)`：

```cmake
file(GLOB MESSAGE_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# 编译为静态库（.a / .lib）
add_library(message_utils STATIC
    ${MESSAGE_UTILS_SOURCES}
)

# 用 PUBLIC 暴露头文件目录：链接该库的目标自动获得此 include 路径
target_include_directories(message_utils
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

`src/utils/sort/CMakeLists.txt` 同理，生成静态库 `sort_utils`：

```cmake
file(GLOB SORT_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(sort_utils STATIC
    ${SORT_UTILS_SOURCES}
)

target_include_directories(sort_utils
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### 2.3 根目标链接静态库

根 `CMakeLists.txt` 创建可执行目标后，通过 `target_link_libraries` 链接这两个静态库：

```cmake
cmake_minimum_required(VERSION 3.16)
project(cmake-demo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 可执行文件（此时不在这里直接列源文件，源文件由子目录提供）
add_executable(${PROJECT_NAME})
add_subdirectory(src)

# 主程序直接依赖消息/排序工具库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        message_utils
        sort_utils
)
```

### 2.4 关键命令解析

| 命令 | 作用 |
| --- | --- |
| `add_library(<name> STATIC <srcs>)` | 把源文件编译为静态库（Linux 下为 `.a`，Windows 下为 `.lib`）。 |
| `target_include_directories(<lib> PUBLIC <dir>)` | 声明该库的公开头文件路径。`PUBLIC` 表示：库自身和**链接该库的目标**都能使用此路径，因此 `main.cpp` 无需再手动 `target_include_directories` 就能 `#include "message.h"`。 |
| `target_link_libraries(<exe> PRIVATE <libs>)` | 让可执行文件链接指定的静态库，`PRIVATE` 表示仅本目标使用、不传递给下游。 |

### 2.5 递归扫描仍然保留

静态库方案**保留了**第一部分的"自动扫描子文件夹"逻辑——每个 `CMakeLists.txt` 末尾依旧有那段 `file(GLOB ALL_ENTRIES ...)` + `foreach` 的递归代码，用于自动发现并加入下级子目录。两者是**互补**关系：

- 自动扫描：解决"如何发现并进入子目录"的问题；
- 静态库：解决"子目录源码如何组织成可复用模块"的问题。

### 2.6 优缺点

- **优点**：模块边界清晰，可复用、可单独测试；头文件路径通过 `PUBLIC` 自动传递，主工程配置更简洁。
- **缺点**：相比直接 `target_sources`，多了一层库目标的定义；`GLOB` 的固有局限仍然存在（建议配合 `CONFIGURE_DEPENDS` 使用）。

---

## 三、两种方案对比

| 维度 | 方案一：自动扫描子目录 | 方案二：静态库 + 链接 |
| --- | --- | --- |
| 子目录源码去向 | 直接加入可执行目标 | 先编译为静态库，再链接 |
| 模块边界 | 弱（都在同一目标） | 强（独立库目标） |
| 头文件路径 | 每个目录自行 `target_include_directories` | 库用 `PUBLIC` 自动传递 |
| 可复用性 | 低 | 高 |
| 适用场景 | 小型 demo、快速原型 | 中大型项目、需要分模块管理 |

> 本仓库当前（最新提交）采用的是 **方案二**，并保留了方案一的自动扫描逻辑。
