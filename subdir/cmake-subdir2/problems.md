# C++ CMake 子目录项目调试问题排查记录

## 环境

- **OS**: Windows
- **IDE**: VS Code
- **项目**: cmake-subdir2（CMake + 子目录结构）
- **编译器 (MinGW)**: g++ (MSYS2/MinGW)
- **编译器 (Ninja/Clang)**: clang++ 15.0.1, Target: `x86_64-pc-windows-msvc`
- **构建系统**: Ninja
- **链接器 (Clang)**: lld-link

---

## 问题1：修改 launch.json 时报错 vulncheck

**报错信息：**
```
Invalid settings: setting option "vulncheck": invalid option "Prompt" for enum
```

**原因：** Go 扩展 v0.40+ 中 `go.vulncheck` 设置不再接受 `"Prompt"` 值（只接受 `"Off"` 和 `"Imports"`）。VS Code 保存任意配置文件时会校验所有扩展的设置。

**解决：** 在全局 `settings.json` 中添加 `"go.vulncheck": "Off"`

---

## 问题2：Ninja/Clang 调试报错 "cmake_subdir2.exe is missing or invalid"

**报错信息：**
```
cmake_subdir2.exe is missing or invalid
```

**排查过程：**

1. 确认 exe 文件存在且可正常运行 → 文件没问题
2. 确认无残留进程占用 → 进程没问题
3. 检查 clang 编译目标 → `x86_64-pc-windows-msvc`（MSVC ABI）
4. 检查 gdb 版本 → GDB 7.6.1 (2013)

**根因：** clang 编译使用 MSVC ABI + lld-link 链接，生成 PE 格式二进制。GDB 7.6.1 不支持调试 MSVC ABI 格式的二进制文件。

---

## 问题3：改用 cppvsdbg 后能启动但无法进入源码

**现象：** 调试器启动成功，停在入口点，但源码不显示，断点不命中。

**排查过程：**

检查 `build.ninja` 编译标志 → 发现之前的 `-g -gdwarf`（DWARF 格式）残留在 CMake 缓存中：
```
FLAGS = -g -gdwarf -std=gnu++17 ...
```

**根因：** `cppvsdbg`（VS 原生调试引擎）需要 **CodeView (PDB)** 格式的调试信息才能进行源码行映射。DWARF 调试信息嵌入在 exe 中，但 `cppvsdbg` 无法从中读取源码行号。

**验证：** 用 dumpbin 检查 exe，确认调试信息是 DWARF 格式（`.debug_abbrev`, `.debug_info` 等 section），PDB 中没有正确的源码映射。

---

## 问题4：改用 LLDB 调试器报错连接失败

**报错信息：**
```
unable to establish a connection to lldb
```

**原因：** `lldb-mi.exe`（LLDB Machine Interface）不存在。cppdbg 的 `MIMode: "lldb"` 需要 `lldb-mi` 作为后端，但 LLVM 15.0.1 安装中没有包含该组件。

---

## 最终解决方案

### 修改的文件

#### 1. `.vscode/launch.json` — Ninja/Clang 配置

```jsonc
{
  "name": "Debug cmake_subdir2 (Ninja/Clang)",
  "type": "cppvsdbg",          // ← VS 原生调试器，兼容 MSVC ABI 二进制
  "request": "launch",
  "program": "${workspaceFolder}/build/ninja-clang/cmake_subdir2.exe",
  "args": [],
  "stopAtEntry": true,         // ← 先停在入口点，验证调试器可用
  "cwd": "${workspaceFolder}",
  "environment": [],
  "externalConsole": false,
  "sourceFileMap": {           // ← 映射源码路径（大小写/斜杠差异时有用）
    "C:/source/learning_cpp_vscode/cmake-subdir2": "${workspaceFolder}"
  },
  "preLaunchTask": "CMake: build (ninja-clang)"
}
```

#### 2. `CMakePresets.json` — 修正 C 编译器

`"CMAKE_C_COMPILER": "c++"` → `"CMAKE_C_COMPILER": "clang"`

### 关键步骤

1. **彻底清除 CMake 缓存：**
   ```
   Remove-Item build/ninja-clang/CMakeCache.txt
   Remove-Item -Recurse build/ninja-clang/CMakeFiles
   ```

2. **重新配置和构建（使用默认 Debug 标志，即 CodeView 格式）：**
   ```
   cmake --preset ninja-clang
   cmake --build --preset ninja-clang
   ```

3. **验证编译标志已恢复：**
   ```
   FLAGS = -O0 -g -Xclang -gcodeview -std=gnu++17 ...
   ```

---

## 经验总结

| 要点 | 说明 |
|------|------|
| **调试器必须匹配 ABI** | MSVC ABI (clang + lld-link) → 用 `cppvsdbg`；MinGW ABI → 用 `cppdbg` + `gdb` |
| **调试格式必须匹配调试器** | `cppvsdbg` 需要 CodeView (PDB)；`gdb` 需要 DWARF |
| **CMake 缓存是陷阱** | 改 `CMakePresets.json` 或 `CMakeLists.txt` 后要清除缓存 (`CMakeCache.txt` + `CMakeFiles`)，否则旧标志仍生效 |
| **clang on Windows 默认 MSVC ABI** | 直接用 `clang++` 不指定 target 时，默认使用 MSVC 工具链和 lld-link |
| **lldb-mi 不一定存在** | Windows 上 LLVM 安装可能不含 `lldb-mi.exe`，不能用 `MIMode: "lldb"` |
