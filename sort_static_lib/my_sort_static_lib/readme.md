# 1.config并编译
```bash
cmake -S . --preset mingw
cmake --build --preset mingwbuild
```
# 2.安装

## 1.配置安装前缀
```bash
# 方法1: 配置时直接指定安装
cmake -S . -B build/mingw -DCMAKE_INSTALL_PREFIX="C:/install/my_sort_static_lib"
# 方法2: 不指定安装前缀，使用preset配置CMAKE_INSTALL_PREFIX变量
cmake -S . --preset mingw
```

## 2.安装命令
```bash
# 方法一
cmake --build build/mingw --target install
# 方法二
cmake --install build/mingw
```

## 3.不配置安装前缀，命令行指定安装前缀直接安装
```bash
cmake --install build/mingw --prefix "C:/install/my_sort_static_lib"
```