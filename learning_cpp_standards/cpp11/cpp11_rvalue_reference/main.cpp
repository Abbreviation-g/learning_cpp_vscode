#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <type_traits>
#include <utility>
// https://subingwen.cn/cpp/rvalue-reference/
// 右值引用

// 1.1 右值
// C++11 增加了一个新的类型，称为右值引用（ R-value reference），标记为 &&。在介绍右值引用类型之前先要了解什么是左值和右值：

// 1. lvalue 是loactor value的缩写，rvalue 是 read value的缩写
// 2. 左值是指存储在内存中、有明确存储地址（可取地址）的数据；
// 3. 右值是指可以提供数据值的数据（不可取地址）；

// 通过描述可以看出，区分左值与右值的便捷方法是：可以对表达式取地址（&）就是左值，否则为右值 。所有有名字的变量或对象都是左值，而右值是匿名的。

namespace rvalue_reference_ns {
    static int &&x = 5;
    void test1() {
        // 左值
        int num = 9;
        // 左值引用
        int &a = num;
        // 右值
        // 右值引用
        int &&b = 8;
        // 常量右值引用
        const int &&d = 6;
        // const int &&dd = b; // error
        // constexpr int&& d = b; // error
        // 常量左值引用，可以用同类型左值引用右值引用，常量左值引用常量右值引用进行初始化
        const int &c = num;
        const int &cc = b;
        const int &ccc = d;
    }

    // 1.2 右值引用
    // 右值引用就是对一个右值进行引用的类型。
    // 因为右值是匿名的，所以我们只能通过引用的方式找到它。
    // 无论声明左值引用还是右值引用都必须立即进行初始化，因为引用类型本身并不拥有所绑定对象的内存，
    // 只是该对象的一个别名。
    // 通过右值引用的声明，该右值又“重获新生”，其生命周期与右值引用类型变量的生命周期一样，
    // 只要该变量还活着，该右值临时量将会一直存活下去。
    int &&value = 520;
    class Test {
    public:
        // Test(const Test &a) = delete;
        Test() : m_num(new int(100)) {
            std::cout << "constructor Test():m_num(new int(100)); " << "\n";
            std::printf("m_num的地址: %p\n", m_num);
        }
        // 拷贝构造
        Test(const Test &a) : m_num(new int(*a.m_num)) {
            std::cout << "copy constructor Test(const Test& a):m_num(new int(*a.m_num))" << "\n";
        }
        // 移动构造(复用另外一个对象里的资源(堆内存)) , m_num 浅拷贝
        Test(Test &&a) : m_num(a.m_num) {
            // a对象的指针指向空
            a.m_num = nullptr;
            std::cout << "move constructor Test(Test&& a): m_num(a.m_num)" << "\n";
        }
        ~Test() {
            // if (m_num != nullptr) {
            std::cout << "destructor " << std::endl;
            delete m_num;
            m_num = nullptr;
            // }
        }
        int *m_num;
    };
    Test get_obj() {
        Test t;
        return t;
    }
    Test get_obj2() {
        return Test{};
    }
    Test &&get_obj3() {
        return Test{};
    }
    void test2() {
        // 用 GCC 禁止省略拷贝：
        // g++ -std=c++11 -fno-elide-constructors main.cpp -o test.exe
        //  ./test.exe
        //  constructor Test():m_num(new int(100));
        // m_num的地址: 00000257e8298e90
        // copy constructor Test(const Test& a):m_num(new int(*a.m_num))
        // destructor
        // copy constructor Test(const Test& a):m_num(new int(*a.m_num))
        // destructor
        // destructor

        // Test t;
        // Test temporary(t); // 拷贝 1
        // Test obj(temporary); // 拷贝 2
        // Test t = get_obj();
    }

