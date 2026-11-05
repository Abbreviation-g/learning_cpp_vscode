#include <array>
#include <ios>
#include <iostream>
#include <limits>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <vector>
// https://subingwen.cn/cpp/fold-expressions/
// 折叠表达式
namespace ns1 {
    // 一个典型的 C++ 可变参数模板递归求和 实现

    // 1. 终止递归的基准函数
    int sum() {
        return 0;
    }
    // 2. 递归展开参数包的函数模板
    template<typename T, typename... Args>
    int sum(T first, Args... rest) {
        return first + sum(rest...);
    }
    //     详细执行过程（以 sum(1, 2, 3) 为例）:

    // 第一次递归展开：此时计算：1 + sum(2, 3)
    // 第二次递归展开：此时计算：1 + (2 + sum(3))
    // 第三次递归展开：此时计算：1 + (2 + (3 + sum()))
    // 终止递归，最终结果：1 + (2 + (3 + 0)) = 6

    //     上面是 C++11/14 中处理参数包的常见方法：

    // 递归分解：每次取出第一个参数（first），剩下的放入参数包（rest...）
    // 递归调用：sum(rest...) 将参数包展开传递给下一次调用
    // 终止条件：当参数包为空时，调用无参数的 sum() 终止递归
    // 这种写法虽然能工作，但优点不明显：

    // 代码冗长：必须显式定义一个终止条件的重载函数。
    // 编译慢：递归实例化会增加编译器的负担。
    // 易读性差：逻辑分散在两个函数中。

    // 我们可以使用现代 C++ 的改进写法 – 折叠表达式来解决上述问题：
    template<typename... Args>
    auto sum2(Args... args) {
        // 或者 return (... + args);  // 一元左折叠
        return (args + ...); // 一元右折叠 // sum(1, 2, 3) -> 展开为 1 + (2 + 3)
    }
} // namespace ns1

// 1.2 折叠表达式的语法
// 折叠表达式的核心语法包含四个部分：

// ( 参数包 op ... )           // 一元右折叠
// ( ... op 参数包 )           // 一元左折叠
// ( 参数包 op ... op 初始值 )  // 二元右折叠
// ( 初始值 op ... op 参数包 )  // 二元左折叠
// 其中 op 是运算符，几乎所有的二元运算符都支持折叠：

// 算术运算符：+, -, *, /, %
// 位运算符：&, |, ^, <<, >>
// 逻辑运算符：&&, ||
// 比较运算符：==, !=, <, >, <=, >=
// 成员访问运算符：.*, ->*
// 逗号运算符：,
// 赋值与复合赋值运算符：=、+=、-=、*=、/=、%=、&=、|=、^=、<<=、>>=
namespace ns2 {
    // 1.2.1 一元右折叠// (pack op ...)
    // 展开规则：
    // (args + ...) → arg1 + (arg2 + (... + argN))
    // 等价于：arg1 + (arg2 + (arg3 + ...))
    // 示例： sum(1, 2, 3, 4);
    // 展开：1 + (2 + (3 + 4)) = 10
    // 执行顺序：先计算 3+4=7，再 2+7=9，再 1+9=10
    template<typename... Args>
    auto sum(Args... args) {
        return (args + ...); // 从右向左结合
    }

    // 1.2.2 一元左折叠// (... op pack)
    //     展开规则：
    // (... + args) → ((arg1 + arg2) + ...) + argN
    // 等价于：((arg1 + arg2) + arg3) + ...
    // 示例：
    // sum(1, 2, 3, 4);
    // // 展开：((1 + 2) + 3) + 4 = 10
    // // 执行顺序：先计算 1+2=3，再 3+3=6，再 6+4=10
    template<typename... Args>
    auto sum2(Args... args) {
        return (... + args); // 从左向右结合
    }

    // 1.2.3 二元右折叠// ( pack op ... op init )
    //     展开规则：
    // (args + ... + init) → arg1 + (arg2 + (... + (argN + init)))
    // 等价于：arg1 + (arg2 + (arg3 + ... + init))
    // 示例：
    // sum_with_init(1, 2, 3, 4);
    // // 展开：1 + (2 + (3 + (4 + 0))) = 10
    // // 相当于：一元右折叠 + init
    template<typename... Args>
    auto sum_with_init(Args... args) {
        return (args + ... + 0);
    }

    // 1.2.4 二元左折叠// ( init op ... op pack )
    //     展开规则：
    // (init + ... + args) → (((init + arg1) + arg2) + ...) + argN
    // 等价于：(((init + arg1) + arg2) + arg3) + ...
    // 示例：
    // sum_with_init2(1, 2, 3, 4);
    // // 展开：(((0 + 1) + 2) + 3) + 4 = 10
    // // 相当于：init + 一元左折叠
    template<typename... Args>
    auto sum_with_init2(Args... args) {
        return (0 + ... + args);
    }
} // namespace ns2

namespace ns3 {
    //     二元折叠

    // 语法： ( Value op ... op Pack ) 或 ( Pack op ... op Value )

