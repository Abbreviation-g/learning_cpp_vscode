#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
// https://subingwen.cn/cpp/variant/
// 类型安全的联合体 -- std::variant

namespace ns1 {
    // 2. std::variant 操作
    // 2.1 赋值与访问
    // 我们可以像使用普通变量一样赋值。访问时，主要使用 index()（索引）和 get()（获取）。
    void test() {
        // 定义：可以是 int, double 或 string
        std::variant<int, double, std::string> v;

        // 1. 默认状态：由于 int 是第一个类型，v 初始化为 0
        std::cout << "Index: " << v.index() << ", value: " << std::get<int>(v) << "\n";

        // 2. 赋值一个 double
        v = 3.14;
        std::cout << "Index: " << v.index() << ", value: " << std::get<double>(v) << "\n";

        // 3. 赋值一个 string
        v = "hello"; // 隐式转换：const char* -> std::string
        std::cout << "Index: " << v.index() << ", value: " << std::get<std::string>(v) << "\n";
        // 注意：index() 返回当前激活类型在模板参数列表中的位置（从 0 开始）。
    }

    //     2.1.1 std::get 的危险与 std::get_if 的温柔
    // 如果我们使用 std::get 去取一个当前不存在的类型，程序会直接抛出 std::bad_variant_access
    // 异常。这对调试很有用，但在生产环境可能导致崩溃。更安全的做法是使用
    // std::get_if，它返回一个指针。如果类型不对，它返回 nullptr，而不会抛出异常。
    void test2() {
        std::variant<int, double> v = 42;

        // -- 危险 --
        try {
            double d = std::get<double>(v); // 错误！v 里是 int，不是 double
        } catch (const std::bad_variant_access &e) {
            std::cerr << "error: " << e.what() << "\n";
        }
        // --- 安全示范 ---
        if (auto *ptr = std::get_if<double>(&v)) {
            std::cout << "获取到 double: " << *ptr << std::endl;
        } else {
            std::cout << "当前不是 double，指针为空" << std::endl;
        }
        if (auto *ptr = std::get_if<int>(&v)) {
            std::cout << "获取到 int: " << *ptr << std::endl;
        }
    }
} // namespace ns1
namespace ns2 {
    //     2.1.2 进阶访问 - std::visit
    // 想象一下，如果我们想打印 variant 里的值，必须得写一堆 if-else 或者 switch这太麻烦了！如果
    // variant 定义了 10 种类型，你得写 10 个分支。

    // C++17 提供的std::visit 允许我们传入一个可调用对象（函数、Lambda），它会自动识别 variant
    // 当前存的是什么类型，然后调用对应的函数。
    struct Pointer {
    public:
        void operator()(int i) {
            std::cout << "Int: " << i << std::endl;
        }
        void operator()(double d) {
            std::cout << "Double: " << d << std::endl;
        }
        void operator()(const std::string &s) {
            std::cout << "string: " << s << std::endl;
        }
    };
    void test() {
        std::variant<int, double, std::string> v;

        v = 100;
        std::visit(Pointer{}, v); // 自动调用 operator()(int)

        v = 3.14;
        std::visit(Pointer{}, v); // 自动调用 operator()(double)

        v = "Test";
        std::visit(Pointer{}, v); // 自动调用 operator()(string)

        // --- 更强大的泛型 lambda (C++17) ---
        // 如果不想手写三个重载，可以用泛型 lambda 自动推导
        auto generic_printer = [](auto &&arg) {
            std::cout << "value: " << arg << ", type size: " << sizeof(arg) << "\n";
        };
        v = 42;
        std::visit(generic_printer, v);
    }

    void test2() {
        std::variant<int, double> v1 = 10;
        std::variant<int, double> v2 = 2.0;

        std::visit(
            [](auto &&a, auto &&b) {
                // 结果类型推导
                auto result = a + b;
                std::cout << a << " + " << b << " = " << result << std::endl;
                // 打印结果类型名称 (输出 'd' 代表 double, 'i' 代表 int)
                std::cout << "结果类型: " << typeid(result).name() << "\n";
            },
            v1, v2);
        // 虽然 v1 是 int，但因为它加上了 double 类型的 v2，最终结果被提升为了 double。
    }

