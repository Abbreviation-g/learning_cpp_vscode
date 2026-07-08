#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef byte  // Windows SDK byte macro conflicts with std::byte
#endif

#include <iostream>
#include <cstddef>
#include <bitset>

// helper: print std::byte as binary
namespace {
std::ostream& operator<<(std::ostream& os, std::byte b)
{
    return os << std::bitset<8>(std::to_integer<int>(b));
}
} // namespace

void test_byte()
{
    std::cout << "=== C++17 std::byte Demo ===\n\n";

    // std::byte is enum class byte : unsigned char {};
    // Only brace-init allowed, only bitwise ops supported

    // --- init & left shift ---
    std::byte b{ 66 };               // 66 = 0b01000010
    std::cout << "b:     " << b << "\n";      // 01000010

    std::byte b2 = b << 1;
    std::cout << "b<<1:  " << b2 << "\n";     // 10000100
    b <<= 1;
    std::cout << "b<<=1: " << b << "\n";      // 10000100

    // --- right shift ---
    std::byte b3 = b >> 1;
    std::cout << "b>>1:  " << b3 << "\n";     // 01000010
    b >>= 1;
    std::cout << "b>>=1: " << b << "\n";      // 01000010

    // --- bitwise OR ---
    std::byte b4 = b | std::byte{ 0b11110000 };
    std::cout << "b|F0:  " << b4 << "\n";     // 11110010
    b |= std::byte{ 0b11110000 };
    std::cout << "b|=F0: " << b << "\n";      // 11110010

    // --- bitwise AND ---
    b4 = b & std::byte{ 0b11110000 };
    std::cout << "b&F0:  " << b4 << "\n";     // 11110000
    b &= std::byte{ 0b11110000 };
    std::cout << "b&=F0: " << b << "\n";      // 11110000

    // --- bitwise XOR ---
    b4 = b ^ std::byte{ 0b11111111 };
    std::cout << "b^FF:  " << b4 << "\n";     // 00001111
    b ^= std::byte{ 0b11111111 };
    std::cout << "b^=FF: " << b << "\n";      // 00001111

    // --- bitwise NOT ---
    b4 = ~b;
    std::cout << "~b:    " << b4 << "\n";     // 11110000

    // --- default init (zero-initialized) ---
    std::byte b6{};
    std::cout << "b6{}:  " << b6 << "\n";     // 00000000

    // --- sizeof ---
    std::cout << "sizeof(std::byte): " << sizeof(std::byte) << "\n";

    // --- std::to_integer ---
    std::byte b7{ 0xFF };
    std::cout << "to_integer<unsigned int>: "
              << std::to_integer<unsigned int>(b7) << "\n";   // 255
    std::cout << "to_integer<int>:          "
              << std::to_integer<int>(b7) << "\n";            // 255
    std::cout << "to_integer<signed char>:  "
              << static_cast<int>(std::to_integer<signed char>(b7)) << "\n"; // -1
}

int main()
{
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
