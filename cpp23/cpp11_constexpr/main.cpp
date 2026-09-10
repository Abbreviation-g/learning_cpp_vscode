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
    //  c++11标准，函数必须要有返回值，c++14之后允许
    constexpr void func1() {
    }
    // c++11标准，返回值必须是常量表达式，c++14之后允许
    constexpr int func2() {
        int a = 1;
        return a;
    }
    // 2.函数在使用之前，必须要有对应的定义语句
    constexpr int func3();
    void test_func3() {
        func3();
    }
    constexpr int func3() {
        return 111;
    }
    // 3.c++11标准，c++14之后允许
    // 整个函数的函数体中，不能出现非常量表达式之外的语句（using 指令、typedef 语句以及 static_assert 断言、return语句除外）。
    constexpr int func4() {
        constexpr int a = 100;
        constexpr int b = 20;
        for (int i = 0; i < b; i++) {
        }
        return a + b;
    }
    constexpr int func5() {
        using mytype = long double;
        constexpr mytype a = 100;
        constexpr mytype b = 20;
        return a + b;
    }
    class Test {
    public:
        constexpr int func() {
            constexpr int var = 100;
            return 5 * var;
        }
    };
    void test_class() {
        Test t;
        constexpr int num = t.func();
        std::cout << "num: " << num << std::endl;
    }
} // namespace constexpr_ns
// 2.2 修饰模板函数
// C++11 语法中，constexpr 可以修饰函数模板，但由于模板中类型的不确定性，因此函数模板实例化后的模板函数是否符合常量表达式函数的要求也是不确定的。如果 constexpr
// 修饰的模板函数实例化结果不满足常量表达式函数的要求，则 constexpr 会被自动忽略，即该函数就等同于一个普通函数。
namespace constexpr_template_method_ns {
    struct Person {
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