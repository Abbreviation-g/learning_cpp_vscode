#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
// https://subingwen.cn/cpp/constexpr-if/
//  constexpr if

// 1. 什么是 constexpr if
// 简单来说，constexpr if 是 C++17
// 引入的一个编译期判断语句。它的作用是告诉编译器：在编译代码的时候，就根据这个条件决定保留哪一段代码，扔掉哪一段代码。
// constexpr if
// 允许我们将那些对当前类型来说非法的代码直接在编译阶段剔除掉，于是编译器就不会检查那些被剔除的代码的语法正确性。

// 它的写法和普通 if 几乎一样，只是开头加了一个 constexpr 关键字：

// 注意：constexpr 在 if 的后面，不是前面！
//  普通 if：条件在运行时判断。两个分支（if-else）都会被编译，只是运行时只走一个。
//  constexpr if：条件在编译期判断。如果不满足条件，那个分支的代码根本不会被编译。
//  特性	传统 if	constexpr if
//  判断时间	运行时	编译时
//  性能影响	需要判断分支	零运行时开销
//  代码编译	所有分支都编译	只编译真分支
//  适用场景	普通条件判断	模板、编译时常量

// 2.1 基础用法
namespace ns1 {
    //     std::is_integral_v 是 C++17
    //     中引入的一个模板变量，用于在编译时检查一个类型是否为整数类型。
    // std::is_integral_v<T>对以下类型返回 true：
    // bool
    // char, signed char, unsigned char
    // char8_t (C++20)
    // char16_t, char32_t, wchar_t
    // short, unsigned short
    // int, unsigned int
    // long, unsigned long
    // long long, unsigned long long

    template<typename T>
    void processValue(const T &value) {
        if constexpr (std::is_integral_v<T>) {
            // 如果 T 是 int, long 等，这段代码保留
            std::cout << "这是整数: " << value << "*2=" << value * 2 << "\n";
        } else if constexpr (std::disjunction_v<std::is_same<T, std::string>,
                                 std::is_same<T, const std::string>>) {
            // 如果 T 是 std::string，这段代码保留
            std::cout << "这是字符串: " << value << ", length=" << value.length() << '\n';
        } else {
            std::cout << "其它类型" << "\n";
        }
    }

    void test() {
        // 情况 1：传入整数
        processValue(10);
        // 编译器在这里只生成了整数版本的代码

        // 情况 2：传入字符串
        processValue(std::string("Hello C++17"));
        // 编译器在这里只生成了字符串版本的代码

        const std::string s2{ "xxxx" };
        processValue(s2);

        auto s3 = "xxxx";
        processValue(s3); // 其它类型
    }

    //     2.2 让非法代码变得合法
    // 看下面的代码，我们尝试获取一个值的 .size()
    // 泛型函数, 试图打印元素的个数
    template<typename T>
    void printCount(const T &container) {
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            // 只有当 T 是 vector 时，这段代码才参与编译
            // 如果不满足条件，编译器会直接无视这段代码，
            std::cout << "Vector 大小: " << container.size() << std::endl;
        } else {
            // 如果不是 vector，执行这里
            std::cout << "这不是一个 Vector，无需计算大小" << std::endl;
        }
    }
    struct MyStruct {
        int id;
    };
    void test2() {
        std::vector myVec{ 1, 2, 3 };
        MyStruct s{ 100 };
        printCount(myVec); // 走 if 分支，打印 3
        printCount(s); // 走 else 分支
    }

    template<typename T, std::enable_if_t<std::is_convertible_v<T, std::string>, int> = 0>
    void print_string_length(T &&value) {
        const std::string s = std::forward<T>(value);
        std::cout << "length: " << s.length() << "\n";
    }
    template<typename T>
    void print_string_length2(const T &value) {
        if constexpr (std::is_convertible_v<T, std::string>) {
            const std::string s = value;
            std::cout << "length: " << s.length() << "\n";
        } else {
            std::cout << "不支持其类型" << "\n";
        }
    }

    void test3() {
        print_string_length("hello"); // const char* 隐式转换为 std::string
        print_string_length(std::string("C++17"));

        print_string_length2("hello"); // const char* 隐式转换为 std::string
        print_string_length2(std::string("C++17"));
    }

    //     3. constexpr if 不是万能的
    // constexpr if 绝不是万能药，如果用得不好，反而会写出看似正确实则挂了的代码。

    // 下面的代码是一个错误示例：
    template<typename T>
    void bad_example(T value) {
        int x = 10;
        // 错误！x不是编译时常量
        // if constexpr (x > 5)  // ❌ 编译错误
        // {
        //     std::cout << "x > 5" << std::endl;
        // }
        // 正确：使用编译时常量
        constexpr int y = 10;
        if constexpr (y > 5) // ✅ 正确
        {
            std::cout << "y > 5" << std::endl;
        }
        const int z = 10;
        if constexpr (z > 5) // ✅ 正确
        {
            std::cout << "z > 5" << std::endl;
        }
    }

    class A {
    public:
        static int x;
        inline static int y = 20;
        static A instance;
    };
    inline A A::instance{};
    int A::x = 10;
} // namespace ns1
int main() {
    ns1::test();
    ns1::test2();
    ns1::test3();
}

// 使用constexpr
// if的时候它的条件要求非常严格，总结为一句话：条件必须是“编译期常量”且必须是“上下文转换为 bool
// 值”的表达式。这意味着编译器在编译代码时，必须能够直接计算出这个条件的结果是 true 还是
// false，而不能依赖运行时的变量。

// 必须是常量表达式
// 允许
// constexpr 变量：if constexpr (N > 0)
// enum 枚举值：if constexpr (Color::Red == 0)
// sizeof / alignof：if constexpr (sizeof(int) == 4)
// Type Traits (最常用)：if constexpr (std::is_pointer_v<T>)、if constexpr (std::is_integral_v<T>)。
// 不允许
// 普通变量：int x = 5; if constexpr (x > 0) ，x 的值虽然已知，但不是常量表达式。
// 函数参数：void func(int n) { if constexpr (n > 0) ... } ，n 是运行时传入的。
// 运行时函数结果：if constexpr (std::time(nullptr) > 0) ，函数返回值不是常量。
// 必须能隐式转换为 bool
// if constexpr (10) -> true
// if constexpr (nullptr) -> false
// if constexpr (std::is_integral_v<int>) -> true
// constexpr if 是一把锋利的手术刀，用来切除模板中的类型差异，但它不是锤子，不能到处乱砸。
