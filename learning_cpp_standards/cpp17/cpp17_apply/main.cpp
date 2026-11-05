#include <iostream>
#include <tuple>
#include <functional>
// https://subingwen.cn/cpp/apply/
// 终极元组解包器 - std::apply / std::make_from_tuple

// 1. std::apply 概述
// std::apply 是 C++17
// 引入的一个实用工具函数，其主要目的是将元组（或类元组对象）解包为函数调用的参数。这解决了在 C++14
// 及之前版本中，需要手动解包元组参数调用函数的繁琐问题。

// 函数原型与语法：

// #include <tuple>

// namespace std
// {
//     template <class F, class Tuple>
//     constexpr decltype(auto) apply(F&& f, Tuple&& t);
// }
// 模板参数：

// F：可调用对象类型（函数、函数指针、函数对象、lambda 等）
// Tuple：类元组类型（std::tuple、std::pair、std::array 或任何支持 std::get 和 std::tuple_size
// 的类型） 返回值：

// 返回 f 的调用结果
// 使用 decltype(auto) 自动推导返回类型，完美转发返回值
// C++23 之前只有元组版本，C++23 增加了类似 std::invoke 的按序传参版本，这里我们专注 C++17 标准。

// 2. std::apply 应用
// 2.1 普通函数与 std::apply
// 这是最基础的用法，演示如何将元组展开传给函数。
namespace ns1 {
    void add(int a, int b) {
        std::cout << "a + b = " << (a + b) << std::endl;
    }
    void print_info(int id, const std::string &name, double score) {
        std::cout << "ID: " << id << ", Name: " << name << ", Score: " << score << std::endl;
    }
    void test() {
        std::tuple<int, int> t1{ 10, 20 };
        std::apply(add, t1);

        std::tuple<int, std::string, double> t2{ 101, "allice", 99.9 };
        std::apply(print_info, t2);
    }
} // namespace ns1

// 2.2 Lambda 表达式与 std::apply
// Lambda 非常适合配合 std::apply 做一些临时的计算。
namespace ns2 {
    void test() {
        auto t = std::make_tuple(2, 3);

        // 直接写 Lambda 来使用 tuple 中的值
        std::apply(
            [](int x, int y) {
                std::cout << "x * y = " << x * y << std::endl;
            },
            t);

        // Lambda 捕获变量
        int base = 100;
        std::apply(
            [base](int x, int y) {
                std::cout << "base+x+y= " << base + x + y << std::endl;
            },
            t);
    }
} // namespace ns2
// 2.3 成员函数与 std::apply
namespace ns3 {
    class Multiplier {
    public:
        int factor;
        explicit Multiplier(int f) : factor(f) {}
        int muliply(int x) const {
            return x * factor;
        }
        void show(int x, int y) const {
            std::cout << "result: " << (x + y) * factor << "\n";
        }
    };
    void test() {
        Multiplier m{ 10 };
        // --- 正确示范 1：对象是 tuple 的一部分 ---
        // 我们想把 m 和 5 一起传给 multiply
        // tuple 的第一个元素是对象 m (指针或引用)，第二个是参数 5
        auto args1 = std::make_tuple(&m, 5); // 注意这里用指针 m 很方便

        // 调用: (m.*multiply)(5)
        int res = std::apply(&Multiplier::muliply, args1);
        std::cout << "Result: " << res << std::endl; // 50

        // --- 正确示范 2：直接在 apply 构造 tuple ---
        // 我们要调用 show，需要: 对象, 参数1, 参数2
        // 注意：这里如果用 ref(m) 或 &m 比较好，避免拷贝
        std::apply(&Multiplier::show, std::make_tuple(&m, 2, 3));

        // --- 正确示范 3：使用 std::ref 避免拷贝对象 ---
        Multiplier m2(5);
        // 如果成员函数是非 const 的，且对象很大，建议传引用
        std::apply(&Multiplier::show, std::make_tuple(std::ref(m2), 10, 11));

        // 对于成员函数，std::apply 的逻辑本质上是：invoke(f, get<0>(t), get<1>(t),
        // get<2>(t)...)所以，tuple
        // 的第一个元素必须是对象（或对象指针/引用），后续元素才是函数的参数。这与 std::invoke
        // 是完全一脉相承的。
    }
} // namespace ns3

