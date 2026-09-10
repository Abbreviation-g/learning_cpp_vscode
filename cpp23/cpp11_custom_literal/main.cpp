#include <cstdio>
#include <iostream>

// https://subingwen.cn/cpp/user-literal/
// 1. 自定义字面量
// 自定义字面量是C++11引入的特性，允许程序员为字面量定义自己的后缀，从而创建具有特定类型和值的对象。它的基本语法如下：
// // 定义字面量运算符
// 返回值类型 operator"" 后缀名(参数类型 参数) {
//     // 转换逻辑
// }

// 如果详细进行划分，定义字面量的时候有四种重载形式：
// 整数字面量
// // 整数字面量
// ReturnType operator"" _suffix(unsigned long long);
// 浮点数字面量
// // 浮点字面量
// ReturnType operator"" _suffix(long double);
// 字符字面量
// // 字符字面量
// ReturnType operator"" _suffix(char);
// 字符串字面量
// // 字符串字面量
// ReturnType operator"" _suffix(const char*, size_t);
namespace basic_syntax {
    // 1.整数参数-处理整数字面量, 千位转换为个位
    constexpr unsigned long long operator""_k(unsigned long long value) {
        return value * 1000;
    }
    // 2.浮点数参数-处理浮点数字面量， 千米转换为米
    constexpr long double operator""_km(long double value) {
        return value * 1000.0;
    }
    // 3.字符参数-处理字符字面量， 将小写字母转换为大写字母
    constexpr char operator""_upper(char c) {
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 'A';
        }
        return c;
    }
    // 4.字符串参数-处理字符串字面量， 将字符串前后增加大括号
    std::string operator""_w(const char *str, size_t len) {
        return std::string("(") + std::string(str, len) + std::string(")");
    }
    // 5. 字符串参数-处理字符串字面量， 字符串重复3次
    std::string operator""_repeat(const char *str, size_t len) {
        std::string result;
        for (int i = 0; i < 3; ++i) {
            result.append(std::string(str, len));
        }
        return result;
    }

    // 测试以下
    void test() {
        // 1. 整数字面量, 将千位转换为个位
        unsigned long long distance = 5_k; // 5000
        std::cout << "Distance: " << distance << std::endl;

        // 2. 浮点数字面量, 将千米转换为米
        long double length = 3.56789_km; // 3500.0
        std::cout << "Length: " << length << std::endl;

        // 3. 字符字面量, 将小写字母转换为大写字母
        char upperChar = 'a'_upper; // 'A'
        std::cout << "Upper Char: " << upperChar << std::endl;

        // 4. 字符串字面量, 将字符串前后增加大括号
        const std::string wrappedStr = "Hello, World!"_w; // (Hello, World!)
        std::cout << "Wrapped String: " << wrappedStr << std::endl;

        // 5. 字符串字面量, 字符串重复3次
        std::string repeatedStr = "Hello, World!"_repeat; // Hello, World!Hello, World!Hello, World!
        std::cout << "Repeated String: " << repeatedStr << std::endl;

        // 6.字面量的组合
        std::string combinedStr = "Hello"_repeat + " " + "World"_w; // HelloHelloHello (World)
        std::cout << "Combined String: " << combinedStr << std::endl;
    }
} // namespace basic_syntax

namespace combined_syntax {
    struct MyColor {
        uint8_t r, g, b;
        void print() const {
            std::cout << "Color: (" << static_cast<int>(r) << ", " << static_cast<int>(g) << ", " << static_cast<int>(b) << ")" << std::endl;
        }
    };
    MyColor operator""_rgb(const char *str, size_t len) {
        // "255, 100, 100"
        int r, g, b;
        std::sscanf(str, "%d, %d, %d", &r, &g, &b);
        return MyColor{ static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b) };
    }
    MyColor operator""_rgb(unsigned long long value) {
        return MyColor{
            .r = (static_cast<uint8_t>((value >> 16) & 0xFF)), .g = (static_cast<uint8_t>((value >> 8) & 0xFF)), .b = (static_cast<uint8_t>(value & 0xFF))
        };
    }

    void test() {
        MyColor color = "255, 100, 101"_rgb;
        color.print(); // Color: (255, 100, 100)

        MyColor color2 = 0xFF6465_rgb;
        color2.print(); // Color: (255, 100, 101)
    }
} // namespace combined_syntax

int main() {
    basic_syntax::test();
    combined_syntax::test();
    return 0;
}
