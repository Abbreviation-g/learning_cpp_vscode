// https://subingwen.cn/cpp/POD/
// POD类型
#include <iostream>

// 1. POD 类型
// POD是英文中 Plain Old Data 的缩写，翻译过来就是普通的旧数据
// 。POD在C++中是非常重要的一个概念，通常用于说明一个类型的属性，尤其是用户自定义类型的属性。

// POD属性在C++11中往往又是构建其他C++概念的基础，事实上，在C++11标准中，POD出现的概率相当高。因此学习C++，尤其是在
// C++11中，了解 POD的概念是非常必要的。

// Plain ：表示是个普通的类型
// Old ：体现了其与C的兼容性，支持标准C函数
// 在C++11中将 POD划分为两个基本概念的合集，即∶平凡的（trivial） 和标准布局的（standard layout ） 。

// 2. “平凡”类型
// 2.1 拥有平凡的默认构造函数（trivial constructor）和析构函数（trivial destructor）。
// 平凡的默认构造函数就是说构造函数什么都不干。
// 通常情况下，不定义类的构造函数，编译器就会为我们生成一个平凡的默认构造函数。
// // 使用默认的构造函数
// class Test {};
// 一旦定义了构造函数，即使构造函数不包含参数，函数体里也没有任何的代码，那么该构造函数也不再是"平凡"的。
// class Test1
// {
//     Test1();	// 程序猿定义的构造函数, 非默认构造
// };
// 关于析构函数也和上面列举的构造函数类似，一旦被定义就不平凡了。但是这也并非无药可救，使用=default关键字可以显式地声明默认的构造函数，从而使得类型恢复
// “平凡化”。
// 2.2 拥有平凡的拷贝构造函数（trivial copy constructor）和移动构造函数（trivial move
// constructor）。 平凡的拷贝构造函数基本上等同于使用memcpy 进行类型的构造。
// 同平凡的默认构造函数一样，不声明拷贝构造函数的话，编译器会帮程序员自动地生成。
// 可以显式地使用=default 声明默认拷贝构造函数。
// 而平凡移动构造函数跟平凡的拷贝构造函数类似，只不过是用于移动语义。
// 2.3 拥有平凡的拷贝赋值运算符（trivial assignment operator）和移动赋值运算符（trivial move
// operator）。
//  这基本上与平凡的拷贝构造函数和平凡的移动构造运算符类似。
// 2.4 不包含虚函数以及虚基类。
// 类中使用virtual 关键字修饰的函数 叫做虚函数
// class Base {
// public:
//     Base() {}
//     virtual void print() {}
// };
// 虚基类是在创建子类的时候在继承的基类前加virtual 关键字 修饰
// 语法: class 派生类名：virtual  继承方式  基类名
// class Base {
// public:
//     Base() {}
// };
// // 子类Child，虚基类：Base
// class Child : virtual public Base {
//     Child() {}
// };

// 3. “标准布局”类型
// 标准布局类型主要主要指的是类或者结构体的结构或者组合方式。
// 标准布局类型的类应该符合以下五点定义，最重要的为前两条：
// 3.1 所有非静态成员有相同 的访问权限（public，private，protected）。
// 类成员拥有不同的访问权限（非标准布局类型）
// class Base{
// public:
//     Base() {}
//     int a;
// protected:
//     int b;
// private:
//     int c;
// };
// 类成员拥有相同的访问权限（标准布局类型）
// class Base{
// public:
//     Base() {}
//     int a;
//     int b;
//     int c;
// };
// 3.2 在类或者结构体继承时，满足以下两种情况之一∶
// 派生类中有非静态成员，基类中包含静态成员（或基类没有变量）。
// 基类有非静态成员，而派生类没有非静态成员。
// struct Base { static int a;};
// struct Child: public Base{ int b;};          // ok
// struct Base1 { int a;};
// struct Child1: public Base1{ static int c;}; // ok
// struct Child2:public Base, public Base1 { static int d;); // ok
// struct Child3:public Base1{ int d;};         // error
// struct Child4:public Base1, public Child     // error
// {
//     static int num;
// };
// 通过上述例子得到的结论：
// 非静态成员只要同时出现在派生类和基类间，即不属于标准布局。
// 对于多重继承，一旦非静态成员出现在多个基类中，即使派生类中没有非静态成员变量，派生类也不属于标准布局。

// 4.1 对“平凡”类型判断
// C++11提供的类模板叫做 is_trivial，其定义如下：

// template <class T> struct std::is_trivial;
// std::is_trivial 的成员value 可以用于判断T的类型是否是一个平凡的类型（value
// 函数返回值为布尔类型）。除了类和结构体外，is_trivial还可以对内置的标准类型数据（比如int、float都属于平凡类型）及数组类型（元素是平凡类型的数组总是平凡的）进行判断。
namespace is_trivial_ns {
    using namespace std;
    class A {};
    class B {
        B() {}
    };
    class C : B {};
    class D {
        virtual void fn() {}
    };
    class E : virtual public A {};
    class F : public A {};

