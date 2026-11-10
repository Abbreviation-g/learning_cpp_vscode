
#include <cstddef>
#include <type_traits>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <utility>

// ============================================================
// C++17 Structured Bindings
// ============================================================

// ---- 1. Binding an array ----
void test_bind_array() {
    std::cout << "=== 1. Binding an array ===\n\n";

    int arr1[3] = { 1, 2, 3 };

    auto [x, y, z] = arr1; // copy
    std::cout << "x:" << x << ", y:" << y << ", z:" << z << "\n";
    x = 4;
    std::cout << "arr1[0]:" << arr1[0] << " (unchanged, copy)\n";

    auto &[x2, y2, z2] = arr1; // reference
    std::cout << "x2:" << x2 << ", y2:" << y2 << ", z2:" << z2 << "\n";
    x2 = 5;
    std::cout << "arr1[0]:" << arr1[0] << " (changed, ref)\n";

    const auto &[x3, y3, z3]{ arr1 };
    const auto &[x4, y4, z4](arr1);
    std::cout << "x3:" << x3 << ", x4:" << x4 << "\n";
}

// ---- 2. Binding a tuple-like type ----
void test_bind_tuple() {
    std::cout << "\n=== 2. Binding a tuple-like type ===\n\n";

    std::tuple<std::string, int, float> foo{ "csdn", 8, 6.6f };

    const auto &[x3, y3, z3] = foo;
    std::cout << "x3:" << x3 << ", y3:" << y3 << ", z3:" << z3 << "\n";

    auto &[x4, y4, z4] = foo;
    x4 = "github";
    y4 = 6;
    z4 = 8.8f;
    std::cout << "foo: " << std::get<0>(foo) << "," << std::get<1>(foo) << "," << std::get<2>(foo)
              << "\n";

    int a = 1, b = 2;
    const auto &[x6, y6] = std::tie(a, b);
    x6 = 6;
    auto [x7, y7] = std::tie(a, b);
    std::cout << "x7:" << x7 << ", a:" << a << "\n";
    x7 = 8;
    std::cout << "x6:" << x6 << ", a:" << a << "\n";
    if (&x6 != &x7)
        std::cout << "Error: &x6 != &x7\n";
}

// ---- 3. Binding to data members (struct) ----
namespace {

    struct Info {
        mutable std::string name;
        volatile int number;
    };

    inline Info func() {
        return Info{ "csdn", 666 };
    }

    struct Data {
        int b{ 1 }, d{ 2 }, p{ 3 }, q{ 4 };
    };

} // namespace

void test_bind_struct() {
    std::cout << "\n=== 3. Binding to data members ===\n\n";

    const auto [x5, y5] = func();
    std::cout << "x5:" << x5 << ", y5:" << y5 << "\n";
    x5 = "github";

    const auto [b1, d1, p1, q1] = Data{};
    std::cout << "b1:" << b1 << ", d1:" << d1 << ", p1:" << p1 << ", q1:" << q1 << "\n";

    const auto [b2, d2, p2, q2] = Data{ 4, 3, 2, 1 };
    std::cout << "b2:" << b2 << ", d2:" << d2 << ", p2:" << p2 << ", q2:" << q2 << "\n";

    Data s;
    auto &[b3, d3, p3, q3] = s;
    std::cout << "b3:" << b3 << ", d3:" << d3 << ", p3:" << p3 << ", q3:" << q3 << "\n";
    b3 = 4;
    d3 = 3;
    p3 = 2;
    q3 = 1;
    std::cout << "s.b:" << s.b << ", s.d:" << s.d << ", s.p:" << s.p << ", s.q:" << s.q << "\n";
}

// ---- 4. map iteration with structured bindings ----
void test_bind_map() {
    std::cout << "\n=== 4. map iteration with structured bindings ===\n\n";

    const std::map<std::string, std::string> addrs{ { "csdn",
                                                        "https://blog.csdn.net/fengbingchun/" },
        { "github", "https://github.com/fengbingchun" } };

    for (const auto &[key, val] : addrs) {
        std::cout << "key: " << key << ", addr: " << val << "\n";
    }
}

