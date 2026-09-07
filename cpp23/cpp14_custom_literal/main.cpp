#include <chrono>
#include <codecvt>
#include <complex>
#include <cstdio>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <typeinfo>

// https://subingwen.cn/cpp/user-literal/
// 2. C++14 中的自定义字面量
// 在 C++11 中，只能为以下类型定义自定义字面量：

// 整数：unsigned long long
// 浮点数：long double
// 字符：char
// 字符串：const char*, size_t（字符串长度）

// C++14 扩展了自定义字面量的功能，表现在以下两个方面。

namespace original_literal {
    // 2.1 原始字面量
    // 原始字面量还是用来处理原始字符序列，和字符串字面量一样，只是不需要指定字符串长度。
    std::chrono::seconds operator""_s(const char *str) {
        try {
            int i = std::stoi(str);
            return std::chrono::seconds(i);
        } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            return std::chrono::seconds(0);
        }
    }
    void test() {
        auto duration = 10_s; // 10 seconds
        std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    }
} // namespace original_literal
namespace builtin_chrono_literal {
    // 2.2 标准库内置字面量
    // C++14 标准库主要在以下几个头文件中引入了内置字面量：

    // 1)时间字面量 (在 <chrono> 中)
    // 在 C++ 的 std::chrono
    // 中，这些后缀是时间间隔（duration）的用户定义字面量，用于方便地创建时间单位。
    void test_chrono_literals() {
        using namespace std::chrono_literals;

        auto duration1 = 10s; // 10 seconds
        auto duration2 = 5min; // 5 minutes
        auto duration3 = 2h; // 2 hours
        auto duration4 = 500ms; // 500 milliseconds
        auto duration5 = 100us; // 100 microseconds
        auto duration6 = 50ns; // 50 nanoseconds

        std::cout << "Duration1: " << duration1.count() << " seconds" << std::endl;
        std::cout << "Duration2: " << duration2.count() << " minutes" << std::endl;
        std::cout << "Duration3: " << duration3.count() << " hours" << std::endl;

        std::cout << "Duration4: " << duration4.count() << " milliseconds" << std::endl;
        std::cout << "Duration5: " << duration5.count() << " microseconds" << std::endl;
        std::cout << "Duration6: " << duration6.count() << " nanoseconds" << std::endl;

        auto total = duration1 + duration2 + duration3;
        std::cout << "Total duration in seconds: " << std::chrono::duration_cast<std::chrono::seconds>(total).count() << " seconds" << std::endl;
        std::cout << "Total duration in hours: " << std::chrono::duration_cast<std::chrono::hours>(total).count() << " hours" << std::endl;

        std::this_thread::sleep_for(2s);
        std::cout << "after Slept for 2 seconds" << std::endl;
    }
} // namespace builtin_chrono_literal

namespace builtin_string_literal {
    // 字符串字面量 (在 <string> 中)
    // 使用内置字面量s可以直接将一个char*字符串转换成std::string类型的对象。
    std::string to_utf8(std::u16string_view text) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
        return conv.to_bytes(text.data(), text.data() + text.size());
    }

    std::string to_utf8(std::u32string_view text) {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.to_bytes(text.data(), text.data() + text.size());
    }
    void test_string_literal() {
        using namespace std::string_literals;

        auto s1 = "Hello, World!"s; // std::string
        std::cout << typeid(s1).name() << "\t: " << s1 << std::endl;

        auto s2 = u"world"s; // std::u16string
        std::cout << typeid(s2).name() << "\t: " << to_utf8(s2) << std::endl;

        auto s3 = U"world"s; // std::u32string
        std::cout << typeid(s3).name() << "\t: " << to_utf8(s3) << std::endl;

        auto s4 = L"world"s; // std::wstring
        std::wcout << typeid(s4).name() << "\t: " << s4 << std::endl;

        auto s5 = "hello"s + "cpp"s; // 编译器完成
        std::cout << typeid(s5).name() << "\t: " << s5 << std::endl;

        auto raw = R"(Line1\nLine2)"s; // 原始字符串 + std::string
        std::cout << typeid(raw).name() << "\t: " << raw << std::endl;
    }
} // namespace builtin_string_literal

namespace builtin_complex_literal {
    //     复数字面量 (在 <complex> 中)
    // 在 C++ 中，std::complex 定义了三个方便的虚数字面量后缀：：i、if、il
    // 后缀	类型	对应的 std::complex 类型
    // i	double	std::complex<double>
    // if	float	std::complex<float>
    // il	long double	std::complex<long double>

    void test_complex_literals() {
        using namespace std::complex_literals;
        auto comp1 = 1.0 + 2i;
        auto comp2 = 4i;
        auto comp3 = 3.0F + 4.1if;
        auto comp4 = 1.0L + 2.0il; // std::complex<long double>(1.0L, 2.0L)

        std::cout << typeid(comp1).name() << "\t: " << comp1 << std::endl;
        std::cout << typeid(comp2).name() << "\t: " << comp2 << std::endl;
        std::cout << typeid(comp3).name() << "\t: " << comp3 << std::endl;
        std::cout << typeid(comp4).name() << "\t: " << comp4 << std::endl;
    }
} // namespace builtin_complex_literal

namespace using_builtin_literal {
    // NOLINTBEGIN(bugprone-unused-local-non-trivial-variable)
    void test() {
        // 方式1：导入特定字面量
        using std::chrono_literals::operator""s;
        auto a = 5s; // 5秒
        using std::string_literals::operator""s; // 注意冲突！
        auto b = "hi"s; // "hi"字符串

        // 方式2：使用限定名
        auto s1 = std::chrono_literals::operator""s(5); // 5秒
        auto s2 = std::string_literals::operator""s("hi", 2); // "hi"字符串

        // 方式3：最佳实践（推荐）
        using namespace std::chrono_literals; // 只导入时间
        using namespace std::string_literals; // 只导入字符串
        auto s3 = 10s; // 10秒
        auto s4 = "hello"s; // "hello"字符串
    }
    // NOLINTEND(bugprone-unused-local-non-trivial-variable)
} // namespace using_builtin_literal

int main() {
    original_literal::test();
    builtin_chrono_literal::test_chrono_literals();
    builtin_string_literal::test_string_literal();
    builtin_complex_literal::test_complex_literals();
    using_builtin_literal::test();
    return 0;
}

// 字面量类型	头文件	    命名空间	                主要后缀	返回类型
// 时间字面量	<chrono>	std::chrono_literals	h, min, s, ms, us, ns
// 对应的 duration 类型 字符串字面量	<string>	std::string_literals
// s	std::basic_string 相应类型 复数字面量	<complex>
// std::complex_literals	i, if, il	std::complex 相应类型
