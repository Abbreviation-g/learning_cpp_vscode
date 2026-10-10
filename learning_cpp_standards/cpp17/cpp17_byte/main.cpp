#include <cstdint>
#include <type_traits>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef byte // Windows SDK byte macro conflicts with std::byte
#endif

#include <bitset>
#include <cstddef>
#include <iostream>

// [C++17中std::byte的使用_c++
// byte-CSDN博客](https://blog.csdn.net/fengbingchun/article/details/134624568)
//  C++17中std::byte的定义如下：std::byte本质上代表一个字节的值

// enum class byte : unsigned char {};
// AI写代码
// cpp
// 运行
//       与unsigned
//       char一样，std::byte可以用来访问其它对象(对象表示)占用的原始内存(raw
//       memory),但与unsigned
//       char不同的是，它不是字符类型，也不是算术类型。std::byte模型只是位的集合，仅支持按位和比较操作。

//       函数std::is_integral_v<T>定义如下：判断T是不是整型，如T为整型，则返回true；如果T的类型为bool、char、char16_t、char32_t、wchar_t、short、int、long、long
//       long，以及任何实现定义的扩展整数类型(any implementation-defined
//       extended integer
//       types)，包括任何有符号、无符号、和cv限定的变体(cv-qualified
//       variants),则返回true

// template< class T >
// inline constexpr bool is_integral_v = is_integral<T>::value;
// AI写代码
// cpp
// 运行
//       std::byte相关函数：
//       (1).std::to_integer<T>(std::byte):将std::byte对象转换为整型T,仅当is_integral_v<T>为true时有效；
//       (2).左移：<<=, 等价于: b = (std::byte)b << (T)shift;
//       仅当is_integral_v<T(shift)>为true时有效； (3).左移：<<，等价于:
//       std::byte(static_cast<unsigned int>(b) << (T)shift);
//       仅当is_integral_v<T(shift)>为true时有效； (4).右移：>>=, 等价于: b =
//       (std::byte)b >> (T)shift; 仅当is_integral_v<T(shift)>为true时有效；
//       (5).右移: >>, 等价于：std::byte(static_cast<unsigned int>(b) >>
//       (T)shift); 仅当is_integral_v<T(shift)>为true时有效；
//       (6).按位&=、|=、^=：等价于：l = l & r; l = l | r; l = l ^ r;
//       (7).按位&、|、^、~：等价于：std::byte(static_cast<unsigned int>(l) |
//       static_cast<unsigned int>(r))；等

//       注意：
//       (1).列表初始化(使用花括号初始化)是唯一可以直接初始化std::byte对象的方法。
//       (2).std::byte对象存储 在栈上。
//       (3).sizeof(std::byte)的大小总是1.
//       (4).大多数时候std::byte位数是8，但在有些平台上可能不是。
//       (5).用std::to_integer<>(std::byte)可以把std::byte转换为任意基本整数类型(bool、字符类型或者整数类型)。
//       (6).std::byte没有定义输入和输出运算符
//       ，因此不得不把它转换为整数类型再进行I/O。

//       以下为测试代码：

// helper: print std::byte as binary
namespace {
    std::ostream &operator<<(std::ostream &os, std::byte b) {
        return os << std::bitset<8>(std::to_integer<int>(b));
    }
} // namespace