// ---- 5. move semantics with structured bindings ----
void test_bind_move() {
    std::cout << "\n=== 5. move semantics with structured bindings ===\n\n";

    Info info{ "github", 888 };
    auto &&[u, v] = std::move(info);
    std::cout << "info.name: " << info.name << " (still holds value)\n";

    Info info2{ "csdn", 666 };
    auto [u2, v2] = std::move(info2);
    std::cout << "info2.name: '" << info2.name << "' (moved-from, empty)\n";
}

// ============================================================
// Main
// ============================================================

// 结构化绑定
// https://subingwen.cn/cpp/structured-bindings/
// 1. 结构化绑定基本语法
// C++17
// 引入了一个非常有趣且实用的特性，叫做结构化绑定。简单来说，它让我们可以像给变量起名字一样，一次性把一个复杂对象（如数组、结构体、元组）里的多个值拆分出来，分别赋值给不同的变量。

// 在 C++17
// 之前，如果我们想从一个包含两个值的对象里取值，代码可能会有点啰嗦。有了结构化绑定，代码瞬间变得像
// Python 或 JavaScript 一样优雅。

// 结构化绑定就像是一个特殊的变量声明。它的基本长这样：

// auto [变量1, 变量2, 变量3, ...] = 要绑定的对象;
// auto：让编译器自动推导类型（就像你平时用 auto a = 10; 一样）。
// [ ... ]：方括号里是你想要的新变量名。
// =：赋值符号。
// 注意： 方括号里的变量名数量，必须和你要绑定的对象里的元素数量完全一致，否则会报错。

// 假设现在有一个函数，它返回了一个包含学生姓名和分数的结构体：

// C++17 之前（传统写法）：

// Student result = getStudent();
// // 得这样写，很啰嗦
// std::string name = result.name;
// int score = result.score;
// C++17 结构化绑定（新写法）：

// // 一行搞定！直接“解包”
// auto [name, score] = getStudent();
// 使用结构化绑定代码更少，逻辑更清晰，阅读的人一眼就能看出 name 和 score 分别代表什么。
namespace ns1 {
    void test1() {
        // 定义一个包含 3 个整数的数组
        int my_array[]{ 10, 20, 30 };
        // 【核心代码】
        // 我们创建名为 a, b, c 的三个变量，分别对应数组里的 0, 1, 2 号元素
        auto [a, b, c] = my_array;
        std::cout << "a: " << a << std::endl;
        std::cout << "b: " << b << std::endl;
        std::cout << "c: " << c << std::endl;
    }
    // 定义一个简单的结构体
    struct User {
        std::string name;
        int age;
        double height;
    };
    void test2() {
        // 创建一个结构体实例
        User u{ "ming", 18, 177.7 };
        // 【核心代码】
        // 自动识别 u.name 是 string，u.age 是 int，u.height 是 double
        // 并按顺序赋值给 name, age, height 三个新变量
        auto [name, age, height] = u;
        std::cout << "姓名: " << name << std::endl;
        std::cout << "年龄: " << age << std::endl;
        std::cout << "身高: " << height << std::endl;
        //         在使用结构化绑定的时候，关于类成员权限的说明：如果结构体成员是
        //         private（私有的），那么直接绑定会失败。结构化绑定要求成员必须是
        //         public（公有的）或者提供了特殊的接口。这里的特殊接口指的是针对该结构体或类实现的元组式协议。

        // 简单来说，如果我们的结构体或者类成员是 private
        // 的，不能直接使用默认的结构化绑定，但可以通过重载以下全局函数，让编译器知道如何解包对应的对象：

        // std::tuple_size<S>::value（或者 std::tuple_size<S>::value 的特化）： C++
        // 标准库中的一个工具，用于在编译期获取类型 S 中包含的元素个数。

        // 它的核心作用不是查看某个变量有多少个元素，而是查看某个数据类型定义了多少个成员。

        // std::tuple_element<I, S>::type（特化）：类型萃取工具，用于告知编译器第 I 个元素的类型。

        // I：元素的索引（从 0 开始）。
        // S：要查询的类型（如 tuple、pair、array 或支持结构化绑定的类）。
        // get 函数：可以是成员函数 get<I>() 或者全局函数 get<I>(s)，用于返回对应索引的值。
    }
}; // namespace ns1
namespace ns2 {
    class User {
    private:
        std::string name;
        int age;
        double height;

