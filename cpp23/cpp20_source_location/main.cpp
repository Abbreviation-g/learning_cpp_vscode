#include <iostream>
#include <source_location>

// https://blog.csdn.net/fengbingchun/article/details/143082167
// C++20中头文件source_location的使用
//     <source_location>是C++20中新增加的头文件，此头文件是utility库的一部分。

// 主要内容为类
// std::source_location：表示有关源代码的某些信息，例如文件名(__FILE__)、行号(__LINE__)和函数名(__func__)。

namespace {

void log(
    const std::source_location location = std::source_location::current()) {
  std::clog << "file: " << location.file_name() << "(" << location.line() << ":"
            << location.column()
            << "); function name: " << location.function_name() << std::endl;
}

void func() { log(); }

} // namespace

int test_source_location() {
  log();
  func();

  return 0;
}

int main() {
  test_source_location();
  return 0;
}