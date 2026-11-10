#include "constants.hpp"
#include <cstddef>

// 传统方式仍然需要外部（源文件）定义
int Settings::screen_width = 1920; // 必须在某个源文件中

// 必须在源文件中定义
Singleton *Singleton::instance = nullptr;