    public:
        // 修改 1: 添加公有构造函数，允许外部初始化
        User(std::string n, int a, double h) : name(n), age(a), height(h) {}
        // 修改 2: 实现 get 接口 (返回对应索引的成员)
        // 注意：这里必须返回引用或 const 引用，才能对原对象读取（如果是引用绑定）
        template<std::size_t I>
        auto &get() & {
            std::cout << "template<std::size_t I>        auto &get() &" << "\n";
            // 函数参数列表后的第二个 & 是引用限定符，表示这个成员函数只能通过左值对象调用。
            // User user{"Alice", 20, 1.7};
            // user.get<0>();              // 可以：user 是左值
            // User{"Alice", 20, 1.7}.get<0>(); // 不可以：临时对象是右值
            // 这种限定常用于避免从临时对象返回引用，否则临时对象销毁后，引用会悬空。它也可以和 &&
            // 重载，分别处理左值和右值对象。
            if constexpr (0 == I) {
                return name;
            } else if constexpr (1 == I) {
                return age;
            } else if constexpr (2 == I) {
                return height;
            }
        }
        // const 版本的 get，用于 const 对象
        template<std::size_t I>
        const auto &get() const & {
            std::cout << "template<std::size_t I>        const auto &get() const &" << "\n";
            if constexpr (0 == I) {
                return name;
            } else if constexpr (1 == I) {
                return age;
            } else if constexpr (2 == I) {
                return height;
            }
        }
        template<std::size_t I>
        auto &&get() && {
            std::cout << "template<std::size_t I>        auto &&get() &&" << "\n";
            if constexpr (0 == I) {
                return std::move(name);
            } else if constexpr (1 == I) {
                return std::move(age);
            } else if constexpr (2 == I) {
                return std::move(height);
            }
        }
    };
} // namespace ns2

// 用户自定义类型的 tuple_size/tuple_element 特化必须位于 std 命名空间。
namespace std {
    // 修改 3: 特化 std::tuple_size，告知编译器 User 有 3 个元素
    template<>
    struct tuple_size<ns2::User> : integral_constant<size_t, 3> {};
    // 修改 4: 特化 std::tuple_element，告知编译器每个位置的类型
    template<size_t I>
    struct tuple_element<I, ns2::User> {
        // tuple_element 描述元素本身的类型，不应包含 get() 返回的引用。
        using type = remove_reference_t<decltype(declval<ns2::User &>().template get<I>())>;
    };
} // namespace std

