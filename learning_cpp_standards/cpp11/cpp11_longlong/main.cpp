// https://subingwen.cn/cpp/longlong/
// long long 整形

#include <cfloat>
#include <climits>
#include <iostream>
#include <limits>

namespace ll_ns1 {
    using namespace std;
    void test() {
        cout << "char min: " << static_cast<int>(CHAR_MIN)
             << ", max: " << static_cast<int>(CHAR_MAX) << endl;
        cout << "signed char min: " << static_cast<int>(SCHAR_MIN)
             << ", max: " << static_cast<int>(SCHAR_MAX) << endl;
        cout << "unsigned char max: " << static_cast<int>(UCHAR_MAX) << endl;
        // char min: -128, max: 127
        // signed char min: -128, max: 127
        // unsigned char max: 255

        cout << "short min: " << SHRT_MIN << ", max: " << SHRT_MAX << endl;
        cout << "unsigned short max: " << USHRT_MAX << endl;
        // short min: -32768, max: 32767
        // unsigned short max: 65535

        cout << "int min: " << INT_MIN << ", max: " << INT_MAX << endl;
        cout << "unsigned int max: " << UINT_MAX << endl;
        // int min: -2147483648, max: 2147483647
        // unsigned int max: 4294967295

        cout << "long min: " << LONG_MIN << ", max: " << LONG_MAX << endl;
        cout << "unsigned long max: " << ULONG_MAX << endl;
        // long min: -2147483648, max: 2147483647
        // unsigned long max: 4294967295

        long long max = LLONG_MAX;
        long long min = LLONG_MIN;
        unsigned long long ull_max = ULLONG_MAX;
        cout << "Max Long Long value: " << max << endl
             << "Min Long Long value: " << min << endl
             << "Max unsigned Long Long value: " << ull_max << endl;
        // Max Long Long value: 9223372036854775807
        // Min Long Long value: -9223372036854775808
        // Max unsigned Long Long value: 18446744073709551615

        // 类型关键字	典型大小 (字节)	说明
        // bool	1	布尔型，值为 true 或 false
        // char	1	字符型，也可存储小整数 (-128~127 或 0~255)
        // short / short int	2	短整型
        // int	4	整型（最常见）
        // long / long int	4 或 8	长整型（Windows 通常为 4，Linux 64位通常为 8）
        // long long / long long int	8	长长整型 (C++11 引入)
        cout << "bool: " << sizeof(bool) << " bytes" << endl; // 1
        cout << "char: " << sizeof(char) << " bytes" << endl; // 1
        cout << "short: " << sizeof(short) << " bytes" << endl; // 2
        cout << "int: " << sizeof(int) << " bytes" << endl; // 4
        cout << "long: " << sizeof(long) << " bytes" << endl; // 4
        cout << "long long: " << sizeof(long long) << " bytes" << endl; // 8

        //         事实上在C++11中还有一些类型与以上两种类型是等价的:

        // 对于有符号类型的 long long和以下三种类型等价
        // long long int
        // signed long long
        // signed long long int
        // 对于无符号类型的unsigned long long 和unsigned long long int是等价的
    }
    // 可以看到 long long
    // 类型能够存储的最大/最小值还是非常大/小的，但是这个值根据平台不同会有所变化，原因是因为C++11标准规定该类型至少占8字节，它占的字节数越多，对应能够存储的数值也就越大。
    // 在C++11中一共只定义了以下5种标准的有符号整型：
    //  signed char
    //  short int
    //  int
    //  long int
    //  long long int
    //  标准同时规定，每一种有符号整型都有一种对应的无符号整数版本，且有符号整型与其对应的无符号整型具有相同的存储空间大小。比如与
    //  signed int对应的无符号版本的整型是 unsigned int。

    // 当我们在C++中处理数据的时候，如果参与运算的数据或者传递的参数类型不匹配，整型间会发生隐式的转换，这种过程通常被称为整型的提升。比如如下表达式∶

    // (int)num1 + (long long)num2
    // 关于这种整形提升的隐式转换遵循如下原则:

    // 长度越大的整型等级越高，比如 long long int 的等级会高于int。
    // 长度相同的情况下，标准整型的等级高于扩展类型，比如 long long int 和 int64 如果都是64
    // 位长度，则long long int类型的等级更高。 相同大小的有符号类型和无符号类型的等级相同，long long
    // int 和unsigned longlong int的等级就相同。
    // 转换过程中，低等级整型需要转换为高等级整型，有符号的需要转换为无符号整形。

    void test2() {
        // min() 关注“最小正值”
        // lowest() 关注“最小可能值”
        cout << "float min: " << numeric_limits<float>::lowest()
             << ", max: " << numeric_limits<float>::max()
             << ", min positive: " << numeric_limits<float>::min() << '\n';

        cout << "double min: " << numeric_limits<double>::lowest()
             << ", max: " << numeric_limits<double>::max()
             << ", min positive: " << numeric_limits<double>::min() << '\n';

        cout << "long double min: " << numeric_limits<long double>::lowest()
             << ", max: " << numeric_limits<long double>::max()
             << ", min positive: " << numeric_limits<long double>::min() << '\n';
        // float min: -3.40282e+38, max: 3.40282e+38, min positive: 1.17549e-38
        // double min: -1.79769e+308, max: 1.79769e+308, min positive: 2.22507e-308
        // long double min: -1.18973e+4932, max: 1.18973e+4932, min positive: 3.3621e-4932

        cout << "float size: " << sizeof(float) << " bytes\n"; // 4
        cout << "double size: " << sizeof(double) << " bytes\n"; // 8
        cout << "long double size: " << sizeof(long double) << " bytes\n"; // 16
    }
} // namespace ll_ns1

int main() {
    std::cout << "--------" << "" << "--------" << "\n";
    ll_ns1::test();
    std::cout << "--------" << "" << "--------" << "\n";
    ll_ns1::test2();
}