void test_byte() {
    std::cout << "=== C++17 std::byte Demo ===\n\n";

    // std::byte is enum class byte : unsigned char {};
    // Only brace-init allowed, only bitwise ops supported

    // --- init & left shift ---
    std::byte b{ 66 }; // 66 = 0b01000010
    std::cout << "b:     " << b << "\n"; // 01000010

    std::byte b2 = b << 1;
    std::cout << "b<<1:  " << b2 << "\n"; // 10000100
    b <<= 1;
    std::cout << "b<<=1: " << b << "\n"; // 10000100

    // --- right shift ---
    std::byte b3 = b >> 1;
    std::cout << "b>>1:  " << b3 << "\n"; // 01000010
    b >>= 1;
    std::cout << "b>>=1: " << b << "\n"; // 01000010

    // --- bitwise OR ---
    std::byte b4 = b | std::byte{ 0b11110000 };
    std::cout << "b|F0:  " << b4 << "\n"; // 11110010
    b |= std::byte{ 0b11110000 };
    std::cout << "b|=F0: " << b << "\n"; // 11110010

    // --- bitwise AND ---
    b4 = b & std::byte{ 0b11110000 };
    std::cout << "b&F0:  " << b4 << "\n"; // 11110000
    b &= std::byte{ 0b11110000 };
    std::cout << "b&=F0: " << b << "\n"; // 11110000

    // --- bitwise XOR ---
    b4 = b ^ std::byte{ 0b11111111 };
    std::cout << "b^FF:  " << b4 << "\n"; // 00001111
    b ^= std::byte{ 0b11111111 };
    std::cout << "b^=FF: " << b << "\n"; // 00001111

    // --- bitwise NOT ---
    b4 = ~b;
    std::cout << "~b:    " << b4 << "\n"; // 11110000

    // --- default init (zero-initialized) ---
    std::byte b6{};
    std::cout << "b6{}:  " << b6 << "\n"; // 00000000

    // --- sizeof ---
    std::cout << "sizeof(std::byte): " << sizeof(std::byte) << "\n";

    // --- std::to_integer ---
    std::byte b7{ 0xFF };
    std::cout << "to_integer<unsigned int>: " << std::to_integer<unsigned int>(b7) << "\n"; // 255
    std::cout << "to_integer<int>:          " << std::to_integer<int>(b7) << "\n"; // 255
    std::cout << "to_integer<signed char>:  " << static_cast<int>(std::to_integer<signed char>(b7))
              << "\n"; // -1
}

namespace byte_ns {
    // 2.1 初始化 std::byte
    // 由于 std::byte 是强类型的枚举类，我们不能直接用整数赋值（如果没有强制类型转换）需要使用
    // std::byte{}。当写下 std::byte{} 时，是在告诉编译器：“请构造一个 std::byte
    // 类型的对象，并使用空列表 {} 对其进行初始化”。

    // 对于普通类/结构体：MyClass{} 会调用默认构造函数。
    // 对于枚举类（如 std::byte）：它不涉及传统意义上的构造函数，而是进行 零初始化。
    // 由于 {} 里面是空的，std::byte{} 会执行 值初始化，对于枚举类型来说，这等同于 零初始化。因为
    // std::byte 的底层类型是 unsigned char，所以：

    // std::byte{} 生成的值等于 static_cast<std::byte>(0)。
    // 它的整数值为 0，二进制为 0000 0000。
    // 因此，std::byte{} 的含义是：创建一个初始值为 0 的 std::byte 类型的临时纯右值对象。
    void test1() {
        // 1. 默认初始化（值未定义）
        std::byte b1;
        std::byte b2{};
        std::byte b3{ 0x41 };
        std::byte b4{ 65 };
        std::byte b5{ 'A' }; // 字符 'A' 的 ASCII 码是 65，把它看作数值存进去
        std::byte b6 = std::byte{ 0x01 }; // 十六进制
        std::byte b7 = std::byte{ 0b10101010 }; // 二进制
        std::byte b8 = std::byte{ 255 }; // 十进制
        // 错误：不能隐式转换
        // std::byte b9 = 10;
    }
    // 2.2 位运算操作
    // std::byte 不支持 +、-、*、/
    // 等算术运算，位运算操作是它最强大的地方。它让代码明确表达了：“我在操作二进制位”。

