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

将 `MinGW Makefiles` generator 搭配 MinGW 的 `mingw32-make.exe` 使用：

```json
"generator": "MinGW Makefiles",
"cacheVariables": {
    "CMAKE_CXX_COMPILER": "g++",
    "CMAKE_C_COMPILER": "gcc",
    "CMAKE_MAKE_PROGRAM": "C:/MinGW/bin/mingw32-make.exe"
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

## 3. 调试时 stdout 没有显示在 VS Code Terminal 面板

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

## 最终可用配置

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
        "CMAKE_MAKE_PROGRAM": "C:/MinGW/bin/mingw32-make.exe",
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
            "miDebuggerPath": "C:\\MinGW\\bin\\gdb.exe",
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
