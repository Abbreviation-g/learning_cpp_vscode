#include <array>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>

// https://subingwen.cn/cpp/autotype/
// 自动类型推导
// 1. auto
// 在C++11之前auto和static是对应的，表示变量是自动存储的，但是非static的局部变量默认都是自动存储的，因此这个关键字变得非常鸡肋，
// 在C++11中他们赋予了新的含义，使用这个关键字能够像别的语言一样自动推导出变量的实际类型。
// 1.1 推导规则
// C++11中auto并不代表一种实际的数据类型，只是一个类型声明的 “占位符”，auto并不是万能的在任意场景下都能够推导出变量的实际类型，
// 使用auto声明的变量必须要进行初始化，以让编译器推导出它的实际类型，在编译时将auto占位符替换为真正的类型。使用语法如下：

namespace auto_ns {
    void test() {
        auto x = 3.14;
        auto y = 530;
        auto z = 520L;
        auto zz = 'c';
        auto zx = 3.14L; // long double
        short zy = 1;
        auto zyx = zy; // short

        int temp = 110;
        auto *p = &temp; // int*
        auto b = &p; // int **
        auto &c = temp; // int &
        auto &cc = p; // int* &
        auto d = temp; // int

        // 不仅如此，auto还可以和指针、引用结合起来使用也可以带上const、volatile限定符，在不同的场景下有对应的推导规则，规则内容如下：
        // 当变量不是指针或者引用类型时，推导的结果中不会保留const、volatile关键字
        // 当变量是指针或者引用类型时，推导的结果中会保留const、volatile关键字

        // const修饰
        int tmp = 250;
        const auto a1 = tmp; // const int
        auto a2 = a1; // int
        const auto a22 = a1; // const int
        const auto &a3 = tmp; // const int &
        auto &a4 = a3; // const int&

        auto *pt4 = &a1; // const int*
        const auto *const pt5 = &tmp; // const int* const
        // pt5 = nullptr; // 错误
    }

    //     1.2 auto的限制
    // auto关键字并不是万能的，在以下这些场景中是不能完成类型推导的：

    // 1.2.1不能作为函数参数使用。因为只有在函数调用的时候才会给函数参数传递实参，auto要求必须要给修饰的变量赋值，因此二者矛盾。
    // int func(auto a, auto b){} //报错

    // 1.2.2不能用于类的非静态成员变量的初始化
    class TestClass {
    public:
        // auto v1 = 0; // 报错
        // static auto v2 = 0; // 报错
        static const auto v3 = 10; // 正确，可以静态成员变量的初始化
    };

    // 1.2.3 不能使用auto关键字定义数组
    void test_array_auto() {
        int arr[]{ 1, 2 };
        auto t1 = arr; // int*
        // auto t2[] = arr; // error, auto无法定义数组
        // auto t3[] = { 1, 2, 3 }; // error, auto无法定义数组
    }

    // 1.2.4无法使用auto推导出模板参数
    template <typename T>
    struct TestStruct {};
    void test_auto_struct() {
        TestStruct<double> t1;
        TestStruct<double> t2 = t1;
        // TestStruct<auto> t3 = t1; // 报错
    }
} // namespace auto_ns

// auto的应用
namespace auto_usage_ns {
    // 1.用于STL的容器遍历。
    void test() {
        using namespace std;
        map<int, string> mp;
        mp.insert(make_pair(1, "xx"));
        mp.insert(make_pair(2, "yy"));
        mp.insert(make_pair(3, "zz"));

        // 在C++11之前，定义了一个stl容器之后，遍历的时候常常会写出这样的代码：

        for (map<int, string>::iterator it = mp.begin(); it != mp.end(); it++) {
            cout << "key: " << it->first << ", value: " << it->second << endl;
        }
        // 可以看到在定义迭代器变量 it 的时候代码是很长的，写起来就很麻烦，使用了auto之后，就变得清爽了不少：
        for (auto it = mp.begin(); it != mp.end(); it++) {
            cout << "key: " << it->first << ", value: " << it->second << endl;
        }
        for (const auto &it : mp) {
            cout << "key: " << it.first << ", value: " << it.second << endl;
        }
    }

    // 2.用于泛型编程，在使用模板的时候，很多情况下我们不知道变量应该定义为什么类型，比如下面的代码：
    class T1 {
    public:
        static int get() {
            return 10;
        }
    };
    class T2 {
    public:
        static std::string get() {
            return "hello";
        }
    };
    template <typename T>
    void func(void) {
        auto result = T::get();
        std::cout << "result: " << result << std::endl;
    }
    template <typename T, typename R>
    void func2(void) {
        R result = T::get();
        std::cout << "result: " << result << std::endl;
    }
    void test2() {
        func<T1>();
        func<T2>();

        func2<T1, int>();
        func2<T2, std::string>();
    }
} // namespace auto_usage_ns

