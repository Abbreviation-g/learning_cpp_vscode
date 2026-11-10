#pragma once

// 可以在头文件中定义！
// 全局变量
inline int global_counter = 0;
inline const double PI = 3.141592653589793;
// 类的静态成员变量
struct Config {
    inline static int max_connections = 200;
};