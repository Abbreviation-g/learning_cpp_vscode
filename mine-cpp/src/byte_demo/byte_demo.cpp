#include "byte_demo.h"

#include <bitset>
#include <cstddef>
#include <iostream>

namespace {

std::ostream& operator<<(std::ostream& os, std::byte b)
{
	return os << std::bitset<8>(std::to_integer<int>(b));
}

} // namespace

int test_byte()
{
	// ????????(????????)??????????std::byte?????
	std::byte b{ 66 };
	std::cout << b << "\n"; // 01000010

	std::byte b2 = b << 1;
	std::cout << b2 << "\n"; // 10000100
	b <<= 1;
	std::cout << b << "\n"; // 10000100

	std::byte b3 = b >> 1;
	std::cout << b3 << "\n"; // 01000010
	b >>= 1;
	std::cout << b << "\n"; // 01000010

	std::byte b4 = b | std::byte{ 0b11110000 };
	std::cout << b4 << "\n"; // 11110010
	b |= std::byte{ 0b11110000 };
	std::cout << b << "\n"; // 11110010

	b4 = b & std::byte{ 0b11110000 };
	std::cout << b4 << "\n"; // 11110000
	b &= std::byte{ 0b11110000 };
	std::cout << b << "\n"; // 11110000

	b4 = b ^ std::byte{ 0b11111111 };
	std::cout << b4 << "\n"; // 00001111
	b ^= std::byte{ 0b11111111 };
	std::cout << b << "\n"; // 00001111

	b4 = ~b;
	std::cout << b4 << "\n"; // 11110000

	[[maybe_unused]] std::byte b5;
	// std::cout << b5 << "\n"; // error: ????????std::byte??????????????????

	std::byte b6{};
	std::cout << b6 << "\n"; // 00000000

	std::cout << "sizeof std::byte: " << sizeof(std::byte) << "\n"; // sizeof std::byte: 1

	std::byte b7{ 0xFF };
	std::cout << std::to_integer<unsigned int>(b7) << ","
		<< std::to_integer<int>(b7) << ","
		<< static_cast<int>(std::to_integer<signed char>(b7)) << "\n"; // 255,255,-1

	return 0;
}