    // 如果参数包 pack 非空：执行左结合折叠展开
    // 如果参数包 pack 为空：整个折叠表达式直接等价于 Value ，运算符完全不参与计算。
    // 示例（处理空包）：
    template<typename... Args>
    auto sum(Args... args) {
        // 即使 args 为空，结果也是 0
        return (0 + ... + args);
    }
    template<typename... Args>
    void print_all(Args... args) {
        // 基于 << 运算符的二元左折叠
        (std::cout << ... << args) << std::endl;
    }

    void test() {
        std::cout << sum() << "\n";
        std::cout << sum(1, 2, 3) << "\n";

        print_all();
        print_all("A", "B");

        // 检查是否所有为真（使用二元折叠处理空包更通用）
        auto check = [](auto... conditions) {
            // 空包默认返回 true
            return (true && ... && conditions);
        };
        std::cout << std::boolalpha << check() << std::endl; // 输出: true
        std::cout << std::noboolalpha;

        // 关于print_all函数的函数体(std::cout << ... << args) 实际上是一个基于 <<（流插入）
        // 运算符的二元左折叠。它可以展开为嵌套的左移调用：((((std::cout << arg1) << arg2) << arg3 )
        // ... )。这个二元折叠，有一个初始值 std::cout。 如果 args 为空，表达式就退化只剩下
        // std::cout。 所以后面接上的 << std::endl 实际上是 std::cout <<
        // std::endl，因此是安全的，会输出一个换行符。
    }
} // namespace ns3

// 2. 详细示例和测试代码
// 2.1 基本算术运算
namespace ns4 {
    // 一元右折叠：求和
    template<typename... Args>
    auto sum_right(Args... args) {
        // 相当于：arg1 + (arg2 + (arg3 + ...))
        return (args + ...);
    }
    // 一元左折叠：求和
    template<typename... Args>
    auto sum_left(Args... args) {
        // 相当于：((arg1 + arg2) + arg3) + ...
        return (... + args);
    }
    // 二元右折叠：带初始值的求和
    template<typename... Args>
    auto sum_right_with_init(Args... args) {
        // 相当于：arg1 + (arg2 + (... + (argN + 0)))
        return (args + ... + 0);
    }
    // 二元左折叠：带初始值的求和
    template<typename... Args>
    auto sum_left_with_init(Args... args) {
        // 相当于: (((arg1+0)+arg2)+arg3)+...+argn
        return (0 + ... + args);
    }
    void test() {
        std::cout << "=== 基本算术运算测试 ===" << std::endl;
        // 测试一元折叠
        std::cout << "sum_right(1, 2, 3, 4, 5) = " << sum_right(1, 2, 3, 4, 5) << std::endl;
        std::cout << "sum_left(1, 2, 3, 4, 5) = " << sum_left(1, 2, 3, 4, 5) << std::endl;
        // 对于加法，左右折叠结果相同（加法满足结合律）
        std::cout << "sum_right(1.1, 2.2, 3.3) = " << sum_right(1.1, 2.2, 3.3) << std::endl;
        // 测试二元折叠（带初始值）
        std::cout << "sum_with_init_right(1, 2, 3) = " << sum_right_with_init(1, 2, 3) << std::endl;
        std::cout << "sum_with_init_left(1, 2, 3) = " << sum_left_with_init(1, 2, 3) << std::endl;

        // 空参数包测试（需要二元折叠）
        std::cout << "sum_with_init_right() = " << sum_right_with_init() << std::endl;
        std::cout << "sum_with_init_left() = " << sum_left_with_init() << std::endl;

        // 注意：一元折叠不能用于空参数包，会导致编译错误
        // auto result = sum_right();  // 编译错误！
        std::cout << std::endl;
    }
} // namespace ns4

namespace ns5 {
    // 1.3 空参数包的处理
    // 当涉及到 空参数包
    // 的处理时，折叠表达式的行为取决于你使用的是哪种形式的折叠（一元折叠还是二元折叠），因为并非所有的运算符都支持在没有操作数的情况下进行计算。以下是关于空参数包处理的详细规则：

    // 核心规则：合法与非法

    // C++ 标准明确规定，如果折叠表达式展开时涉及空参数包，除了以下例外情况，通常会导致格式错误：

    // 合法且必须指定初始值的运算符（以下 3 种）：
    // 逻辑与 (&&)：空包结果为 true。
    // 逻辑或(||)：空包结果为 false。
    // 逗号(,)：空包结果为 void()（即什么都不做）。
    // 非法（不能用于空包）的运算符：
    // 算术运算符（+, -, *, /, % 等）：加法或乘法没有操作数是无意义的。
    // 比较、位运算、指针运算等。

    // 2.2 逻辑运算
    template<typename... Args>
    bool all_true(Args... args) {
        // 相当于：((arg1 && arg2) && arg3) && ... argN
        return (... && args);
    }
    template<typename... Args>
    bool any_true(Args... args) {
        // 相当于 (((arg1 || arg2) && arg3) || ... argN
        return (... || args);
    }

