#include <iostream>
// https://subingwen.cn/cpp/constexpr/
// constexpr
namespace const_ns {
    using namespace std;
    void func(const int num) {
        int a1 = 520;
        int s2 = 250;
        const int count = 24;
        int arr[count];
        cout << "arr length: " << sizeof(arr) / sizeof(arr[0]) << endl;
        // int arr2[num]; // Function parameter 'num' with unknown value cannot be used in a constant expression

        int &x = a1;
        const int &y = a1;
        cout << "y: " << y << endl;
    }
} // namespace const_ns
namespace constexpr_ns {
    // 在定义常量时，const 和 constexpr 是等价的，都可以在程序的编译阶段计算出结果，例如：
    int f() {
        return 1;
    }
    void test() {
        const int m = f(); // 不是常量表达式，m的值只有在运行时才会获取。
        const int i = 520; // 是一个常量表达式
        const int j = i + 1; // 是一个常量表达式

        constexpr int i2 = 520; // 是一个常量表达式
        constexpr int j2 = i2 + 1; // 是一个常量表达式
    }

    struct TS {
        int a;
    };
    void func() {
        constexpr TS t{ .a = 3 };
        // t.a = 1000; // 不支持
    }
    // 1.函数必须要有返回值，并且return 返回的表达式必须是常量表达式。
    //  c++11标准，函数必须要有返回值，c++14之后允许返回 void
    constexpr void func1() {
    }
    // c++11标准，函数体内只能有一条 return 语句，不能有局部变量等，
    // 因此下面这种 "先定义局部变量再返回" 的写法在 C++11 下会报
    // "body of 'constexpr' function ... not a return-statement"。
    // C++14 之后才允许。
    // constexpr int func2() {
    //     int a = 1;
    //     return a;
    // }
    // 2.函数在使用之前，必须要有对应的定义语句
    constexpr int func3();
    void test_func3() {
        func3();
    }
    constexpr int func3() {
        return 111;
    }
    // 3.c++11标准，整个函数的函数体中，不能出现非常量表达式之外的语句
    // (using 指令、typedef 语句以及 static_assert 断言、return语句除外)。
    // 因此函数体内不能有局部变量、循环、分支等，这些是 C++14 才放宽的。
    // C++14: constexpr int func4() {
    //     constexpr int a = 100;
    //     constexpr int b = 20;
    //     for (int i = 0; i < b; i++) {
    //     }
    //     return a + b;
    // }
    // C++14: constexpr int func5() {
    //     using mytype = long double;
    //     constexpr mytype a = 100;
    //     constexpr mytype b = 20;
    //     return a + b;
    // }
    class Test {
    public:
        // C++11 中隐式默认构造函数不是 constexpr, 必须显式声明,
        // 否则无法在常量表达式中使用该类型的对象。
        constexpr Test() = default;
        // C++11 要求 constexpr 函数体只能是单条 return 语句,
        // 因此这里不能先定义局部变量 `constexpr int var = 100;` 再返回,
        // 该写法(C++14 放宽)在 C++11 下会报 "not a return-statement"。
        constexpr int func() {
            return 5 * 100;
        }
    };
    void test_class() {
        // C++11 中 constexpr 成员函数会隐式成为 const 成员函数,
        // 因此只有 const 对象才能在常量表达式中调用它。
        // 若写成 `Test t; constexpr int num = t.func();` 在 C++11 下会报
        // "called in a constant expression" 错误。
        //
        // 另外, 隐式默认构造函数在 C++11 中不是 constexpr 的, 所以
        // `const Test t; constexpr int num = t.func();` 同样会失败 —— 必须
        // 显式提供 constexpr 默认构造函数, 让对象成为常量表达式可用。
        const Test t{};
        constexpr int num = t.func();
        std::cout << "num: " << num << std::endl;
    }
} // namespace constexpr_ns
// 2.2 修饰模板函数
// C++11 语法中，constexpr 可以修饰函数模板，但由于模板中类型的不确定性，因此函数模板实例化后的模板函数是否符合常量表达式函数的要求也是不确定的。如果 constexpr
// 修饰的模板函数实例化结果不满足常量表达式函数的要求，则 constexpr 会被自动忽略，即该函数就等同于一个普通函数。
namespace constexpr_template_method_ns {
    struct Person {
        // C++11 要求 constexpr 对象类型必须是字面量类型 (literal type),
        // 因此这里显式提供 constexpr 构造函数, 否则
        // `constexpr struct Person p1{ "luffy", 19 };` 无法形成常量表达式。
        constexpr Person(const char *n, int a) : name(n), age(a) {
        }
        const char *name;
        int age;
    };

    // 定义函数模板
    template <typename T>
    constexpr T dispaly(T t) {
        return t;
    }
    void test() {
        using namespace std;
        struct Person p{ "luffy", 19 };
        // 普通函数
        struct Person ret = dispaly(p);
        cout << "luffy's name: " << ret.name << ", age: " << ret.age << endl;

        // 常量表达式函数
        constexpr int ret1 = dispaly(250);
        cout << ret1 << endl;

        constexpr struct Person p1{ "luffy", 19 };
        constexpr struct Person p2 = dispaly(p1);
        cout << "luffy's name: " << p2.name << ", age: " << p2.age << endl;

        // 在上面示例程序中定义了一个函数模板 display()，但由于其返回值类型未定，因此在实例化之前无法判断其是否符合常量表达式函数的要求：
        // struct Person ret = dispaly(p);由于参数p是变量，所以实例化后的函数不是常量表达式函数，此时 constexpr 是无效的
        // constexpr int ret1 = dispaly(250);参数是常量，符合常量表达式函数的要求，此时 constexpr 是有效的
        // constexpr struct Person p2 = dispaly(p1);参数是常量，符合常量表达式函数的要求，此时 constexpr 是有效的
    }
} // namespace constexpr_template_method_ns
// 2.3 修饰构造函数
// 如果想用直接得到一个常量对象，也可以使用constexpr修饰一个构造函数，这样就可以得到一个常量构造函数了。常量构造函数有一个要求：构造函数的函数体必须为空，并且必须采用初始化列表的方式为各个成员赋值。
namespace constexpr_constructor_ns {
    struct Person {
        constexpr Person(const char *p, int age) : name(p), age(age) {
        }
        const char *name;
        int age;
    };
    void test() {
        constexpr struct Person p1("luffy", 19);
        std::cout << "luffy's name: " << p1.name << ", age: " << p1.age << std::endl;
    }
} // namespace constexpr_constructor_ns
int main() {
    const_ns::func(2);
    constexpr_ns::test();
    constexpr_ns::test_class();
    constexpr_template_method_ns::test();
    constexpr_constructor_ns::test();
}