#include <array>
#include <charconv>
#include <cstddef>
#include <iostream>
#include <string>
#include <system_error>
// https://subingwen.cn/cpp/u8_0x_literal/
//  UTF-8字面量和十六进制浮点数字面量

// 2. UTF-8字面量
namespace ns1 {
    void test() {
        using namespace std::string_literals;

        auto s1 = u8"Hello"s; // UTF-8字符串
        auto s2 = u8"你好世界"s; // UTF-8中文字符串
        auto s3 = u8"😀 Hello"s; // 带表情的字符串

        // 写法 A：普通字符串（"本地模式"）
        std::string s11 = "你好";
        // 问题：如果在中文 Windows 下编译，它是 GBK；在 Linux 下编译，它是 UTF-8。
        // 容易导致乱码！

        // 写法 B：u8 字符串（"强制 UTF-8 模式"）
        std::string s22 = u8"你好";
        // 保证：无论在哪里编译，它永远存成 UTF-8 的格式。
        // 安全！放心！

        // C++17 完全合法
        std::string msg = u8"这是一条消息";
        std::cout << msg << "\n"; // 只要你的终端支持 UTF-8，就能正常显示中文
    }
} // namespace ns1

// 3. 十六进制浮点数字面量
// 在 C++17 之前，我们写的浮点数都是十进制的：

// 0.5
// 3.14159
// 1.2e-4 (科学计数法)
// 这对于数学计算很友好，但对于计算机底层开发（如嵌入式、驱动、图形学）非常不友好。因为计算机内部是用二进制存储浮点数的。比如，想表示十进制
// 0.1，在二进制里是一个无限循环小数，不仅存储有精度误差，而且很难从字面上看出它对应的二进制位是什么。这就像用分数1/3表示成十进制小数（0.33333…），永远无法精确。十六进制浮点字面量可以精确控制浮点数的二进制表示，避免编译器将十进制数转换为二进制时产生的歧义或额外误差。

// C++17 允许我们直接用十六进制来写浮点数！这让我们能精确控制数字的二进制表示。

// 语法格式：0x + 十六进制整数部分 + .+ 十六进制小数部分 + p + 指数

// 前缀：必须以 0x 或 0X 开头。
// 底数：中间是十六进制数（0-9, A-F），可以包含小数点。
// 指数标志：必须用 p 或 P（代表 Power，2的幂），千万不能用 e（e 是十进制科学计数法）。
// 指数值：p 后面跟的是十进制整数，代表 2 的多少次方。
// 写法	含义解析	十进制结果
// 0x1p0	1×20	1.0
// 0x1p4	1×24 (即 1×16)	16.0
// 0x1.8p1	1.5 (十六进制 0.8 等于十进制的 0.5)×21	3.0
// 0xAp2	10 (十六进制 A)×22	40.0
// 0x8p-1	8×2−1 (即 8×0.5)	4.0
namespace ns2 {
    void test() {
        double value = 0.1;

        // 缓冲区
        std::array<char, 32> buf{};
        // C++17 的 std::to_chars std::chars_format::hex 是关键
        auto result =
            std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::hex);

        if (result.ec == std::errc()) {
            // 注意：这里生成的是 "1.999999999999ap-4" (没有 0x 前缀)
            // 你可能需要手动加上 "0x"
            std::cout << "Hex float literal: 0x" << std::string(buf.data(), result.ptr)
                      << std::endl;
            // 输出: 0x1.999999999999ap-4
        } else {
            std::cout << "转换失败\n";
        }

        // struct to_chars_result
        // {
        //     char* ptr;      // 指向写入结束位置的下一个字符的指针
        //     std::errc ec;   // 错误码，若转换成功则为 std::errc()
        // };
    }

    void test2() {
        // 1. 整数转十六进制
        std::array<char, 10> buf{};
        auto res = std::to_chars(buf.data(), buf.data() + buf.size(), 255, 16);
        if (res.ec == std::errc()) {
            // 手动添加 null 终止符以便打印
            *res.ptr = '\0';
            std::cout << "Hex: " << buf.data() << "\n";
        }

        // 2. 浮点数转科学计数法 (假设支持 C++17 浮点重载)
        // 注意：不同编译器对浮点 to_chars 的支持进度不同
        std::array<char, 20> fbuf{};
        double val = 3.14159;
        auto res_f = std::to_chars(
            fbuf.data(), fbuf.data() + fbuf.size(), val, std::chars_format::scientific, 2);
        if (res_f.ec == std::errc()) {
            *res.ptr = '\0';
            std::cout << "Sci: " << fbuf.data() << std::endl; // 可能输出: 3.14e+00
        }
    }
} // namespace ns2
int main() {
    ns1::test();

    ns2::test();
    ns2::test2();
}