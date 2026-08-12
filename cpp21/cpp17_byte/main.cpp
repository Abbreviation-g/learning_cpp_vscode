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
  std::byte b{66};                     // 66 = 0b01000010
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
  std::byte b4 = b | std::byte{0b11110000};
  std::cout << "b|F0:  " << b4 << "\n"; // 11110010
  b |= std::byte{0b11110000};
  std::cout << "b|=F0: " << b << "\n"; // 11110010

  // --- bitwise AND ---
  b4 = b & std::byte{0b11110000};
  std::cout << "b&F0:  " << b4 << "\n"; // 11110000
  b &= std::byte{0b11110000};
  std::cout << "b&=F0: " << b << "\n"; // 11110000

  // --- bitwise XOR ---
  b4 = b ^ std::byte{0b11111111};
  std::cout << "b^FF:  " << b4 << "\n"; // 00001111
  b ^= std::byte{0b11111111};
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
  std::byte b7{0xFF};
  std::cout << "to_integer<unsigned int>: " << std::to_integer<unsigned int>(b7)
            << "\n"; // 255
  std::cout << "to_integer<int>:          " << std::to_integer<int>(b7)
            << "\n"; // 255
  std::cout << "to_integer<signed char>:  "
            << static_cast<int>(std::to_integer<signed char>(b7)) << "\n"; // -1
}

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  std::cout << "std::byte -- C++17 Feature Demo\n";
  std::cout << "================================\n";
  std::cout << "__cplusplus = " << __cplusplus << "\n\n";

  test_byte();

  std::cout << "\nDone!\n";
  return 0;
}