// 2. decltype
// 在某些情况下，不需要或者不能定义变量，但是希望得到某种类型，这时候就可以使用C++11提供的decltype关键字了，它的作用是在编译器编译的时候推导出一个表达式的类型，语法格式如下：
// decltype (表达式)
// decltype 是“declare type”的缩写，意思是“声明类型”。decltype的推导是在编译期完成的，它只是用于表达式类型的推导，并不会计算表达式的值。来看一组简单的例子：
namespace decltype_ns {
    void test() {
        int a = 10;
        decltype(a) b = a;
        decltype(a + 3.14) c = a + 3.14;
        decltype(a + b * c) d = a + b * c;
    }
    // 2.1 推导规则
    // 1.表达式为普通变量或者普通表达式或者类表达式，在这种情况下，使用decltype推导出的类型和表达式的类型是一致的。
    class Test {
    public:
        Test() {
        }
        Test(std::string s, int n) : text(s), num(n) {
        }
        std::string text;
        int num = 0;
        static const int value = 110;
    };
    void test1() {
        int x = 99;
        const int &y = x;
        decltype(x) a = x; // int
        decltype(y) b = y; // const int&

        decltype(Test::value) c = 0; // const int
        Test t;
        decltype(t.text) d; // std::string
    }
    // 2.表达式是函数调用，使用decltype推导出的类型和函数返回值一致。
    int func_int() {
        return 0;
    }
    int &func_int_ref() {
        static int a = 0;
        return a;
    }
    int &&func_int_rr() {
        static int a = 0;
        return std::move(a);
    }
    // 函数 func_cint()
    // 返回的是一个纯右值（在表达式执行结束后不再存在的数据，也就是临时性的数据），对于纯右值而言，
    // 只有类类型可以携带const、volatile限定符，除此之外需要忽略掉这两个限定符，因此推导出的变量d的类型为
    // int 而不是 const int。
    const int func_cint() {
        return 0;
    }
    const int &func_cint_ref() {
        static int a = 0;
        return a;
    }
    const int &&func_cint_rr() {
        return 0;
    }
    // 只有类类型可以携带const、volatile限定符，除此之外需要忽略掉这两个限定符
    const Test func_ctest() {
        return Test{ std::string("xx"), 1 };
    }
    void test2() {
        int num = 0;
        decltype(func_int()) a = func_int(); // int
        decltype(func_int_ref()) b = func_int_ref(); // int&
        decltype(func_int_rr()) c = func_int_rr(); // int&&
        decltype(func_cint()) d = func_cint(); // int
        decltype(func_cint_ref()) e = func_cint_ref(); // const int&
        decltype(func_cint_rr()) f = func_cint_rr(); // const int&&
        decltype(func_ctest()) g = func_ctest(); // const Test
    }

    // 3.表达式是一个左值，或者被括号( )包围，使用 decltype推导出的是表达式类型的引用（如果有const、volatile限定符不能忽略）。
    void test3() {
        const Test obj{ "x", 99 };

        decltype(obj.text) a = obj.text; // std::string
        // 带括号的表达式
        decltype((obj.text)) b = obj.text; // const std::string&
        decltype((obj.num)) bb = obj.num; // const int&

        Test obj2{ "x", 99 };
        // 带括号的表达式
        decltype((obj2.text)) b2 = obj2.text; // std::string&
        decltype((obj2.num)) bb2 = obj2.num; // int&

        // 加法表达式
        int n = 1, m = 10;
        decltype(n + m) c = n + m; // int
        decltype(n = n + m) d = n = n + m; // int& n=n+m 得到一个左值 n，符合场景3，因此d的类型为 int&
    }
} // namespace decltype_ns

// 2.2 decltype的应用
namespace decltype_usage_ns {
    template <class T>
    class Container {
    public:
        void print_container(T &c) {
            for (m_it = c.begin(); m_it != c.end(); m_it++) {
                std::cout << "value:" << *m_it << ", ";
            }
            std::cout << std::endl;
        }

    private:
        decltype(T().begin()) m_it;
    };

    void test() {
        std::list<int> ls{ 1, 4, 1, 6, 99, 28 };
        Container<decltype(ls)> c;
        c.print_container(ls); // value:1, value:4, value:1, value:6, value:99, value:28,

        std::array<double, 5> arr{ 0.0, 2.2, 9, 0.1, -0.2 };
        Container<decltype(arr)>{}.print_container(arr);

        const std::array<double, 5> arr2{ 0.0, 2.2, 9, 0.1, -0.2 };
        Container<decltype(arr2)>{}.print_container(arr2);
    }
} // namespace decltype_usage_ns

// 3. 返回类型后置
namespace trailing_return_type_ns {
    template <typename R, typename T, typename U>
    R add(T t, U u) {
        return t + u;
    }
    template <typename T, typename U>
    auto add2(T t, U u) -> decltype(t + u + 0) {
        return (t * 1000) + (u / 100) + ((static_cast<int>(u)) % 3);
    }
    void test() {
        int x = 1;
        double y = 12.4;
        auto ret = add<decltype(x + y), int, double>(x, y);
        std::cout << ret << std::endl;

        auto ret2 = add2(x, y);
        std::cout << ret2 << std::endl;
    }
} // namespace trailing_return_type_ns

int main() {
    auto_usage_ns::test();
    auto_usage_ns::test2();
    decltype_usage_ns::test();
    trailing_return_type_ns::test();
}