namespace ns2 {
    // 这种机制实际上是把我们的类伪装成了一个
    // std::tuple。如果使用了这种方式，结构化绑定就不会去关心成员是不是 public 的，它只会调用提供的
    // get 方法来获取数据。std::pair 和 std::tuple
    // 之所以能支持结构化绑定，就是内部实现了这一套接口。
    void test() {
        // 创建实例 (现在的初始化调用的是我们添加的构造函数)
        User u = { "ming", 19, 199.9 };
        // 【核心代码】
        // 编译器发现 User 实现了 tuple-like 协议，
        // 它会调用 u.get<0>(), u.get<1>(), u.get<2>() 来获取值，
        // 从而绕过了成员是 private 的限制。
        auto [name, age, height] = u;
        // template<std::size_t I>        auto &&get() &&
        std::cout << "姓名: " << name << std::endl;
        std::cout << "年龄: " << age << std::endl;
        std::cout << "身高: " << height << std::endl;
        name = "xxxx";
        std::cout << "姓名: " << u.get<0>()
                  << std::endl; // template<std::size_t I>        auto &get() &

        auto &[name2, age2, height2] = u;
        // template<std::size_t I>        auto &get() &
        std::cout << "姓名: " << name2 << std::endl;
        std::cout << "年龄: " << age2 << std::endl;
        std::cout << "身高: " << height2 << std::endl;
        name2 = "xxxx";
        std::cout << "姓名: " << u.get<0>()
                  << std::endl; // template<std::size_t I>        auto &get() &

        const auto &[name3, age3, height3] = u;
        // template<std::size_t I>        const auto &get() const &
        std::cout << "姓名: " << name3 << std::endl;
        std::cout << "年龄: " << age3 << std::endl;
        std::cout << "身高: " << height3 << std::endl;
        // name3 = "xxxx";
        // std::cout << "姓名: " << u.get<0>() << std::endl;
    }

    // 2.3 绑定 std::pair 和 std::map
    // 在 C++
    // 中，std::map（映射容器）存储的是一对一对的值（键值对）。比如存储学号对应姓名。以前我们要遍历
    // map 很麻烦，现在非常爽。关于std::map有一个小细节就是它的每个元素其实是一个 std::pair。
    void test2() {
        // 创建一个 map：存储 ID 和 姓名
        std::map<int, std::string> students = { { 101, "李华" }, { 102, "韩梅梅" } };
        // / 传统写法要用 iter->first 和 iter->second
        // 【核心代码】使用结构化绑定遍历
        // key 代表键（id），value 代表值
        for (const auto &[key, value] : students) {
            std::cout << "ID: " << key << ", 姓名: " << value << std::endl;
        }
        // 也可以单独绑定一个 pair
        std::pair<int, std::string> p{ 202, "alice" };
        auto [id, name] = p;
        std::cout << "单独 Pair -> ID: " << id << ", 姓名: " << name << std::endl;
    }
} // namespace ns2
namespace ns3 {
    // 2.4 绑定 std::tuple
    // std::tuple 是 C++11 引入的一个非常有用的标准库容器，它可以看作是 std::pair
    // 的泛化版本。std::pair 只能存两个元素，而 std::tuple 可以存储任意数量、任意类型的元素。
    // 我们也可以将它看作是超级结构体，因为它不需要预先定义结构体类型就可以把任意类型的数据打包在一起。

    // 以下是 std::tuple 的常见用法详解：

    // 头文件：使用前需要包含头文件#include <tuple>

    // 创建和初始化

    // 直接构造：最直接的方式是在尖括号中指定类型，圆括号中指定值。

    // // 存储 int, double, string
    // std::tuple<int, double, std::string> myTuple(10, 3.14, "Hello");
    // 使用 std::make_tuple (推荐)

    // auto myTuple = std::make_tuple(10, 3.14, "Hello");
    // 空元组

    // std::tuple<> emptyTuple;
    // 由于 tuple 内部的类型可能不同，不能像 vector 那样用下标 [i] 访问（因为编译器不知道 [i]
    // 返回的是什么类型）。必须使用编译期索引。