    void test() {
        std::cout << "=== 逻辑运算测试 ===" << std::endl;

        std::cout << "all_true(true, true, true) = " << std::boolalpha << all_true(true, true, true)
                  << std::endl;
        std::cout << "all_true(true, false, true) = " << all_true(true, false, true) << std::endl;

        std::cout << "any_true(false, false, true) = " << any_true(false, false, true) << std::endl;
        std::cout << "any_true(false, false, false) = " << any_true(false, false, false)
                  << std::endl;

        // 空参数包的特殊情况
        std::cout << "all_true() = " << all_true() << std::endl; // 返回 true
        std::cout << "any_true() = " << any_true() << std::endl; // 返回 false

        std::cout << std::endl;
    }
} // namespace ns5

namespace ns6 {
    // 2.3 字符串连接
    template<typename... Args>
    std::string concatenate(Args... args) {
        std::ostringstream oss;
        // 二元左折叠：(((oss << arg1) << arg2) << ...) << argN
        (oss << ... << args);
        return oss.str();
    }
    // 使用逗号分隔参数
    // 对于逗号运算符 (A, B) 的逻辑是：先执行 A，再执行 B，整个表达式的值是 B 的值。
    // 逗号运算符会从左到右依次执行左侧和右侧表达式。每段把一个参数及逗号写入
    // std::cout，最后再输出换行。因此结果类似：
    template<typename... Args>
    void print_with_comma(Args... args) {
        // (std::cout << arg1 << ", ", (std::cout << arg2 << ", ",(std::cout << arg3 << ", ",)))
        //  << std::endl;
        ((std::cout << args << ", "), ...) << std::endl;
        // (..., (std::cout << args << ", ")) << std::endl;

    }
    void test() {
        std::cout << "=== 字符串操作测试 ===" << std::endl;
        std::string result = concatenate("Hello", "World", "CPP");
        std::cout << result << "\n";

        std::cout << R"(print_with_comma(1, 2, 3, \"apple\", \"banana\"): )";
        print_with_comma(1, 2, 3, "apple", "banana");
    }
} // namespace ns6
namespace ns7 {
    // 2.4 比较运算

    // 检查所有参数是否按顺序递增
    template<typename... Args>
    bool is_increasing(Args... args) {
        // std::numeric_limits<T>::lowest()：获取模板参数 T 中所有类型的公共类型的最小可表示值。
        // std::common_type_t<Args...>：它会推断出 Args... 中所有参数都能隐式转换成的那个公共类型。
        // 如果 Args... 是 <int, double>，公共类型是 double。
        // 如果 Args... 是 <short, int, long>，公共类型通常是
        // long（取决于具体平台和实现，通常是能容纳所有数的最宽类型）。 如果 Args... 是 <int,
        // std::string>，这行代码会编译报错，因为没有公共类型。
        auto last = std::numeric_limits<std::common_type_t<Args...>>::lowest();

        bool result = true;
        // 折叠表达式：检查每个参数是否大于前一个
        // 使用逗号运算符和逻辑与组合比较
        ((result = result && (args > last), last = args), ...); // 一元右折叠
        // (true, ... , ((result = result && (args > last)), last = args)); // 二元右折叠

        return result;
    }
    void test() {
        std::cout << "=== 比较运算测试 ===" << std::endl;
        std::cout << std::boolalpha;
        std::cout << "is_increasing(1, 2, 3, 4, 5) = " << is_increasing(1, 2, 3, 4, 5) << std::endl;
        std::cout << "is_increasing(1, 3, 2, 4, 5) = " << is_increasing(1, 3, 2, 4, 5) << std::endl;

        std::array arr{ 1, 3, 4 };
        bool arr_is_increasing = std::apply(
            [](const auto &...values) { return is_increasing(values...); }, arr);
        std::cout << "is_increasing(arr) = " << arr_is_increasing << std::endl;
    }
} // namespace ns7

namespace ns8 {
    // 2.5 复杂应用 - 调用函数

    // 使用折叠表达式调用多个函数
    template<typename... Func>
    void execute_all(Func... funcs) {
        // 一次调用所有函数
        // (func1(), (func2(), (func3(), func4())))
        (funcs(), ...);
    }

    // 将多个值插入容器
    template<typename Container, typename... Args>
    void insert_all(Container &container, Args... args) {
        // 使用逗号运算符
        (container.push_back(args), ...);
    }

    void test() {
        std::cout << "=== 函数调用测试 ===" << std::endl;
        std::cout << "execute_all 测试：" << std::endl;
        execute_all([]() { std::cout << "Function 1" << std::endl; },
            []() { std::cout << "Function 2" << std::endl; },
            []() { std::cout << "Function 3" << std::endl; });

        std::cout << "\ninsert_all 测试：" << std::endl;
        std::vector<int> vec{};
        insert_all(vec, 1, 2, 3, 4, 5, 6);
        auto print_items = [](const auto &items) {
            for (const auto &item : items) {
                std::cout << item << ", ";
            }
            std::cout << std::endl;
        };
        print_items(vec);
    }
} // namespace ns8
int main() {
    ns3::test();

    ns4::test();

    ns5::test();

    ns6::test();

    ns7::test();

    ns8::test();
}