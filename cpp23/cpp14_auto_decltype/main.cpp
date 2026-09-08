#include <iostream>
#include <vector>
// https://subingwen.cn/cpp/auto-decltype/
// 1: auto -> 进行返回值类型推导
//  但问题是1.1移除所有引用。1.2移除虽有const volatile
// 2: decltype(auto)

namespace auto_decltype_ns {
    // c++11
    // -> int 指定返回值太多余
    auto func_cpp11() -> int {
        return 0;
    }
    // c++14
    // 1.如果有多个返回语句，那么要保证返回值类型一致
    auto func_cpp14(int x, double y) {
        if (x == 0) {
            return x + y;
        }
        if (1 == x) {
            // return 1; // 'auto' in return type deduced as 'int' here but deduced as 'double' in earlier return statementclang(auto_fn_different_deductions)
            return 1.0;
        }
        return x * y;
    }
    // c++14
    // 2.也可以用于递归函数推导返回值 -> 必须要有非递归的返回语句
    auto factorial(int x) {
        if (x <= 1) {
            return 1;
        }
        return (x * factorial(x - 1));
    }
    // c++14
    // 3. auto会剥离所有的引用和const volatile限定，特殊情况const + 指针
    int &get_ref() {
        static int x = 10;
        return x;
    }
    // 返回值是int，剥离了引用
    auto func1() {
        return get_ref();
    }
    // 返回值int&
    auto &func2() {
        return get_ref();
    }
    // 返回值int
    auto func3() {
        const int x = 100;
        return x;
    }
    // 返回值const char*
    // 因为const char* 中的const是底层const（指向的内容不可变），属于类型的一部分，传值拷贝是会被保留
    auto func4() {
        const char *name = ""; // 这个字符串被存储到静态存储区，所以返回没有问题
        return name;
    }
    auto func5() {
        static int name[] = { 1, 2, 3, 4 };
        return &name;
    }
} // namespace auto_decltype_ns

namespace decltype_ns {
    // decltype(auto)
    // c++11 返回值后置 必须使用在模板函数中
    // auto -> 会剥离引用，剥离const和volatile
    // decltype -> 可以保留引用，保留const volatile
    template <typename T, typename U>
    auto func5(T t, U u) -> decltype(t + u) {
        return u + t;
    }
    auto call_func5() {
        std::string s1 = "hello";
        const std::string s2 = "world";
        auto result = func5(s1, s2); // std::string
        std::cout << "result: " << result << "," << typeid(result).name() << std::endl; // NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
    }
    template <typename T, typename U>
    decltype(auto) func6(const T &t, const U &u) {
        return u + t;
    }
    auto call_func6() {
        std::string s1 = "hello";
        const std::string s2 = "world";
        auto result = func6(s1, s2); // std::string
        std::cout << "result: " << result << "," << typeid(result).name() << std::endl; // NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
    }
    std::string &get_name() {
        static std::string name = "ddd";
        return name;
    }
    auto call_get_name() {
        auto name1 = get_name(); // std::string
        decltype(auto) name2 = get_name(); // std::string&
    }
    // 返回值类型int&
    decltype(auto) get_element(std::vector<int> &vec, size_t index) {
        return vec.at(index);
    }
} // namespace decltype_ns
namespace decltype_ns_2 {
    //     decltype(auto) 对变量名和表达式的不同处理方式：
    // 1.如果 decltype 里的参数是一个不加括号的变量名（如 x），推导结果就是该变量的类型。
    // 2.如果decltype里的参数是一个表达式（如x+y或vec[i]），推导结果取决于该表达式的值类别：
    //      只要 decltype 的参数被括号 ( ... ) 包裹，该参数在语法上就被视为一个表达式，而不是一个变量名。
    //      既然是表达式，接下来就要看这个表达式是左值 还是 右值：
    //          A)如果是左值（L-value，如变量），推导结果为 类型的引用 (T&)。
    //          B)如果是右值（R-value，如临时对象或字面量），推导结果为 类型本身 (T)。
    void test() {
        int x = 9;
        decltype(auto) v1 = x; // int
        decltype(auto) v2 = (x); // int& // A)如果是左值（L-value，如变量），推导结果为 类型的引用 (T&)。
    }
    int x = 10;
    // 返回值int
    decltype(auto) test1() {
        return x;
    }
    // 返回值int&
    decltype(auto) test2() {
        return (x);
    }
    int m = 1, n = 3;
    // 返回值int // B)如果是右值（R-value，如临时对象或字面量），推导结果为 类型本身 (T)。
    decltype(auto) test3() {
        return (m + n);
    }
} // namespace decltype_ns_2
int main() {
    decltype_ns::call_func5();
    decltype_ns::call_func6();
}