    void test() {
        // clang-format off
        cout << std::boolalpha;
        cout << "is_trivial:" << std::endl;
        cout << "int: " << is_trivial<int>::value << endl; // true int ：内置标准数据类型，属于 trivial 类型
        cout << "A: " << is_trivial<A>::value << endl; // true A ：拥有默认的构造和析构函数，属于 trivial 类型
        cout << "B: " << is_trivial<B>::value << endl; // false B ：自定义了构造函数，因此不属于 trivial 类型
        cout << "C: " << is_trivial<C>::value << endl; // false C ：基类中自定义了构造函数，因此不属于 trivial 类型
        cout << "D: " << is_trivial<D>::value << endl; // false D ：类成员函数中有虚函数，因此不属于 trivial 类型
        cout << "E: " << is_trivial<E>::value << endl; // false E ：继承关系中有虚基类，因此不属于 trivial 类型
        cout << "F: " << is_trivial<F>::value << endl; // true
        // clang-format on
    }
} // namespace is_trivial_ns

// 4.2 对“标准布局”类型的判断
// 同样，在C++11中，我们可以使用模板类来帮助判断类型是否是一个标准布局的类型，其定义如下：

// template <typename T> struct std::is_standard_layout;
// 通过 is_standard_layout模板类的成员
// value（is_standard_layout<T>∶∶value），我们可以在代码中打印出类型的标准布局属性，函数返回值为布尔类型。
namespace is_standard_layout_ns {
    struct A {};
    struct B : A {
        int j;
    };
    struct C {
    public:
        int a;

    private:
        int c;
    };
    struct D1 {
        static int i;
    };
    struct D2 {
        int i;
    };
    struct E1 {
        static int i;
    };
    struct E2 {
        int i;
    };
    struct D : public D1, public E1 {
        int a;
    };
    struct E : public D1, public E2 {
        int a;
    };
    struct F : public D2, public E2 {
        static int a;
    };
    struct G : public A {
        int foo;
        A a;
    };
    struct H : public A {
        A a;
        int foo;
    };
    struct H2 : public A {
        int foo;
        A a;
    };
    void test() {
        using namespace std;
        cout << std::boolalpha;
        // clang-format off
        cout << "is_standard_layout:" << std::endl;
        cout << "A: " << is_standard_layout<A>::value << endl; // true A ：没有虚基类和虚函数，属于 standard_layout 类型
        cout << "B: " << is_standard_layout<B>::value << endl; // true B ：没有虚基类和虚函数，属于 standard_layout 类型
        cout << "C: " << is_standard_layout<C>::value << endl; // false C ：所有非静态成员访问权限不一致，不属于 standard_layout 类型
        cout << "D: " << is_standard_layout<D>::value << endl; // true D ：基类和子类没有同时出现非静态成员变量，属于 standard_layout 类型
        cout << "D1: " << is_standard_layout<D1>::value << endl; // true D1 ：没有虚基类和虚函数，属于 standard_layout 类型
        cout << "E: " << is_standard_layout<E>::value << endl; // false E ：基类和子类中同时出现了非静态成员变量，不属于 standard_layout 类型
        cout << "F: " << is_standard_layout<F>::value << endl; // false F ：多重继承中在基类里同时出现了非静态成员变量，不属于 standard_layout 类型
        cout << "G: " << is_standard_layout<G>::value << endl; // true
        cout << "H: " << is_standard_layout<H>::value << endl; // false H ：子类中第一个非静态成员的类型与其基类类型不能相同，不属于 standard_layout 类型
        cout << "H2: " << is_standard_layout<H2>::value << endl; // true
        // clang-format on
    }
} // namespace is_standard_layout_ns
// 事实上，我们使用的很多内置类型默认都是 POD的。POD
// 最为复杂的地方还是在类或者结构体的判断。不过上面也给大家介绍了判断的方法，相信大家对
// POD已经有所理解。那么，使用POD有什么好处呢？

// 字节赋值，代码中我们可以安全地使用memset 和 memcpy 对 POD类型进行初始化和拷贝等操作。
// 提供对C内存布局兼容。C++程序可以与C 函数进行相互操作，因为POD类型的数据在C与C++
// 间的操作总是安全的。
// 保证了静态初始化的安全有效。静态初始化在很多时候能够提高程序的性能，而POD类型的对象初始化往往更加简单

namespace virtual_inherit_ns {
    using namespace std;
    class A {
    public:
        A() {
            cout << "A的构造函数" << "\n";
        }
        int data;
    };

    class B : public A {}; // B 继承 A
    class C : public A {}; // C 也继承 A

    class D : public B, public C {}; // D 同时继承 B 和 C
    void test() {
        D d;
        // A的构造函数
        // A的构造函数

        // 问题1 A被构造了两次

        // d.data = 100; // main.cpp(213, 13): Member found by ambiguous name lookup
        // 问题2 ‌二义性‌：访问 d.data 时，编译器不知道是访问 B::A::data
        // 还是 `C::A::data。
    }
} // namespace virtual_inherit_ns
namespace virtual_inherit_ns2 {
    class A {
    public:
        int data;
        A() : data(0) {
            std::cout << "A 构造" << std::endl;
        }
        A(int i) : data(i) {
            std::cout << "A 构造(int i)" << std::endl;
        }
    };

