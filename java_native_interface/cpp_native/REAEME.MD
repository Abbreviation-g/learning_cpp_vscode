# JNI Hello 动态库

基于 CMake 构建的 JNI C++ 动态库，供 Java 通过 JNI 调用。

## 前置条件

- CMake >= 3.14
- JDK（确保 `JAVA_HOME` 环境变量已设置）
- C++ 编译器（MSVC / MinGW / GCC）

## 构建方式

### 方法 1：命令行构建

**Windows (PowerShell):**
```powershell
# 确保 JAVA_HOME 已设置，例如：
$env:JAVA_HOME = "C:\sw\jdk-17.0.4.1"

# 配置
cmake -B build -S .

# 构建
cmake --build build --config Release
```

**Linux/macOS:**
```bash
# 配置
cmake -B build -S .

# 构建
cmake --build build
```

### 方法 2：VS Code CMake Tools

在 VS Code 中安装 **CMake Tools** 扩展，打开项目后会自动提示配置和构建。

## 输出

- **Windows**: `build/Release/hello.dll`（Release）/ `build/Debug/hello.dll`（Debug）
- **Linux**: `build/libhello.so`
- **macOS**: `build/libhello.dylib`

## 原始 gcc 命令（参考）

```bash
gcc -I"C:\sw\jdk-17.0.4.1\include" -I"C:\sw\jdk-17.0.4.1\include\win32" --shared -o hello.dll src/com_Hello.cpp
```