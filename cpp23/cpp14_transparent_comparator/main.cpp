#include <iostream>
#include <functional>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>
// https://subingwen.cn/cpp/transparent-comparator/
//  透明操作符函数对象 -》目的是优化程序，减少拷贝
//  函数对象，例如仿函数(即重载了operator()的类创建的对象，可以像函数一样使用)
//  函数对象的特点: 1.可调用 2.可以存储状态 3.可以模板化 4.可以作为类型使用，作为参数进行数据传递

// 在 C++14 之前，标准库中的函数对象（如 std::less）是单态的，
namespace functor_ns {
    struct Add {
        int count = 0; // 存储函数对象的状态
        int operator()(int a, int b) {
            count = a + b;
            return count;
        }
    };
    void test_add() {
        Add add;
        int result = add(1, 2);
    }
    template <typename T>
    struct Mul {
        T count = 0; // 存储函数对象的状态
        T operator()(T a, T b) {
            count = a * b;
            return count;
        }
    };
    template <typename T>
    struct MyAdd {
        T count = 0; // 存储函数对象的状态
        T operator()(T a, T b) {
            count = a + b;
            return count;
        }
    };
    template <typename T, typename Func>
    int calculate(T a, T b, Func f) {
        return f(a, b);
    }
    void test_add_t() {
        MyAdd<int> add;
        int result = calculate(1, 2, add);
        Mul<int> mul;
        result = calculate(1, 2, mul);
    }
} // namespace functor_ns

// c++14 透明的函数对象
// 模板化函数调用：operator() 是一个模板函数，它可以接受任意类型 T 和 U。
// 自动类型推导：编译器根据传入的参数自动推导 T 和 U。
// 完美转发 (std::forward)：参数的引用属性（左值/右值）被保留，不产生多余的拷贝。
// decltype 尾返回类型：返回类型由 operator< 的计算结果决定。如果 T 和 U 不能比较，这段代码直接在编译期报错。
// using is_transparent = void;是透明的关键标识：标准库容器（如 std::map, std::unordered_map）会检查比较器类里是否有 is_transparent
// 这个嵌套类型。如果有，容器就知道这个比较器允许异构查找，从而在函数中开启异形参数支持。

// 2. 完整列表
// 所有标准的算术、比较和逻辑运算符在 C++14 中都增加了透明的 void 特化版本：
// 功能	普通版本	透明版本 (C++14)
// 算术运算
// 加法	std::plus	std::plus<>
// 减法	std::minus	std::minus<>
// 乘法	std::multiplies	std::multiplies<>
// 除法	std::divides	std::divides<>
// 取模	std::modulus	std::modulus<>
// 取反	std::negate	std::negate<>
// 比较运算
// 等于	std::equal_to	std::equal_to<>
// 不等于	std::not_equal_to	std::not_equal_to<>
// 大于	std::greater	std::greater<>
// 小于	std::less	std::less<>
// 大于等于	std::greater_equal	std::greater_equal<>
// 小于等于	std::less_equal	std::less_equal<>
// 逻辑运算
// 逻辑与	std::logical_and	std::logical_and<>
// 逻辑或	std::logical_or	std::logical_or<>
// 逻辑非	std::logical_not	std::logical_not<>
// 为了让大家清晰地理解 C++14 中透明操作符 (Operator<>) 与普通操作符 (Operator<T>) 的区别，下面将针对列表中的每一类操作符提供具体的代码示例。
namespace transparent_comparator_ns {
    // 非透明版本
    // 主模板
    template <typename T>
    struct MyPlus {
        T operator()(const T &a, const T &b) const {
            return a + b;
        }
    };
    void use_my_plus() {
        MyPlus<int> pi;
        pi(1, 2);
        int a = 0, b = 1;
        pi(a, b);
        MyPlus<double> pd;
        MyPlus<std::string> ps;
    }

    // 透明版本
    template <>
    struct MyPlus<void> {
        using is_transparent = void; // 标签，标记支持异构处理(不同类型的处理)
        template <typename T, typename U>
        decltype(auto) operator()(T &&a, U &&b) const { // T&& 万能引用类型，又叫做转发引用类型
            return std::forward<T>(a) + std::forward<U>(b);
        }
    };
    void test() {
        MyPlus<int>{}(1, 2);

        int res = std::plus{}(1, 2);
    }
    // 透明函数的优势
    void my_test() {
        // 优势1.使用关联容器搜索时，使用透明版本效率更高
        std::set<std::string, std::less<std::string>> s1{ "hello", "world" };
        std::set<std::string, std::less<>> s2{ "hello", "world" }; // 使用透明版本

        auto it1 = s1.find("hello"); // 隐式类型转换得到一个临时string对象
        auto it2 = s2.find("world"); // 透明版本可以接收const char*类型
        std::cout << "it1: " << *it1 << std::endl;
        std::cout << "it2: " << *it2 << std::endl;

        using namespace std::string_literals;
        using namespace std::string_view_literals;
        // 优势2.支持对不同类型的数据(异构类型)进行统一操作 -> 灵活性提高
        std::map<std::string, int, std::greater<>> m1; // 使用透明版本
        m1["apple"] = 1;
        m1["banana"] = 2;
        m1["cherry"] = 3;
        auto it3 = m1.find("apple"); // 传入const char* ，但是不产生临时对象
        auto it4 = m1.find("banana"s); // std::string
        auto it5 = m1.find("cherry"sv); // std::string_view（零拷贝，不分配内存,不产生临时对象）
        std::cout << it3->first << it3->second << std::endl;

        std::map<std::string, int, std::greater<std::string>> m2; // 使用不透明版本
        // auto it6 = m2.find("cherry"sv); // 报错，因为string_view不能隐式转换为string类型

        // 优势3.能够进行自动类型推导
        std::plus<> add;
        auto result = add(1, 2); // int
        auto result2 = add(0.0, 1.1); // double
        auto result3 = add(0, 1.2); // double
        auto result4 = add("hello"s, "dds"); // std::string

        // 非透明版本
        std::plus<int>{}(1, 2);
        std::cout << "std::plus<int>{}(1,2.2): " << std::plus<int>{}(1, 2.2) << std::endl; // 3

        std::vector<std::string> v1{ "hello", "world" };
        auto it = std::find(v1.begin(), v1.end(), "hello");
        if (it != v1.end()) {
            std::cout << "找到，索引: " << std::distance(v1.begin(), it) << ", " << *it << std::endl; // 0
        } else {
            std::cout << "未找到" << std::endl;
        }
    }