    // 使用 virtual 关键字声明虚继承
    class B : virtual public A {
    public:
        B() {
            std::cout << "B 构造" << std::endl;
        }
    };

    class C : virtual public A {
    public:
        C() {
            std::cout << "C 构造" << std::endl;
        }
    };

    // D 继承 B 和 C
    class D : public B, public C {
    public:
        D() {
            std::cout << "D 构造" << std::endl;
        }
    };
    class D2 : public B, public C {
    public:
        // D 必须显式调用 A 的构造函数
        D2(int i) : A(i), B(), C() {
            // A(10) 真正生效
            // B() 和 C() 中对 A 的构造调用被忽略
        }
    };

    void test() {
        D d;
        d.data = 10086;
        //  A 构造
        //  B 构造
        //  C 构造
        //  D 构造

        // D 的对象中只包含‌一个‌ A 的子对象。
        // B 和 C 共享这唯一的 A 实例。
        // 访问 d.data 不再有二义性。

        D2 d2(10099);
        // A 构造(int i)
        // B 构造
        // C 构造

        //         3. 关键特性
        // 1. 唯一实例
        // 无论继承路径有多少条，虚基类在最终派生类中只存在‌一份‌副本。

        // 2. 构造函数调用规则
        // ‌最派生类负责初始化虚基类‌：
        // 在创建 D 的对象时，‌只有 D 的构造函数可以直接调用 A
        // 的构造函数‌。
    }
} // namespace virtual_inherit_ns2
namespace virtual_inherit_ns3 {
#include <iostream>

    class Animal {
    public:
        Animal() {
            std::cout << "Animal 构造" << std::endl;
        }
        void breathe() {
            std::cout << "呼吸" << std::endl;
        }
    };

    // 虚继承 Animal
    class Flyer : virtual public Animal {
    public:
        Flyer() {
            std::cout << "Flyer 构造" << std::endl;
        }
    };

    // 虚继承 Animal
    class Swimmer : virtual public Animal {
    public:
        Swimmer() {
            std::cout << "Swimmer 构造" << std::endl;
        }
    };

    // 多重继承
    class Duck : public Flyer, public Swimmer {
    public:
        Duck() {
            std::cout << "Duck 构造" << std::endl;
        }
    };

    void test() {
        Duck d;
        d.breathe(); // ✅ 无二义性，只有一个 Animal 子对象
        // Animal 构造
        // Flyer 构造
        // Swimmer 构造
        // Duck 构造
        // 呼吸
    }
    // 目的‌：解决多重继承中的二义性和数据冗余。
    // ‌语法‌：class Derived : virtual public Base。
    // ‌结果‌：虚基类在最终对象中只有一份实例。
    // ‌代价‌：轻微的运行时性能开销和更复杂的初始化规则。
} // namespace virtual_inherit_ns3

namespace virtual_method_ns {
    class Base {
    public:
        void print() {
            std::cout << "Base::print" << std::endl;
        }
    };
    class Derived : public Base {
    public:
        void print() {
            std::cout << "Derived::print" << std::endl;
        }
    };
    void call_print(Base *b) {
        b->print();
    }
    void test() {
        Derived d;
        call_print(&d); // Base::print
    }
    class Base2 {
    public:
        virtual void print() { // 虚函数
            std::cout << "Base2::print" << std::endl;
        }
    };
    class Derived2 : public Base2 {
    public:
        void print() {
            std::cout << "Derived2::print" << std::endl;
        }
    };
    void call_print(Base2 *b) {
        b->print();
    }
    void test2() {
        Derived2 d2;
        call_print(&d2); // Derived2::print
    }
    class Base3 {
    public:
        virtual void print() = 0; //  纯虚函数
    };
    class Derived3 : public Base3 {
    public:
        void print() final override{
            std::cout << "Derived3::print" << std::endl;
        }
    };
    void call_print(Base3 *b) {
        b->print();
    }
    void test3(){
        Derived3 d;
        call_print(&d);
    }
} // namespace virtual_method_ns
int main() {
    std::cout << "-------------------------" << "\n";
    is_trivial_ns::test();
    std::cout << "-------------------------" << "\n";
    is_standard_layout_ns::test();
    std::cout << "-------------------------" << "\n";
    virtual_inherit_ns::test();
    std::cout << "-------------------------" << "\n";
    virtual_inherit_ns2::test();
    std::cout << "-------------------------" << "\n";
    virtual_inherit_ns3::test();
    std::cout << "-------------------------" << "\n";
    virtual_method_ns::test();
    std::cout << "-------------------------" << "\n";
    virtual_method_ns::test2();
    std::cout << "-------------------------" << "\n";
    virtual_method_ns::test3();
}