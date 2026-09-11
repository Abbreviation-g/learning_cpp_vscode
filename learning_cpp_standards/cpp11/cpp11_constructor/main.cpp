#include <iostream>

// https://subingwen.cn/cpp/construct/
// 委托构造和继承构造函数
namespace constructor_before_ns {
    class Test {
    public:
        Test() {
        }
        Test(int max) {
            this->m_max = max > 0 ? max : 100;
        }
        Test(int max, int min) {
            this->m_max = max > 0 ? max : 100; // 冗余代码
            this->m_min = (min > 0 && min < max) ? min : 1;
        }
        Test(int max, int min, int middle) {
            this->m_max = max > 0 ? max : 100; // 冗余代码
            this->m_min = (min > 0 && min < max) ? min : 1; // 冗余代码
            this->m_middle = (middle < max && middle > min) ? middle : 50;
        }

        int m_max;
        int m_min;
        int m_middle;
    };
    void test1() {
        using namespace std;
        Test t(90, 30, 60);
        cout << "min: " << t.m_min << ", middle: " << t.m_middle << ", max: " << t.m_max << endl;
    }
} // namespace constructor_before_ns

// 在上面的程序中有三个构造函数，但是这三个函数中都有重复的代码，在C++11之前构造函数是不能调用构造函数的，加入了委托构造之后，我们就可以轻松地完成代码的优化了：
namespace constructor_after_ns {
    // 1.这种链式的构造函数调用不能形成一个闭环（死循环），否则会在运行期抛异常。

    // 2.如果要进行多层构造函数的链式调用，建议将构造函数的调用的写在初始列表中而不是函数体内部，否则编译器会提示形参的重复定义。
    // Test(int max, int min)
    // {
    //     Test(max);	// error, 此处编译器会报错, 提示形参max被重复定义
    //     this->m_min = min > 0 && min < max ? min : 1;
    // }

    // 3.在初始化列表中调用了代理构造函数初始化某个类成员变量之后，就不能在初始化列表中再次初始化这个变量了。
    // 错误, 使用了委托构造函数就不能再次m_max初始化了
    // Test(int max, int min) : Test(max), m_max(max)
    // {
    //     this->m_min = min > 0 && min < max ? min : 1;
    // }

    class Test {
    public:
        Test() {
        }
        Test(int max) {
            this->m_max = max > 0 ? max : 100;
        }
        Test(int max, int min) : Test(max) {
            this->m_min = (min > 0 && min < max) ? min : 1;
        }
        Test(int max, int min, int middle) : Test(max, min) {
            this->m_middle = (middle < max && middle > min) ? middle : 50;
        }

        int m_max;
        int m_min;
        int m_middle;
    };
    void test1() {
    }
} // namespace constructor_after_ns

namespace inherit_ns {
    class Base {
    public:
        Base(int value) : m_value(value) {
        }
        int m_value;
    };
    class Inherit : public Base {
    public:
        Inherit() : Base(100), m_value(0.0) {
        }
        Inherit(double value) : Inherit(100, value) {
        }
        Inherit(int i, double value) : Base(i), m_value(value) {
        }
        double m_value;
        double add_value() {
            return Base::m_value + m_value;
        }
    };
    void test() {
    }
} // namespace inherit_ns
// 2. 继承构造函数
namespace inherit_ns_2 {
    using namespace std;
    class Base {
    public:
        Base(int i) : m_i(i) {
        }
        Base(int i, double j) : m_i(i), m_j(j) {
        }
        Base(int i, double j, string k) : m_i(i), m_j(j), m_k(k) {
        }

        int m_i;
        double m_j;
        string m_k;
    };

    class Child : public Base {
    public:
        Child(int i) : Base(i) {
        }
        Child(int i, double j) : Base(i, j) {
        }
        Child(int i, double j, string k) : Base(i, j, k) {
        }
    };
    class Child2 : public Base {
        using Base::Base;
    };
    void test() {
        Base c = Child{ 520, 13.14, "i love you" };
        cout << "int: " << c.m_i << ", double: " << c.m_j << ", string: " << c.m_k << endl;

        Child2 c2 = { 2, 3.0, "cc" };
    }

} // namespace inherit_ns_2

// 在修改之后的子类中，没有添加任何构造函数，而是添加了using Base::Base;这样就可以在子类中直接继承父类的所有的构造函数，通过他们去构造子类对象了。
// 另外如果在子类中隐藏了父类中的同名函数，也可以通过using的方式在子类中使用基类中的这些父类函数：
namespace inherit_ns_3 {
    // 子类中的func()函数隐藏了基类中的两个func()因此默认情况下通过子类对象只能调用无参的func()，
    // 在上面的子类代码中添加了using Base::func;之后，
    // 就可以通过子类对象直接调用父类中被隐藏的带参func()函数了。
    using namespace std;
    class Base {
    public:
        Base(int i) : m_i(i) {
        }
        Base(int i, double j) : m_i(i), m_j(j) {
        }
        Base(int i, double j, string k) : m_i(i), m_j(j), m_k(k) {
        }

        void func(int i) {
            cout << "base class: i = " << i << endl;
        }

        void func(int i, string str) {
            cout << "base class: i = " << i << ", str = " << str << endl;
        }

        int m_i;
        double m_j;
        string m_k;
    };
    class Child : public Base {
    public:
        using Base::Base;
        using Base::func;
        void func() {
            cout << "child class: i'am luffy!!!" << endl;
        }
    };
    void test() {
        Child c(250);
        c.func();
        c.func(19);
        c.func(19, "luffy");
    }
} // namespace inherit_ns_3
int main() {
    constructor_before_ns::test1();

    inherit_ns_3::test();
}