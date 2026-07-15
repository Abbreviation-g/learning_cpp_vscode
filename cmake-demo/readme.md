# cmake-demo

一个用于学习 **CMake** 的 C++ 示例工程。它演示了如何组织一个分层目录结构的 C++ 项目，并通过 CMake 把多个子目录中的源文件编译进同一个可执行程序。

## 项目简介

本项目从最简单的单文件 CMake 工程逐步演进，依次尝试了多种"把子目录源文件加入编译"的方案，最终采用 **静态库 + 链接** 的方式组织代码。它同时配置了多套编译器工具链（MinGW/GCC、Ninja/Clang），方便对比不同工具链下的构建行为。

## 目录结构

```
cmake-demo/
├── CMakeLists.txt              # 根 CMake 配置：定义工程、可执行目标、链接静态库
├── CMakePresets.json           # 多工具链（mingw / ninja-clang）的预设配置
├── build/                      # 构建产物目录（由 CMake 生成，可忽略）
│   ├── mingw/                  # GCC + MinGW Makefiles 生成结果
│   └── ninja-clang/            # Clang + Ninja 生成结果
└── src/
    ├── CMakeLists.txt          # 扫描 src 层源码，并递归添加子目录
    ├── main.cpp                # 程序入口，调用 message / sort 工具
    └── utils/
        ├── CMakeLists.txt      # 扫描 utils 层源码，并递归添加子目录
        ├── message/
        │   ├── CMakeLists.txt  # 把 message 源文件编译为静态库 message_utils
        │   ├── message.h
        │   └── message.cpp
        └── sort/
            ├── CMakeLists.txt  # 把 sort 源文件编译为静态库 sort_utils
            ├── sort_algorithms.h
            └── sort_algorithms.cpp
```

## 功能说明

- `main.cpp` 是程序入口，依次执行：
  - 打印 `Hello, World!`
  - 调用 `build_message()` 输出一条提示信息（来自 `message` 模块）
  - 对一组整数分别运行 5 种排序算法（冒泡、选择、插入、归并、快速），并用 `std::is_sorted` 校验结果
- `utils/message`：提供 `build_message()` 函数，返回示例提示字符串。
- `utils/sort`：在 `sort_algorithms` 命名空间下提供 5 种排序算法的实现，均返回排序后的 `std::vector<int>` 副本。

## 构建与运行

本项目使用 `CMakePresets.json` 管理工具链，支持两种预设：

### 1. MinGW + GCC

```bash
cmake --preset mingw
cmake --build --preset mingwbuild
```

### 2. Ninja + Clang

```bash
cmake --preset ninja-clang
cmake --build --preset clangbuild
```

构建完成后，在对应 `build/<preset>/` 目录下可找到可执行文件 `cmake-demo`（或 `cmake-demo.exe`）并运行。

## 技术要点

- **C++ 标准**：固定使用 C++17（`CMAKE_CXX_STANDARD 17`，且 `REQUIRED ON`、`EXTENSIONS OFF`）。
- **分层 CMake**：根目录创建可执行目标，各子目录的 `CMakeLists.txt` 负责扫描本层源码并递归加入下级目录。
- **静态库组织**：`message` 与 `sort` 各自编译为静态库（`message_utils`、`sort_utils`），再由根目标通过 `target_link_libraries` 链接。
- **自动扫描源文件**：使用 `file(GLOB ... CONFIGURE_DEPENDS)` 自动收集本层 `.cpp/.h` 等源文件，减少手动维护文件列表的工作量。
- **导出编译命令**：预设中开启 `CMAKE_EXPORT_COMPILE_COMMANDS=ON`，便于 VS Code 的 C++ 插件（如 clangd / IntelliSense）进行代码补全与跳转。

## 相关文档

- [cmake 使用说明（自动扫描子目录与静态库）](./cmake-usage.md)
