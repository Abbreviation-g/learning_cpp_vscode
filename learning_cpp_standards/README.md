# C++ 标准学习集合 (按标准拆分的 5 个独立 CMake 工程)

本目录原来是一个单一的 CMake 工程 (`cpp23`), 现在按 C++ 标准拆分成 **5 个互相独立的
CMake 工程**, 每个工程都自带完整的 CMake / 编辑器配置。

```
.
├── CMakeLists.txt                  # 汇总工程: 只做 add_subdirectory, 无 target
├── CMakePresets.json               # 汇总工程的 presets (整体构建用)
├── cpp-standards.code-workspace    # 多根工作区: 5 个子工程各作为一个 folder
├── .clang-format / .clang-tidy     # 根目录共用副本 (真正生效的是各子工程内的)
├── .clangd                         # 根目录 clangd 配置 (指向汇总 compile_commands.json)
├── .gitignore
├── testdata/
├── cpp11/                          # 独立的 C++11 工程
├── cpp14/                          # 独立的 C++14 工程
├── cpp17/                          # 独立的 C++17 工程
├── cpp20/                          # 独立的 C++20 工程
└── cpp23/                          # 独立的 C++23 工程
```

## 每个子工程的结构

以 `cpp17/` 为例, 每个子工程都是一个**完整且自包含**的 CMake 工程:

```
cpp17/
├── CMakeLists.txt          # 工程定义, 每个 <feature>/main.cpp 一个可执行 target
├── CMakePresets.json       # mingw / ninja-clang 两套 configure+build preset
├── .clangd                 # clangd: 指向 build/mingw/compile_commands.json
├── .clang-format           # clang-format 格式定义
├── .clang-tidy             # clang-tidy 检查规则
├── .vscode/
│   ├── settings.json       # clangd / clang-format / 终端 配置
│   ├── tasks.json          # configure / build / clean / clang-tidy 任务
│   └── launch.json         # 本工程各 target 的调试配置
├── cpp17_lambda/main.cpp
├── cpp17_if_switch/main.cpp
├── cpp17_structured_binding/main.cpp
├── cpp17_byte/main.cpp
└── cpp17_charconv/main.cpp
```

## 各工程的语言标准

| 工程    | CMAKE_CXX_STANDARD | 说明 |
| ------- | ------------------ | ---- |
| `cpp11` | 11                 | 4 个 target |
| `cpp14` | 14                 | 10 个 target |
| `cpp17` | 17                 | 5 个 target |
| `cpp20` | 20                 | 5 个 target |
| `cpp23` | 23                 | `cpp23` (原 `src/main.cpp`) + `cpp23_import` (`import std;`) |

> `cpp14` 工程中 `cpp14_aggregate_class` 与 `cpp14_shared_lock` 两个 target 用到了
> C++20 的 `std::format`, 因此它们通过 `target_compile_features(... cxx_std_20)`
> 单独声明所需标准, 工程默认标准仍然是 C++14。

## 构建方式

### 方式一: 打开多根工作区 (推荐)

用 VS Code 打开 `cpp-standards.code-workspace`, 侧边栏会出现 5 个 folder。
在每个 folder 上分别执行:

- `CMake: Select Configure Preset` → `mingw`
- `CMake: Configure`
- `CMake: Build`

产物落在各子工程的 `cppXX/build/mingw/bin/`。

也可以直接运行各 folder 下的 `CMake: build (mingw build)` task。

### 方式二: 单独打开某个子工程

直接用 VS Code 打开 `cpp17/` 等子目录即可, 该工程自带全部配置, 无需依赖根目录。

```powershell
cd cpp17
cmake --preset mingw
cmake --build --preset mingwbuild
```

### 方式三: 命令行整体构建 (汇总工程)

```powershell
cmake --preset mingw
cmake --build --preset mingwbuild
```

产物落在 `build/mingw/cppXX/bin/`。

> 汇总工程与单独打开子工程使用的是**两套独立的构建树**:
> 整体构建 → `build/<preset>/cppXX/`, 单独构建 → `cppXX/build/<preset>/`。

## C++23 模块 (`import std;`)

`cpp23/cpp23_import/main.cpp` 使用真正的 C++23 模块。**std 模块由 CMake 自动构建**,
无需手动执行任何额外步骤 —— 直接构建 `cpp23_import` 即可:

```powershell
cd cpp23
cmake --preset mingw
cmake --build --preset mingwbuild
```

CMake 会在该工程的二进制目录 (`cpp23/build/mingw/`) 下执行等价于下面的命令,
生成 `gcm.cache/std.gcm`:

```
g++ -std=c++23 -fmodules-ts -fsearch-include-path -c <prefix>/include/c++/<ver>/bits/std.cc
```

实现要点(见 `cpp23/CMakeLists.txt`):
- 用 `g++ -print-file-name=include` 反推 GCC 前缀, 再用 GLOB 找到
  `<prefix>/include/c++/*/bits/std.cc` (注意它**不在** `-print-file-name=include`
  给出的那个目录里)。
- 自定义命令的 `WORKING_DIRECTORY` 设为 `${CMAKE_CURRENT_BINARY_DIR}`, 与编译
  `cpp23_import` 时的工作目录一致, 因此默认模块映射即可找到缓存, 无需额外 mapper。
- 用 `OBJECT_DEPENDS` (而不是仅 `add_dependencies`) 把 `std.gcm` 绑定到目标文件,
  确保 std 模块**一定先于** `main.cpp` 的编译完成。

> 关键点: std 模块必须与引用它的编译单元使用**完全相同的 `-std` 方言**, 否则会报
> "language dialect differs" 错误。修改标准后需删除 `gcm.cache` 再重建。

> 手工构建(备用): 若自动检测失败, 可运行 task **`GCC: build std module (mingw)`**,
> 或手工执行:
> ```powershell
> cd cpp23/build/mingw
> g++ -std=c++23 -fmodules-ts -fsearch-include-path -c bits/std.cc
> ```

## 调试

每个子工程的 `.vscode/launch.json` 只包含本工程的调试配置, 按 F5 选择对应
target 即可 (例如 `调试 cpp17_lambda (F5)`)。

## clang-tidy

每个子工程有一个独立的 clang-tidy task, 只检查本工程的源文件:

| 工程    | task label |
| ------- | ---------- |
| `cpp11` | `clang-tidy: check sources (C++11)` |
| `cpp14` | `clang-tidy: check sources (C++14)` |
| `cpp17` | `clang-tidy: check sources (C++17)` |
| `cpp20` | `clang-tidy: check sources (C++20)` |
| `cpp23` | `clang-tidy: check sources (C++23)` |

运行前请先完成该工程的 CMake configure (需要 `build/mingw/compile_commands.json`)。
