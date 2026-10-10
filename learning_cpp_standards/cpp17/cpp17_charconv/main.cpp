// https://blog.csdn.net/fengbingchun/article/details/136736884
//  C++17中引入了标准头文件<charconv>提供了用于字符序列与数值之间转换的函数。
//   1.std::to_chars:用于将数值转换为相应的字符表示形式并将结果存储在缓冲区中。其返回std::to_chars_result类
//   型的对象，包含两个成员：
//   (1).ptr:指向所写入字符的结束指针之后的第一个字符的指针;
//   (2).ec:是一个错误代码，决定转换是否成功。如果转换成功，错误代码被设置为errc，否则如果发生错误，它将被设置为错误代码。
//   2.std::from_chars:用于将数值的字符表示形式转换为其相应的数值表示形式。其返回std::from_chars_result类型的对象，包含两个成员：
//   (1).ptr:指向所写入字符的结束指针之后的第一个字符的指针;
//   (2).ec:是一个错误代码，决定转换是否成功。如果转换成功，错误代码被设置为errc，否则如果发生错误，它将被设置为错误代码。

//   std::stringstream:重量级
//   std::to_string:可能涉及动态分配
//   std::to_chars:被设计为占用尽可能少的空间，你提供缓冲区，唯一的开销是确保缓冲区足够大。使用std::to_chars的代码不需要任何动态分配。它在格式选项方面也更加灵活，尤其对于浮点值，std::to_string没有格式化选项。
//   std::from_chars:是一个轻量级解析器，不需要进行任何动态分配。
//   如果没有指定精度
//   ，对于浮点数，std::to_chars和std::from_chars还提供双向保证：即保证数值被转换为字符序列之后如果再转换回来还是原来的数值。注意：只有当在同一个实现上读写时这个保证才成立

//   以下为测试代码：

#include <array>
#include <charconv>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