    void my_test2() {
        // 透明版本处理混合类型->作为回调函数来使用
        // 例如. 对容器的数据求和
        std::vector<int> numbers{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::vector<double> numbers2{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10.1 };
        std::vector<std::string> strs = { "apple", "banana", "cherry", "water" };
        int sum1 = std::accumulate(numbers.begin(), numbers.end(), 0, std::plus<>{});
        double sum2 = std::accumulate(numbers2.begin(), numbers2.end(), 0.0, std::plus<>{});
        std::string concat = std::accumulate(strs.begin(), strs.end(), std::string{}, std::plus<>{});
        std::cout << sum1 << std::endl;
        std::cout << sum2 << std::endl;
        std::cout << concat << std::endl;
    }

    // 优势3: 提高代码通用性
    // 实现通用的排序容器(序列容器)
    template <typename Container, typename Compare = std::less<>>
    void sort_container(Container &c, Compare &&compare = Compare{}) {
        std::sort(c.begin(), c.end(), std::forward<Compare>(compare));
    }
    struct MyTransparentLess {
        using is_transparent = void;
        template <typename T, typename U>
        auto operator()(T &&t, U &&u) const -> decltype(std::forward<T>(t) < std::forward<U>(u)) {
            return std::forward<T>(t) < std::forward<U>(u);
        }
    };
    void my_test3() {
        std::vector<int> numbers{ 10, 21, 13, 14, 45, 16, 27, 8, 9, 10 };
        std::vector<double> numbers2{ 1.8, 2.2, 3, 9.4, 5, 6, 0.7, 8, 9, 10.1 };
        std::vector<std::string> strs = { "apple", "banana", "cherry", "water" };

        auto print_vector = [](const auto &vec) { // C++20
            for (const auto &elem : vec) {
                std::cout << elem << ", ";
            }
            std::cout << std::endl;
        };

        std::cout << "before sort -----" << std::endl;
        print_vector(numbers);
        std::cout << "after sort -----" << std::endl;
        sort_container(numbers);
        print_vector(numbers);
        sort_container(numbers, std::greater<>{});
        print_vector(numbers);
        // before sort-- -- -
        // 10, 21, 13, 14, 45, 16, 27, 8, 9, 10,
        // after sort -----
        // 8, 9, 10, 10, 13, 14, 16, 21, 27, 45,
        // 45, 27, 21, 16, 14, 13, 10, 10, 9, 8,

        std::cout << "before sort -----" << std::endl;
        print_vector(numbers2);
        std::cout << "after sort -----" << std::endl;
        // 自定义一个比较器
        sort_container(numbers2, [](auto x, auto y) {
            return x < y;
        });
        print_vector(numbers2);

        std::cout << "before sort -----" << std::endl;
        print_vector(strs);
        std::cout << "after sort -----" << std::endl;
        // 自定义一个比较器
        sort_container(strs, [](const auto &a, const auto &b) {
            return a.length() < b.length();
        });
        print_vector(strs);
        std::vector<std::string> strs2 = { "c", "banana", "a", "w", "ww", "ac", "ab" };
        sort_container(strs2, MyTransparentLess{});
        print_vector(strs2);
    }
} // namespace transparent_comparator_ns

namespace logical_ns {
    struct MyObject {
        int value;
        // 自定义布尔转换（C++11 explicit bool 防止意外隐式转换，但逻辑符通常需要调用）
        explicit operator bool() const {
            return value != 0;
        }
    };
    void my_test() {
        using namespace std;
        // 1. 逻辑与：指针 和 整数
        int *ptr = nullptr;
        int num = 0;

        // 普通版本 std::logical_and<bool> 需要你先写成 bool(ptr) && bool(num)
        // 透明版本直接接受指针和整数，利用隐式转换
        std::cout << "nullptr && 0: " << logical_and{}(ptr, num) << std::endl; // 输出: 0 (false)

        // 2. 逻辑或：自定义类型和 int
        MyObject obj{ 0 };
        int number = 5;

        // 虽然 MyObject 有 explicit operator bool，但标准库透明操作符会处理这种语境
        // 注意：explicit 转换在 && || 表达式中是合法的
        std::cout << "CustomObj || int: " << logical_or{}(obj, number) << std::endl; // 输出: 1 (true)

        // 3. 逻辑非
        std::cout << "!5: " << logical_not{}(5) << std::endl; // 输出: 0 (false)
    }
} // namespace logical_ns
int main() {
    functor_ns::test_add();
    transparent_comparator_ns::my_test();
    transparent_comparator_ns::my_test2();
    transparent_comparator_ns::my_test3();
    logical_ns::my_test();
}