    void test3() {
        // g++ -std=c++11 -fno-elide-constructors main.cpp -o test.exe
        //.\test.exe
        // constructor Test():m_num(new int(100));
        // m_num的地址: 000001edc9870660
        // move constructor Test(Test&& a): m_num(a.m_num)
        // destructor
        // move constructor Test(Test&& a): m_num(a.m_num)
        // destructor
        // destructor
        // 要求右侧对象是一个临时对象，才会调用移动构造函数
        Test t = get_obj();
    }
    void test4() {
        // constructor Test():m_num(new int(100));
        // m_num的地址: 000002147f458ed0
        // ---------
        // m_num的地址: 000002147f458ed0
        // destructor
        // 要求右侧对象是一个临时对象，才会调用移动构造函数
        // 如果没有移动构造函数，就会调用拷贝构造
        Test &&t = get_obj();
        std::cout << "---------" << "\n";
        std::printf("m_num的地址: %p\n", t.m_num);
        // 如果没有移动构造，使用右值引用初始化要求更高一些，要求右侧是一个临时的不能取地址的对象
        Test &&t2 = get_obj2();
        Test &&t3 = get_obj3();
        // Test&& t4 = t; // Rvalue reference to type 'Test' cannot bind to lvalue of type 'Test'clang(lvalue_to_rvalue_ref)
        Test &&t4 = std::move(t);
        Test &&t5 = std::move(get_obj());
    }
    void test5() {
        Test t{};
        Test &&t2 = std::move(t);
    }
} // namespace rvalue_reference_ns

// 3 && 的特性
// 在C++中，并不是所有情况下 && 都代表是一个右值引用，
// 具体的场景体现在模板和自动类型推导中，
// 如果是模板参数需要指定为T&&，
// 如果是自动类型推导需要指定为auto &&，
// 在这两种场景下 &&被称作未定的引用类型。
// 另外还有一点需要额外注意const T&&表示一个右值引用，不是未定引用类型。

// C++11中引用折叠的规则如下：
// 通过右值推导 T&& 或者 auto&& 得到的是一个右值引用类型
// 通过非右值（右值引用、左值、左值引用、常量右值引用、常量左值引用）推导 T&& 或者 auto&& 得到的是一个左值引用类型
namespace rvalue_reference_ns2 {
    template <typename T>
    void f(T &&param) {
        bool is_lvalue_r = std::is_lvalue_reference<decltype(param)>::value;
        std::cout << (is_lvalue_r ? "param is an lvalue" : "param is an rvalue") << "\n";
    }
    template <typename T>
    void f1(const T &&param) {
        bool is_lvalue_r = std::is_lvalue_reference<decltype(param)>::value;
        std::cout << (is_lvalue_r ? "param is an lvalue" : "param is an rvalue") << "\n";
    }
    void test1() {
        f(10); // param is an rvalue
        f1(10); // param is an rvalue
        int x = 11;
        f(x); // param is an lvalue
        f1(std::move(x)); // param is an rvalue
    }
    void test2() {
        int &&a1 = 5; // 右值引用
        auto &&bb = a1; // a1为右值引用，推导出的bb为左值引用类型
        auto &&bb1 = 5; // 5为右值，推导出的bb1为右值引用类型

        int a2 = 5;
        int &a3 = a2; // 左值引用
        auto &&cc = a3; // a3为左值引用，推导出的cc为左值引用类型
        auto &&cc1 = a2; // a2为左值，推导出的cc1为左值引用类型

        const int &s1 = 100; // 常量左值引用
        const int &&s2 = 100; // 常量右值引用
        auto &&dd = s1; // s1为常量左值引用，推导出的dd为常量左值引用类型
        auto &&ee = s2; // s2为常量右值引用，推导出的ee为常量左值引用类型

        const auto &&x = 5; // x为常量右值引用，不需要推导，只能通过右值初始化
    }
} // namespace rvalue_reference_ns2
namespace rvalue_reference_ns3 {
    using namespace std;
    void printValue(int &i) {
        cout << "l-value: " << i << endl;
    }

    void printValue(int &&i) {
        cout << "r-value: " << i << endl;
    }

    void forward(int &&k) {
        printValue(k);
    }
    void test1() {
        int x = 111;
        printValue(x); // l-value: 111
        printValue(1223); // r-value: 1223

        forward(std::move(x)); // l-value: 111
        forward(250); // l-value: 250
    }
    template <typename T>
    void forward2(T &&k) {
        printValue(std::forward<T>(k));
    }
    void test2() {
        int x = 111;
        forward2(x); // l-value: 111
        forward2(std::move(x)); // r-value: 111
        forward2(250); // r-value: 250
    }
} // namespace rvalue_reference_ns3
int main() {
    // rvalue_reference_ns::test2();
    // rvalue_reference_ns::test3();
    // rvalue_reference_ns::test4();
    // rvalue_reference_ns::test5();

    // rvalue_reference_ns2::test1();
    rvalue_reference_ns3::test1();
    rvalue_reference_ns3::test2();
    return 0;
}