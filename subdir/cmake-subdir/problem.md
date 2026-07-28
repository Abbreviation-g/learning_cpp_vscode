# CMake MinGW VS Code 调试问题记录

## 1. `cmake --build --preset mingw` 使用 MSYS make 时失败

### 现象

CMake 在检测编译器或 try-compile 阶段构建失败：

```text
'mTC_xxxx.dir' is not recognized as an internal or external command
make: *** [cmTC_xxxx/fast] Error 1
```

### 原因

工程使用的是 `MinGW Makefiles` generator，但 `CMAKE_MAKE_PROGRAM` 指向了：

```text
C:/MinGW/msys/1.0/bin/make.exe
```

`MinGW Makefiles` 应该使用 MinGW 原生的 make 程序。MSYS `make.exe` 可能会错误
处理该 generator 生成的 Windows 风格路径和反斜杠。

### 解决方法

将 `MinGW Makefiles` generator 搭配 MinGW-w64 的 `mingw32-make` 使用。当前工程通过
PATH 查找 MSYS2 UCRT64 中的工具：

```json
"generator": "MinGW Makefiles",
"cacheVariables": {
    "CMAKE_CXX_COMPILER": "g++",
    "CMAKE_C_COMPILER": "gcc",
    "CMAKE_MAKE_PROGRAM": "mingw32-make"
}
```

修改 make 程序后，需要删除旧的 CMake 缓存或重建构建目录，因为 CMake 会缓存
`CMAKE_MAKE_PROGRAM`。

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
cmake --build --preset mingw
```

## 2. VS Code 调试时没有进入断点

### 现象

VS Code 可以启动调试，但断点不能稳定命中。

### 原因

CMake preset 使用的是：

```json
"CMAKE_BUILD_TYPE": "Release"
```

Release 构建会开启优化，并且通常不适合源码级断点调试。

### 解决方法

VS Code 调试时使用 Debug 构建：

```json
"CMAKE_BUILD_TYPE": "Debug"
```

然后重新生成和构建：

```powershell
cmake --preset mingw
cmake --build --preset mingw
```

Debug 构建会生成带 `-g` 调试信息的二进制文件，GDB 才能把断点绑定到源码位置。

## 3. 切换到 MSYS2 UCRT64 后 configure 找不到编译器或 make

### 现象

删除旧的 `C:\MinGW` 并把 `C:\msys64\ucrt64\bin` 加入环境变量后，VS Code task
仍然可能配置失败，例如：

```text
CMake was unable to find a build program corresponding to "MinGW Makefiles".
CMAKE_MAKE_PROGRAM is not set.
```

或者：

```text
The C++ compiler "g++" is not able to compile a simple test program.
```

### 原因

VS Code 启动时会继承当时的系统环境变量。如果是在 VS Code 已经打开后才修改 PATH，
当前 VS Code 进程仍然使用旧 PATH，`gcc`、`g++`、`mingw32-make`、`gdb` 可能无法
解析到 `C:\msys64\ucrt64\bin`。

### 解决方法

关闭所有 VS Code 窗口后重新打开工程，让 VS Code 重新读取系统环境变量。

重启后，在 VS Code 终端中确认：

```powershell
gcc --version
g++ --version
mingw32-make --version
gdb --version
```

如果之前已经用旧工具链配置过，还需要删除旧缓存：

```powershell
Remove-Item .\build\mingw -Recurse -Force
cmake --preset mingw
```

## 4. 调试时 stdout 没有显示在 VS Code Terminal 面板

### 现象

使用如下 launch 配置时：

```json
"externalConsole": false
```

断点可以命中，但 `std::cout` 输出没有显示在 VS Code 的 Terminal 面板。

### 原因

在 Windows 上使用 `cppdbg` + MinGW GDB 时，stdout 的显示位置由 VS Code C++
扩展控制。即使 `externalConsole` 为 `false`，输出也可能进入 Debug Console，
而不是 Terminal 面板。这个行为和 VS Code C++ 扩展、GDB 版本以及 Windows 控制台
重定向机制有关。

### 解决方法

保持 VS Code 内部调试，并让程序输出显示到 Debug Console：

```json
"externalConsole": false,
"avoidWindowsConsoleRedirection": false,
"internalConsoleOptions": "openOnSessionStart",
"logging": {
    "programOutput": true
}
```

调试时查看 Debug Console 面板即可看到 stdout。

如果必须使用真实控制台窗口查看 stdout，可以改为：

```json
"externalConsole": true
```

这样会打开独立控制台窗口，而不是使用 VS Code 内部 Terminal 面板。

## 当前可用配置

### `CMakePresets.json`

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "mingw",
      "displayName": "MinGW Makefiles",
      "generator": "MinGW Makefiles",
      "binaryDir": "${sourceDir}/build/mingw",
      "cacheVariables": {
        "CMAKE_CXX_COMPILER": "g++",
        "CMAKE_MAKE_PROGRAM": "mingw32-make",
        "CMAKE_C_COMPILER": "gcc",
        "CMAKE_BUILD_TYPE": "Debug"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "mingw",
      "configurePreset": "mingw"
    }
  ]
}
```

### `.vscode/launch.json`

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug cmake_subdir_demo",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}\\build\\mingw\\src\\cmake_subdir_demo.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "avoidWindowsConsoleRedirection": false,
            "internalConsoleOptions": "openOnSessionStart",
            "logging": {
                "programOutput": true
            },
            "MIMode": "gdb",
            "miDebuggerPath": "gdb",
            "preLaunchTask": "CMake: build mingw",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```