    template <typename... Args>
    void print_string(const Args &...args) {
        std::cout << "参数个数: " << sizeof...(args) << "\n";
        int index = 0;
        (std::cout << ... << args) << std::endl;

        ((std::cout << ", " << args), ...);
        std::cout << "\n";
    }

    // 2.2 特殊情况与异常处理
    // 前面说过 variant
    // 不能为空。但如果你确实需要表示没有值的状态怎么办？（比如数据库查询结果为空）。C++17
    // 提供了一个特殊的空类型 std::monostate。 std::holds_alternative 是 <variant>
    // 头文件中的一个非常实用的辅助函数。它的作用是检查 std::variant
    // 对象当前是否持有特定类型的值。函数原型为：
    void test3() {
        // 第一个类型是 monostate，默认不再初始化为 0，而是空
        std::variant<std::monostate, int, std::string> v;
        if (std::holds_alternative<std::monostate>(v)) {
            std::cout << "Variant 目前是空的" << std::endl;
        }
        v = 299;
        if (std::holds_alternative<int>(v)) {
            std::cout << "Variant 现在有一个整数" << std::endl;
        }
    }
    //     std::holds_alternative 实际上是 index() 的一个更方便、更易读的封装版本。

    // 方法	写法	优点	缺点
    // index()	v.index() == 1	性能极高（直接查索引）	可读性差（需要记住 1 代表
    // int），如果类型顺序变了，代码容易挂。 holds_alternative	holds_alternative<int>(v)
    // 可读性极好（直接看代码就知道在查 int 类型），维护方便
    // 编译器需要知道类型索引，可能会增加一点点编译时间（运行时性能通常没区别）。
    // 如果需要遍历所有可能的情况（比如 switch 语句），或者追求极致的编译期已知逻辑，用 index()。
    // 如果只是想在运行前做判断（比如：如果存的是 string 我就去解析，如果是 int
    // 我就去计算），强烈推荐使用 std::holds_alternative，代码更安全、更清晰。
} // namespace ns2

namespace ns3 {
    struct DivideByZeroError {};
    // 我们的结果可能是：一个数字，或者是除以零错误
    using CalcResult = std::variant<double, DivideByZeroError>;
    // 定义操作符类型
    struct OpAdd {};
    struct OpSub {};
    struct OpMul {};
    struct OpDiv {};
    // 所有可能的操作
    using Operator = std::variant<OpAdd, OpSub, OpMul, OpDiv>;
    // 计算逻辑
    CalcResult calculate(double a, Operator op, double b) {
        // 使用 std::visit 自动分发到不同的操作逻辑
        auto calc = [a, b](auto &&op_tag) -> CalcResult {
            // decltype(op_tag)：取得变量 op_tag 的类型。这里它是泛型 lambda 的转发引用参数
            // auto&&，实际类型可能是 OpAdd&、OpSub& 等。
            // std::decay_t<...>：去掉引用和 const / volatile 修饰。
            // 在这个例子里，OpAdd& 会变成OpAdd。
            // using T = ...：给处理后的类型起别名 T。
            using T = std::decay_t<decltype(op_tag)>;
            if constexpr (std::is_same_v<T, OpAdd>) {
                return a + b;
            }
            if constexpr (std::is_same_v<T, OpSub>) {
                return a - b;
            }
            if constexpr (std::is_same_v<T, OpMul>) {
                return a * b;
            }
            if constexpr (std::is_same_v<T, OpDiv>) {
                if (b == 0.0) {
                    return DivideByZeroError{};
                }
                return a / b;
            }
        };
        return std::visit(calc, op);
    }

    void test() {
        double x = 10.0;
        double y = 2.0;

        // 尝试除法
        Operator op = OpDiv{};
        auto result = calculate(x, op, y);

        // 使用 std::visit 处理结果（打印结果 或 错误信息）
        auto print_result = [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, double>) {
                std::cout << "计算结果是: " << arg << std::endl;
            } else if constexpr (std::is_same_v<T, DivideByZeroError>) {
                std::cout << "错误：除数不能为零！" << std::endl;
            }
        };
        std::visit(print_result, result);

        // 尝试除以零
        y = 0.0;
        result = calculate(x, op, y);
        std::visit(print_result, result);
    }
} // namespace ns3
int main() {
    ns1::test();
    ns1::test2();
    ns2::print_string("hello", "world", "!");

    ns2::test();
    ns2::test2();

    ns3::test();
}