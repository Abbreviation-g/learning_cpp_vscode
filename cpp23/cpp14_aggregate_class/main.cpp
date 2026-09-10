#include <cstdio>
#include <iostream>
#include <format>
// https://subingwen.cn/cpp/aggregate-class
// 聚合类 -> 数据集合，讲的是成员初始化
// 如何分辨聚合类
// 在 C++ 中，聚合类 是一种特殊的类类型，它主要用于表示数据的集合，类似于 C 语言的结构体。聚合类的定义非常严格，在C++11
// 及以后标准中，判定一个类型是否为聚合类通常遵循以下规则：
// 1.无用户自定义的构造函数：不能有用户显式提供的构造函数。无拷贝构造，移动构造，析构函数（空析构函数也不行），拷贝运算符重载，移动运算符重载
// 2.非私有/保护的非静态数据成员：所有的非静态数据成员必须是 public 的。静态成员的访问权限没有要求
// 3.无虚函数：不能有虚函数。
// 4.无虚基类：继承必须是public，且积累必须是聚合类
// 5.所有的非静态数据成员，不能有类内的默认初始化器 =,{}, 比如int number{6}不行
//
// 满足上述条件的类、结构体或数组，被称为聚合类。
// 聚合类可以使用花括号初始化列表进行初始化。
//
// 聚合类的初始化
// 1.使用{}初始化类内部的public成员，直接给成员赋值(非聚合类也可以使用{}进行初始化，调用类的构造函数的方式)
// 2.初始化顺序必须与声明的顺序保持一致
// 3.可以只初始化前几个成员，剩余的部分成员进行默认初始化，只能跳过尾部成员，中间的成员无法跳过
// 4.支持嵌套初始化{{{}}}
//
// 为什么要有聚合类
// 1.为了兼容C语言, c结构体使用{}进行初始化是一个古老传统
// 2.零开销初始化：没有构造函数，内存布局确定,直接往内存中写数据
// 3.语法简洁
// 4.编译期安全: 避免未定义陷阱，大大减少了未初始化内存的读取问题
namespace aggregate_class_ns {
    struct Point {
        int x; // 声明和定义
        int y;

    private:
        static int count; // 声明
    };
    Point p{ 1, 2 };
    int Point::count = 9; // 静态变量的定义，如果没有定义，则静态变量count没有存储空间

    // 使用默认的构造，拷贝构造，移动构造等等等
    struct DefaultExample {
        int a;
        int b;
    };
    DefaultExample ddd{ 1, 2 };

    // 聚类类是可以有积累的，并且积累也是聚合类
    struct Base {
        Point p;
        int a;
    };
    struct Derived : public Base {
        int value;
    };
    void test_base() {
        auto print_derived = [](const Derived &der) {
            std::cout << "der.p.x: " << der.p.x << ",der.p.y: " << der.p.y << ", "; // der.p.x: 11,der.p.y: 22
            std::cout << "der.a: " << der.a << ",der.value: " << der.value << std::endl; // der.a: 0,der.value: 0
        };
        Derived der{ 11, 22 }; // c++17标准。允许子类这么初始化
        print_derived(der); // der.p.x: 11,der.p.y: 22 der.a: 0,der.value: 0

        print_derived(Derived{ 11, 22, 33, 44 }); // der.p.x: 11,der.p.y: 22 der.a: 33,der.value: 44

        print_derived(Derived{ { { 11, 22 }, 33 }, 44 }); // der.p.x: 11,der.p.y: 22 der.a: 33,der.value: 44
    }

    // 嵌套的聚合类的初始化
    struct Outer {
        Point p;
        Base b;
        char c;
    };
    Outer o{ { 3, 4 }, { { 5, 6 }, 5 }, 'a' };

    // 错误的聚合类示例
    // 1. 使用量用户自定义的构造函数，移动构造，拷贝构造
    struct Error1 {
        int x;
        Error1() {
        }
        Error1(int v) : x(v) {
        }
        Error1(const Error1 &e) : x(0) {
        }
        ~Error1() {
        }
    };
    Error1 e{ 1 }; // 调用了构造函数进行初始化

    // 2. 有私有成员
    struct Error2 {
    public:
        int x;

    private:
        int y;
    };
    // Error2 e2{1}; // 错误

    // 3. 不允许初始化数据成员
    struct Error3 {
        int x = 9; // c++11 不允许， c++14允许
        int y{ 9 }; // c++11 不允许， c++14允许
    };
    Error3 e3{ 11, 23 }; // c++14之后允许

    // 4.有虚函数
    struct Error4 {
        int x;
        virtual void foo() {
        }
    };
    // Error4 e4{1}; // 报错
    struct Error4_Dervied : public Error4 {
        void foo() {
        }
    };
    // Error4_Dervied ed4{1};

    // 5. 有虚基类
    struct Base5 {};
    struct Error5 : virtual Base5 {
        int x;
    };
    // Error5 e5{9}; // 错误

    // 6. 没有public继承
    struct Error6 : private Base5 {
        int x;
    };
    // Error6 e6{9}; // 错误

    // 7. 基类不是聚合类
    struct Error7 : public Error6 {
        int x;
    };
    // Error7 e7{1,2}; // 错误
} // namespace aggregate_class_ns

// C++14 对聚合类的一个重要改进是：允许在聚合类的非静态数据成员声明时直接提供默认成员初始化器。{}或者=进行初始化
namespace use_aggregate_class_ns {
    struct Test {
        int a;
        int b;
        int c;
        double d{ 0.0 };
        const char *s = "hello";
    };
    struct Student {
        int id = 1000;
        const char *name = "test";
        double score = 60.0;
    };

    void test() {
        Test t{ 1, 2, 3, 3.14, "xxxxxxx" };
        std::printf("t.a=%d t.b=%d t.c=%d t.d=%f t.s=%s\n", t.a, t.b, t.c, t.d, t.s); // t.a=1 t.b=2 t.c=3 t.d=3.140000 t.s=xxxxxxx

        Student s1;
        Student s2{ 1001 };
        Student s3{ 1002, "Bob" };
        Student s4{ 1003, "Alice", 99.9 };
        Student s5{};
        auto print_student = [](const Student &s) {
            std::cout << std::format("Student{{id={}, name={}, score={}}}", s.id, s.name, s.score) << std::endl;
        };
        print_student(s1); // Student{id=1000, name=test, score=60}
        print_student(s2); // Student{id=1001, name=test, score=60}
        print_student(s3); // Student{id=1002, name=Bob, score=60}
        print_student(s4); // Student{id=1003, name=Alice, score=99.9}
        print_student(s5); // Student{id=1000, name=test, score=60}

        // 初始化优先级: 聚合初始化>默认成员初始化 ，如果{}是空的相当于使用默认初始化器
    }
} // namespace use_aggregate_class_ns
int main() {
    aggregate_class_ns::test_base();
    use_aggregate_class_ns::test();
}