    // 位运算
    void bitwise_operations() {
        std::byte a{ 0b00110011 }; // 51
        std::byte b{ 0b11110000 }; // 240
        // 位运算
        std::byte c = a & b; // 按位与: 0b00110000 (48)
        std::cout << "a&b: " << c << "\n";
        std::byte d = a | b; // 按位或: 0b11110011 (243)
        std::cout << "a|b: " << d << "\n";
        std::byte e = a ^ b; // 按位异或: 0b11000011 (195)
        std::cout << "a^b: " << e << "\n";
        std::byte f = ~a; // 按位取反: 0b11001100 (204)
        std::cout << "~a: " << f << "\n";
        // 位移运算
        std::byte g = a << 2; // 左移2位: 0b11001100 (204)
        std::cout << "a << 2: " << g << "\n";
        std::byte h = a >> 2; // 右移2位: 0b00001100 (12)
        std::cout << "a >> 2: " << h << "\n";
    }
    // 复合赋值
    void compound_assignments() {
        std::byte x{ 0b10101010 }; // 170
        // 复合位运算
        x &= std::byte{ 0b11110000 }; // x = 0b10100000 (160)
        x |= std::byte{ 0b00001111 }; // x = 0b10101111 (175)
        x ^= std::byte{ 0b11111111 }; // x = 0b01010000 (80)

        // 位移赋值
        x <<= 1; // 左移一位: 0b10100000 (160)
        x >>= 2; // 右移两位: 0b00101000 (40)
    }
    // 比较
    void comparison_operations() {
        std::byte a{ 100 };
        std::byte b{ 200 };
        std::byte c{ 100 };

        // 比较运算(直接支持)
        if (a == c) {
            std::cout << "a equals c" << std::endl;
        }
        if (a != b) {
            std::cout << "a not equals b" << std::endl;
        }
        // 所有比较操作符都支持
        if (a < b) {
            std::cout << "a is less than b" << std::endl;
        }
        if (b >= a) {
            std::cout << "b is greater than or equal to a" << std::endl;
        }
    }

    // 2.3 把 std::byte 转回整数
    // 将 std::byte 转换回整数是一个明确的操作，因为 std::byte
    // 的设计初衷就是为了防止隐式的算术转换，所以你必须显式地进行转换。C++17 中可以使用
    // std::to_integer函数，这是标准库提供的唯一推荐且安全的方式。它是一个函数模板，定义在头文件
    // <cstdint> 中（通常包含 <cstddef> 也会间接包含，但建议主动引入）。
    void to_integer_demo() {
        std::byte b{ 0xAB }; // 十六进制 171

        // 转换为各种整数类型
        auto as_int = std::to_integer<int>(b);
        auto as_uint = std::to_integer<unsigned int>(b);
        auto as_char = std::to_integer<char>(b);
        std::cout << "as int: " << as_int << std::endl; // 171
        std::cout << "as unsigned: " << as_uint << std::endl; // 171
        // 注意：char 可能是有符号的
        std::cout << "as char (numeric): " << (int)as_char << std::endl; // 需要强制转换

        static_assert(std::is_same_v<decltype(as_int), int>, "int");
        static_assert(std::is_same_v<decltype(as_uint), unsigned int>, "unsigned int");
        static_assert(std::is_same_v<decltype(as_char), char>, "char");
        // std::to_integer 的行为不仅仅是转换，它在概念上类似于 static_cast，它会对 std::byte
        // 的底层数值进行整数转换。
        // 无符号目标类型（如 uint8_t, uint32_t）：
        // 结果就是该字节代表的数值。例如 0xFF 转 uint8_t 是 255，转 uint32_t 也是 255。
        // 有符号目标类型（如 int8_t, int）：
        // 这里需要小心。如果 std::byte 的最高位是 1（例如 0xFF，即二进制 1111 1111）：
        // 转换为 int8_t（有符号 char）：在大多数采用补码的机器上，这会被解释为 -1。
        // 转换为 int（通常是 32 位）：根据整数转换规则，uint8_t 的值（255）会被提升为 32 位的整数
        // 255。
    }