// 2.4 返回值处理与 constexpr
// 2.4.1 返回值推导
// std::apply 会完美转发返回值。我们可以用 auto 接收。
namespace ns4 {
    template <typename T, typename U>
    auto sum(T a, U b) -> decltype(a + b) {
        return a + b;
    }
    void test() {
        auto t = std::make_tuple(1, 2);

        // auto 推导为int
        auto result = std::apply(sum<decltype(std::get<0>(t)), decltype(std::get<1>(t))>, t);

        // 甚至可以接受引用（如果函数返回引用）
        static int x = 0;
        auto get_ref = []() -> int & {
            return x;
        };
        int &ref = std::apply(get_ref, std::tuple<>{}); // 空元组
        ref = 100;
        std::cout << "x= " << x << "\n"; // x= 100
    }
    // 这段代码主要展示了 std::apply 的两个特性：

    // 它可以处理空 tuple（即调用无参函数）。

    // 它可以完美转发返回值类型（保留引用）。

    // get_ref：这是一个 Lambda 表达式，它不接受任何参数，但返回全局变量 x 的引用 int&。

    // std::tuple<>{} ：这是一个空的 tuple。因为它没有元素，std::apply 调用 get_ref
    // 时自然也不传任何参数，相当于调用 get_ref()。

    // int& ref = ...：

    // std::apply 执行 get_ref()，返回 x 的引用。
    // 关键点在于 std::apply 保留了返回引用这个性质（并没有把它变成 int 副本）。
    // ref 成为了 x 的引用。
    // ref = 100;：由于 ref 是引用，这行代码实际上修改了全局变量 x 的值。

    // std::apply 是通用的，无论函数是有参（搭配非空 tuple）还是无参（搭配空
    // tuple），也无论返回的是值还是引用，它都能正确处理。

} // namespace ns4

// 2.4.2 constexpr 和 编译期计算
//  C++17 标准要求 std::apply 是 constexpr 的。这意味着你可以在编译期就用它！
namespace ns5 {
    template <typename T, typename U>
    constexpr auto add(T a, U b) -> decltype(a + b) {
        return a + b;
    }
    void test() {
        // 编译期常量
        constexpr std::tuple<int, int> t{ 3, 4 };

        // 编译期调用apply
        constexpr int res = std::apply(add<decltype(std::get<0>(t)), decltype(std::get<1>(t))>, t);

        static_assert(res == 7, "Math error"); // 编译期检查
        std::cout << res << std::endl;
    }
} // namespace ns5

// 3. std::make_from_tuple
// 在现代 C++ 元编程中，元组可以被视为一种通用的参数包容器。我们经常面临着这样一种需求：将存储在一个
// tuple 中的多个参数，解开并传递给某个类的构造函数，从而创建一个对象。

// 在 C++17 之前，这虽然可以通过 std::tuple 的配合和一些复杂的模板元编程技巧（如
// std::index_sequence）实现，但代码晦涩难懂。C++17 引入了
// std::make_from_tuple，专门为了解决这个问题，它提供了一种类型安全、性能高效且语法简洁的方式来将元组转换为对象。

// std::make_from_tuple 定义在头文件 <tuple> 中。函数原型如下：

// namespace std
// {
//     template <class T, class Tuple>
//     constexpr T make_from_tuple(Tuple&& t);
// }
namespace ns6 {
    class Widget {
    public:
        // 多个构造函数
        Widget(int, double) {
            std::cout << "Widget(int, double)\n";
        }
        Widget(const char *, int) {
            std::cout << "Widget(const char*, int)\n";
        }
        explicit Widget(bool) {
            std::cout << "Widget(bool)\n";
        }
    };
    void test() {
        // 使用不同的元组调用不同的构造函数
        auto w1 = std::make_from_tuple<Widget>(std::make_tuple(10, 2.3));
        auto w2 = std::make_from_tuple<Widget>(std::make_tuple("test", 42));
        auto w3 = std::make_from_tuple<Widget>(std::make_tuple(true));
    }

    // std::make_from_tuple 是 constexpr
    // 的，这意味着你可以在编译期通过元组构造对象，用于静态断言或模板元计算。
    struct Point {
    public:
        int x, y;
        constexpr Point(int a, int b) : x(a), y(b) {};
    };

    void test2() {
        constexpr auto t = std::make_tuple(3, 4);
        constexpr Point p = std::make_from_tuple<Point>(t);
        static_assert(p.x == 3);
        static_assert(p.y == 4);
    }
} // namespace ns6
int main() {
    ns1::test();

    ns2::test();

    ns3::test();

    ns4::test();

    ns5::test();

    ns6::test();
    ns6::test();
}