int test_charconv() {
    constexpr int val_int{ 88 };
    constexpr float val_float{ 66.66f };
    constexpr double val_double{ 88.8888 };
    const std::string str1{ "123" }, str2{ "456.789" };

    // std::to_chars: converts an integer or floating-point value to a character
    // sequence
    std::array<char, 10> buffer;
    // 注意:std::errc没有到bool的隐式转换,所以你不能像下面这样检查: if (res.ec) {}
    // or if (!res.ec) {}
    if (auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), val_int);
        ec == std::errc()) {
        std::cout << "str:" << std::string_view(buffer.data(), ptr - buffer.data())
                  << "\n"; // str:88

        *ptr = '\0'; // 保证结尾有一个空字符
        std::cout << "buffer:" << buffer.data() << "\n"; // buffer:88
    } else {
        std::cerr << "fail to call to_char:" << std::make_error_code(ec).message() << "\n";
        return -1;
    }

    buffer.fill(0);
    if (auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), val_float);
        ec == std::errc()) {
        std::cout << "str:" << std::string_view(buffer.data(), ptr - buffer.data())
                  << "\n"; // str:66.66

        *ptr = '\0'; // 保证结尾有一个空字符
        std::cout << "buffer:" << buffer.data() << "\n"; // buffer:66.66
    } else {
        std::cerr << "fail to call to_char:" << std::make_error_code(ec).message() << "\n";
        return -1;
    }

    buffer.fill(0);
    if (auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), val_double);
        ec == std::errc()) {
        std::cout << "str:" << std::string_view(buffer.data(), ptr - buffer.data())
                  << "\n"; // str:88.8888
    } else {
        std::cerr << "fail to call to_char:" << std::make_error_code(ec).message() << "\n";
        return -1;
    }

    // std::from_chars: converts a character sequence to an integer or
    // floating-point value
    int val1{ 0 };
    auto [ptr, ec] = std::from_chars(str1.data(), str1.data() + str1.size(), val1);
    if (ec == std::errc()) // 注意:std::errc没有到bool的隐式转换,所以你不能像下面这样检查:
                           // if (res.ec) {} or if (!res.ec) {}
        std::cout << "val1:" << val1 << "\n"; // val1:123
    else if (ec == std::errc::invalid_argument)
        std::cerr << "Error: this is not a number: " << str1 << "\n";
    else if (ec == std::errc::result_out_of_range)
        std::cerr << "Error: this number is larger than an int: " << str1 << "\n";
    else
        std::cerr << "fail to call from_chars:" << std::make_error_code(ec).message() << "\n";

    float val2{ 0.f };
    auto [ptr2, ec2] = std::from_chars(str2.data(), str2.data() + str2.size(), val2);
    if (ec2 == std::errc())
        std::cout << "val2:" << val2 << "\n"; // val2:456.789
    else if (ec2 == std::errc::invalid_argument)
        std::cerr << "Error: this is not a number: " << str2 << "\n";
    else if (ec2 == std::errc::result_out_of_range)
        std::cerr << "Error: this number is larger than an float: " << str2 << "\n";
    else
        std::cerr << "fail to call from_chars:" << std::make_error_code(ec2).message() << "\n";

    // std::stringstream: int/float/double->std::string
    std::stringstream ss1;
    ss1 << val_int << "," << val_float << "," << val_double;
    std::string str = ss1.str();
    std::cout << "str:" << str << "\n"; // str:88,66.66,88.8888

    std::stringstream ss2;
    ss2 << str1 << "," << str2;
    str = ss2.str();
    std::cout << "str:" << str << "\n"; // str:123,456.789

    // std::to_string: int/float/double->std::string
    str = std::to_string(val_int);
    std::cout << "str:" << str << "\n"; // str:88
    str = std::to_string(val_float);
    std::cout << "str:" << str << "\n"; // str:66.660004
    str = std::to_string(val_double);
    std::cout << "str:" << str << "\n"; // str:88.888800

    // std::stoi, std::stol, std::stoll: std::string->int/long/long long
    auto ret1 = std::stoi(str1);
    std::cout << "ret1:" << ret1 << "\n"; // ret:123

    // std::stoul, std::stoull: std::string->unsigned long/unsigned long long
    auto ret2 = std::stoul(str1);
    std::cout << "ret2:" << ret2 << "\n"; // ret2:123

    // std::stof, std::stod, std::stold: std::string->float/double/long double
    auto ret3 = std::stof(str2);
    std::cout << "ret3:" << ret3 << "\n"; // ret3:456.789

    return 0;
}
// https://subingwen.cn/cpp/charconv/
// 字符转换
namespace charconv_ns {
    //     1. 为什么我们需要新的转换
    // 在 C++17 之前，C++ 程序员在处理字符和数字转换时经常面临两个痛苦的选择：
    // 1.C 语言风格函数 (atoi, strtol, sprintf)：
    // 类型不安全（容易崩溃），不支持 std::string，功能受限，无法区分错误（比如输入 123abc 会被当成
    // 123 处理）。 2.C++ 流 (std::stringstream/std::to_string)： std::stringstream -
    // 极其慢（涉及内存分配、状态设置），代码冗长（写一行转换要三五行代码），不仅影响性能，还容易写错。
    // std::to_string - 可能抛出bad_alloc异常，并且无法控制精度、格式等

    // C++17 引入了 <charconv> 头文件，提供了一组全新的函数（ std::to_chars 和
    // std::from_chars）。这些函数不仅速度极快（被认为是目前通用 C++
    // 库中最快的），而且极其方便（不分配内存，不抛异常）。
    void test() {
        // 1. 使用 stringstream（慢且有异常）
        std::stringstream ss;
        int value = 25;
        ss << value;
        std::string str = ss.str();

        // 2. 使用 to_string（有异常且无法控制精度、格式等）
        std::string str1 = std::to_string(3.14555F);

        std::cout << "str = " << str << ", str1 = " << str1 << std::endl;

        auto [v1, v2] = std::make_pair(1, 2.2F);
    }