    void print_byte(std::byte b) {
        std::cout << "Raw: " << std::bitset<8>(std::to_integer<int>(b))
                  << "\tInt: " << std::to_integer<int>(b) << "\n";
    }
    void test2() {
        std::vector<std::byte> packet;

        // ------------------------------------------------------
        // 1. 构造第一个字节：位拼接与掩码操作
        // ------------------------------------------------------
        // 假设：版本 = 5 (二进制 101), 标志 = 3 (二进制 011)

        std::byte version_part = std::byte{ 5 }; // 0000 0101
        std::byte flags_part = std::byte{ 3 }; // 0000 0011

        // 将版本移位到高 3 位 (左移 5 位)
        std::byte header_byte = version_part << 5;

        // 将标志放入中间 3 位 (左移 2 位)
        header_byte |= (flags_part << 2);
        // 添加到包中
        packet.push_back(header_byte);

        std::cout << "1. 构造 Header Byte:" << std::endl;
        print_byte(packet[0]);
        std::cout << "\n   预期: Ver(101) << 5 + Flags(011) << 2 = 10101100\n" << std::endl;

        // ------------------------------------------------------
        // 2. 构造负载长度 (普通转换)
        // ------------------------------------------------------
        uint8_t length = 64;
        // 使用 std::to_byte 将整数转为字节
        packet.push_back(std::byte{ length });
        std::cout << "2. 添加负载长度:" << std::endl;
        print_byte(packet[1]);
        std::cout << "\n" << std::endl;

        // ------------------------------------------------------
        // 3. 构造端口号：多字节拆解 (移位与掩码)
        // ------------------------------------------------------
        uint16_t port = 8080; // 二进制: 0001 1111 1000 0000
        // 提取高 8 位 (右移 8 位)
        std::byte port_high = std::byte{ static_cast<unsigned char>((port >> 8) & 0xFF) };
        // 提取低 8 位 (使用掩码 0xFF)
        std::byte port_low = std::byte{ static_cast<unsigned char>(port & 0xFF) };
        packet.push_back(port_high);
        packet.push_back(port_low);

        std::cout << "3. 添加端口号 (Big Endian):" << std::endl;
        std::cout << "   Port: " << port << std::endl;
        print_byte(port_high);
        std::cout << " (高字节)" << std::endl;
        print_byte(port_low);
        std::cout << " (低字节)\n" << std::endl;

        // ------------------------------------------------------
        // 4. 解析数据包：读取与验证
        // ------------------------------------------------------
        std::cout << "=== 接收端解析数据包 ===" << std::endl;

        // A. 读取并解析 Header (反向操作)
        std::byte recv_header = packet[0];
        // 提取版本：右移 5 位，然后掩码 0x07 (111)
        int parsed_ver = std::to_integer<int>((recv_header >> 5) & std::byte{ 0x07 });
        // 提取标志：右移 2 位，然后掩码 0x07 (111)
        int parsed_flags = std::to_integer<int>((recv_header >> 2) & std::byte{ 0x07 });
        std::cout << "解析版本: " << parsed_ver << std::endl;
        std::cout << "解析标志: " << parsed_flags << std::endl;

        // B. 重组端口号
        // 先将 byte 转回整数，再通过移位组合
        uint16_t reconstructed_port =
            (std::to_integer<uint16_t>(packet[2]) << 8) | std::to_integer<uint16_t>(packet[3]);
        std::cout << "解析端口: " << reconstructed_port << std::endl;
    }
} // namespace byte_ns

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "std::byte -- C++17 Feature Demo\n";
    std::cout << "================================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_byte();

    std::cout << "\nDone!\n";
    byte_ns::test1();
    byte_ns::bitwise_operations();
    byte_ns::compound_assignments();
    byte_ns::comparison_operations();
    byte_ns::to_integer_demo();
    byte_ns::test2();
    return 0;
}
