#include <iomanip>
#include <ios>
#include <iostream>

// https://subingwen.cn/cpp/binary-literal/

namespace basic_syntax {
    void binary_literal() {
        //     1. 基本语法
        // C++14
        // 引入了二进制字面量，允许开发者直接使用二进制数字表示整数，使代码在涉及位操作时更加直观和可读。

        // 下面是基本语法格式：
        int binary1 = 0b1010;
        int binary2 = 0B1101;
        std::cout << "Binary 1: " << binary1 << std::endl;
        std::cout << "Binary 2: " << binary2 << std::endl;

        //     2. 其它字面量修饰符
        // 2.1 有符号/无符号
        // 在C++14 中二进制字面量还可以配合有符号/无符号修饰符使用：
        auto x = 0b1010u;                           // 无符号整数
        auto y = 0b1101ul;                          // 无符号长整数
        auto z = 0b1101ull;                         // 无符号长整数
        std::cout << typeid(x).name() << std::endl; // 输出类型信息
        std::cout << typeid(y).name() << std::endl; // 输出类型信息
        std::cout << typeid(z).name() << std::endl; // 输出类型信息
        // 为了方便查阅，这里列出了一些常见的基础类型对应的字符（对应 typeid(z).name()
        // 输出的结果）： 字符	对应的 C++ 类型 v	void b	bool c	char a
        // signed char h	unsigned char s	short t	unsigned short i	int j
        // unsigned int l	long m	unsigned long y	unsigned long long f	float d
        // double Pi	int* (指针类型通常在前面加 P)

        // 2.2 数值分割符
        // C++14 还引入了数字分隔符
        // ‘（即单引号），可以与二进制字面量结合使用，这样可以提供可读性（不影响数值）：
        int mask1 = 0b1000'1001'1010; // 使用单引号分隔二进制数字
        std::cout << "Binary with separator: " << mask1 << std::endl;
        // 数值分割符除了在二进制数据中使用，也可以在其它进制的数值中使用，比如：
        long long bigNumber = 1'000'000'000; // 长整形, 10进制
        double pi = 3.14159'26535'89793;     // 浮点型, 10进制
        int hex = 0xFF'FF'FF'FF;             // 16进制
        int binary = 0b1000'0001'1000'0000;  // 2进制
        int octal1 = 012'345;                // 八进制 12345 = 十进制 5349
        int octal2 = 07'77'777;              // 八进制 777777 = 十进制 262143
    }
} // namespace basic_syntax

namespace usage_bit_flag {
    enum FilePermissions { READ = 0b001, WRITE = 0b010, EXECUTE = 0b100 };
    auto test() -> auto {
        // 设置权限
        int user_perms = 0b111;  // 读、写、执行
        int group_perms = 0b101; // 读、执行
        int other_perms = 0b001; // 只读

        // 检查权限
        if (user_perms & FilePermissions::WRITE) {
            std::cout << "用户有写权限" << std::endl;
        }
    }
} // namespace usage_bit_flag

namespace usage_color {
    struct Color {
        unsigned int argb;
        static constexpr unsigned int ALPHA_MASK = 0xFF000000;
        static constexpr unsigned int RED_MASK = 0x00FF0000;
        static constexpr unsigned int GREEN_MASK = 0x0000FF00;
        static constexpr unsigned int BLUE_MASK = 0x000000FF;
        Color(unsigned char a, unsigned char r, unsigned char g, unsigned char b) {
            argb = (static_cast<unsigned int>(a) << 24) | (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) |
                   static_cast<unsigned int>(b);
        }
        unsigned char getAlpha() const { return static_cast<unsigned char>(argb >> 24); }
        unsigned char getRed() const { return static_cast<unsigned char>((argb & RED_MASK) >> 16); }
        unsigned char getGreen() const { return static_cast<unsigned char>((argb & GREEN_MASK) >> 8); }
        unsigned char getBlue() const { return static_cast<unsigned char>(argb & BLUE_MASK); }
        void print() const { std::cout << "ARGB: 0X" << std::hex << std::setfill('0') << std::setw(8) << argb << std::dec << std::endl; }
    };

    void test() {
        Color col(125, 200, 100, 255);
        col.print();
    }
} // namespace usage_color

int main() {
    basic_syntax::binary_literal();
    usage_bit_flag::test();
    usage_color::test();

    return 0;
}