    void float_formatting() {
        double pi = 3.141592653589793;
        std::array<char, 50> buffer{};
        // 1. 通用格式（自动选择固定或科学计数法）
        auto [ptr1, ec1] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), pi);
        // 2. 科学计数法
        auto [ptr2, ec2] = std::to_chars(
            buffer.data(), buffer.data() + buffer.size(), pi, std::chars_format::scientific);
        // 3. 固定小数格式
        auto [ptr3, ec3] = std::to_chars(
            buffer.data(), buffer.data() + buffer.size(), pi, std::chars_format::fixed);
        // 4. 指定精度, 指定精度5
        auto [ptr4, ec4] = std::to_chars(
            buffer.data(), buffer.data() + buffer.size(), pi, std::chars_format::general, 5);

        auto str1 = std::string(buffer.data(), ptr1);
        auto str2 = std::string{ buffer.data(), ptr2 };
        auto str3 = std::string{ buffer.data(), ptr3 };
        auto str4 = std::string{ buffer.data(), ptr4 };

        std::cout << "通用格式: " << str1 << std::endl; // 通用格式: 3.141692653589793
        std::cout << "科学计数: " << str2 << std::endl; // 科学计数: 3.141692653589793e+00
        std::cout << "固定格式: " << str3 << std::endl; // 固定格式: 3.141692653589793
        std::cout << "精度5位: " << str4 << std::endl; // 精度5位: 3.1416
    }
    void test2() {
        // 1. 准备一个足够大的缓冲区
        // std::array 是很好的选择，因为它分配在栈上，速度快且不会溢出
        std::array<char, 20> buffer{};
        // 2. 要转换的数字
        int value = 12345;
        // 3. 执行转换
        auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        // 4. 检查是否成功
        if (result.ec == std::errc()) {
            // result.ptr 指向写入后的下一个位置
            // 我们可以用它构造一个 std::string_view 来打印，避免拷贝
            std::string_view sv(buffer.data(), result.ptr - buffer.data());
            auto str = sv.data();
            std::cout << "转换成功: " << str << std::endl;
        } else {
            std::cout << "转换失败：缓冲区太小" << std::endl;
        }

        int arr[5]{};
        int *front = arr;
        int *end = arr + 4;
        int lenx = end - front;
        std::cout << "arr+4 - arr: " << lenx << "\n"; // 4
    }

    void test3() {
        std::string str1 = "12345";
        std::string str2 = "123abc"; // 后面有非数字字符
        std::string str3 = "  123"; // 前面有空格

        int result = -1;

        // --- 案例 1：完美解析 ---
        auto res1 = std::from_chars(str1.data(), str1.data() + str1.size(), result);
        if (res1.ec == std::errc()) {
            std::cout << "解析成功1: " << result << std::endl;
        }

        result = -1;
        // --- 案例 2：包含字母 ---
        auto res2 = std::from_chars(str2.data(), str2.data() + str2.size(), result);
        if (res2.ec == std::errc()) {
            std::cout << "解析成功2: " << result << std::endl; // 输出 123
            // 检查是否完全解析
            if (res2.ptr != str2.data() + str2.size()) {
                std::cout << "  -> 警告：字符串未完全解析！" << std::endl;
            }
        }

        result = -1;
        // --- 案例 3：包含空格 ---
        auto res3 = std::from_chars(str3.data(), str3.data() + str3.size(), result);
        if (res3.ec == std::errc::invalid_argument) {
            std::cout << "解析失败3：遇到非法字符 (空格)" << std::endl;
        }
    }
} // namespace charconv_ns
int main() {
    std::cout << "std::byte -- C++17 Feature Demo\n";
    std::cout << "================================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_charconv();

    std::cout << "\nDone!\n";

    std::cout << "================================\n";
    charconv_ns::test();
    std::cout << "================================\n";
    charconv_ns::float_formatting();
    std::cout << "================================\n";
    charconv_ns::test2();
        std::cout << "================================\n";
    charconv_ns::test3();
    return 0;
}
