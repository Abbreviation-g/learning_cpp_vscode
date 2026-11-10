#include <iostream>
#include <string>
#include "constants.hpp"
// https://subingwen.cn/cpp/inline-variables/
//  内联变量

// 1.2 什么是内联变量
// 内联变量是 C++17 引入的特性，它允许我们在头文件中定义变量，而不会违反 ODR
// 规则。当多个源文件包含同一个定义了内联变量的头文件时，编译器保证只有一个实体存在。
// 定义内联变量的基本语法是在变量前加上 inline 关键字：

namespace ns1 {
    void test() {
        int i = default_value<int>;
        double d = default_value<double>;
        std::string s = default_value<std::string>;
        const char *c = default_value<const char *>;
    }
}; // namespace ns1
int main() {
    ns1::test();
}