    void test() {
        // 1. 使用 std::get (索引)：索引从 0 开始。必须在编译期知道索引值。
        // 注意：如果索引越界，编译器会直接报错。
        {
            auto t = std::make_tuple(10, 3.14, "Test");
            int i = std::get<0>(t);
            double d = std::get<1>(t);
            const char *s = std::get<2>(t);
        }
        // 2.使用 std::get (类型)：如果 tuple 中没有重复的类型，也可以通过类型来获取元素。
        {
            auto t = std::make_tuple(10, 3.14, "Test");
            double d = std::get<double>(t); // 直接获取 double 类型的元素
            // int i = std::get<int>(t); // 如果有两个 int，这行会报错
        }
        // 3.结构化绑定 (C++17，最推荐)：这是目前最优雅的方式，可以将 tuple 解包到独立的变量中。
        {
            // 定义一个元组：包含 整数, 浮点数, 字符串
            auto my_data = std::make_tuple(1, 3.14, "hello");

            // 【核心代码】
            // 按顺序解开元组
            auto [num, pi, text] = my_data;
            std::cout << "整数: " << num << std::endl;
            std::cout << "圆周率: " << pi << std::endl;
            std::cout << "文本: " << text << std::endl;
            num = 10086;
            std::cout << "整数: " << std::get<0>(my_data) << std::endl;
        }
    }
    void test2() {
        // 最后再讲一下如何操作元组：

        // 1.获取元素数量 (std::tuple_size)
        using MyTuple = std::tuple<int, double, char>;
        constexpr size_t size = std::tuple_size<MyTuple>::value; // 值为 3
        // 2.修改元素：std::get 返回的是引用，所以可以直接修改：
        auto t = std::make_tuple(10, 3.14);
        std::get<0>(t) = 20; // t 的第一个元素变成了 20
        // 3.元组拼接 (std::tuple_cat)：将两个或多个元组拼接成一个新元组。
        auto t1 = std::make_tuple(1, 2);
        auto t2 = std::make_tuple("Hello", 3.5);
        auto t3 = std::tuple_cat(t1, t2);
        std::apply(
            [](const auto &...values) {
                ((std::cout << values << ", "), ...);
                std::cout << std::endl;
            },
            t3);
        // t3 的类型是 tuple<int, int, const char*, double>
        {
            auto [a, _, c] = std::tuple{ 1, 2, 3 }; // C++17 ERROR
        }
    }
} // namespace ns3
namespace ns4 {
    // 1.std::apply + 折叠表达式：遍历并处理所有元素，最简洁。你当前示例中的 t3 就采用了这种方式
    auto print_lambda_for_tuple() {
        return [](const auto &...values) {
            ((std::cout << values << ", "), ...);
            std::cout << std::endl;
        };
    }
    // 2.std::index_sequence：编译期生成 0, 1, ..., N-1，适合需要同时获取元素索引和元素值的情况。
    template<typename Tuple, std::size_t... I>
    void print_tuple(const Tuple &t, std::index_sequence<I...>) {
        ((std::cout << I << ": " << std::get<I>(t) << '\n'), ...);
    }
    template<class... Ts>
    void print_tuple(const std::tuple<Ts...> &t) {
        print_tuple(t, std::index_sequence_for<Ts...>{});
    }
    template<typename Tuple, typename Func>
    void foreach_in_tuple(Tuple &&t, Func &&f) {
        std::apply(
            [&](auto &&...args) {
                // 使用折叠表达式对每个参数执行 f
                (f(std::forward<decltype(args)>(args)), ...);
            },
            std::forward<Tuple>(t));
    }
    void test() {
        std::cout << "----------------\n";
        auto t1 = std::make_tuple(1, 2);
        auto t2 = std::make_tuple("Hello", 3.5);
        auto t3 = std::tuple_cat(t1, t2);
        std::apply(print_lambda_for_tuple(), t3);

        print_tuple(t3);

        foreach_in_tuple(t3, [](const auto &v) { std::cout << v << ","; });
    }
} // namespace ns4
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "C++17 Structured Bindings Demo\n";
    std::cout << "==============================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_bind_array();
    test_bind_tuple();
    test_bind_struct();
    test_bind_map();
    test_bind_move();

    std::cout << "\nDone!\n";

    ns1::test1();
    ns1::test2();

    ns2::test();
    ns2::test2();

    ns3::test();
    ns3::test2();

    ns4::test();
    return 0;
}
