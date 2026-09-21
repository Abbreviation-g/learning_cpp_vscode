// 默认函数控制 =default 与 =delete
// https://subingwen.cn/cpp/default-delete/

#include <iostream>

// 1. 类与默认函数
// 在C++中声明自定义的类，编译器会默认帮助程序员生成一些他们未自定义的成员函数。
// 这样的函数版本被称为”默认函数”。这样的函数一共有六个，我们一起来看一下：

// 无参构造函数：创建类对象
// 拷贝构造函数：拷贝类对象
// 移动构造函数：拷贝类对象
// 拷贝赋值函数：类对象赋值
// 移动赋值函数：类对象赋值
// 析构函数：   销毁类对象
// 在C++语法规则中，一旦程序员实现了这些函数的自定义版本，则编译器不会再为该类自动生成默认版本。

// 有时程序员会忘记上面提到的规则，最常见的是声明了带参数的构造，
// 如果还需要无参构造函数，这时候必须定义出不带参数的版本。
// 不过通过编译器的提示，这样的问题通常会得到更正。
// 但更为严重的问题是，一旦声明了自定义版本的构造函数，
// 则有可能导致我们定义的类型不再是POD类型，我们便不再能够享受POD类型为我们带来的便利。

// 对于上面提到的这些，我们无需过度担心，
// 因为C++11非常贴心地为我们提供了解决方案，就是使用=default 。

// 2. =default 和 =delete
// 在C++11标准中称= default修饰的函数为显式默认【缺省】（explicit defaulted）函数，
// 而称=delete修饰的函数为删除（deleted）函数或者显示删除函数。

// C++11引入显式默认和显式删除是为了增强对类默认函数的控制，让程序员能够更加精细地控制默认版本的函数。

namespace default_ns1 {
    // 2.1 =default
    // 我们可以在类内部修饰满足条件的类函数为显示默认函数，
    // 也可以在类定义之外修饰成员函数为默认函数。下面举例说明：

    // 在类内部指定函数为默认函数
    // 一般情况下，我们可以在定义类的时候直接在类内部指定默认函数，
    class Base {
    public:
        Base() = default;
        Base(const Base &obj) = default;
        Base(Base &&obj) = default;
        Base &operator=(const Base &obj) = default;
        Base &operator=(Base &&obj) = default;
        ~Base() = default;
    };
} // namespace default_ns1
namespace default_ns2 {
    // 类定义
    class Base {
    public:
        Base();
        Base(const Base &obj);
        Base(Base &&obj);
        Base &operator=(const Base &obj);
        Base &operator=(Base &&obj);
        ~Base();

        // 不能使用 =default 修饰这六个函数以外的函数。
        // 以下写法全部都是错误的
        // Base(int a = 0) = default;
        // Base(int a, int b) = default;
        // void print() = default;
        // bool operator==(const Base &obj) = default;
        // bool operator>=(const Base &obj) = default;
    };
    // 在类定义之外指定成员函数为默认函数
    Base::Base() = default;
    Base::Base(const Base &obj) = default;
    Base::Base(Base &&obj) = default;
    Base &Base::operator=(const Base &obj) = default;
    Base &Base::operator=(Base &&obj) = default;
    Base::~Base() = default;
} // namespace default_ns2

// 2.2 =delete
// =delete 表示显示删除，`显式删除可以避免用户使用一些不应该使用的类的成员函数`，
// 使用这种方式可以有效的防止某些类型之间自动进行隐式类型转换产生的错误。
namespace delete_ns {
    class Base {
    public:
        Base() = default;
        explicit Base(const Base &) = delete;
        explicit Base(Base &&) = delete;
        Base &operator=(const Base &) = delete;
        Base &operator=(Base &&) = delete;
    };
    void test() {
        Base b{};
        Base b2{};
        Base b3{};

        // Base tmp{ b }; // Call to deleted constructor of 'Base'clang(ovl_deleted_init)
        // Base tmp2{ std::move(b2) }; // Call to deleted constructor of
        // 'Base'clang(ovl_deleted_init)

        Base tmp3{};
        // tmp3 = b3; // Candidate function has been explicitly deleted
        Base tmp4{};
        // tmp4 = std::move(b3); // Candidate function has been explicitly deleted
    }
} // namespace delete_ns

namespace delete_ns2 {
    using namespace std;
    // 禁止使用自定义函数
    class Base {
    public:
        explicit Base(int num) : m_num{ num } {}
        Base(char c) = delete; // 禁用带 char类型参数的构造函数，防止隐式类型转换（char转int)
        void set(int num) {
            m_num = num;
        }
        void set(char num) = delete; // 禁止使用带char类型的自定义函数，防止隐式类型转换（char转int)
        void print(char c) = delete; // 禁止使用带char类型的自定义函数，防止隐式类型转换（char转int)
        void print() {
            cout << "num: " << m_num << endl;
        }
        void print(int num) {
            cout << "num: " << num << endl;
        }

    private:
        int m_num;
    };
    void test(){
        Base b{97};
        b.set(10087);
        // b.set('x'); // error 对应的函数被禁用，因此无法给函数传递char类型参数
        // Base b1{'a'}; // error 对应的构造函数被禁用，因此无法使用该构造函数构造对象
        b.print();
        b.print(10086);
        // b.print('c'); // error 对应的打印函数被禁用，因此无法给函数传递char类型参数

        short i = 1;
        Base b2{i};
    }
} // namespace delete_ns2
int main() {
    delete_ns::test();
}