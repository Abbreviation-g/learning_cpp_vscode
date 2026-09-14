// https://subingwen.cn/cpp/bind/
// 可调用对象包装器、绑定器
#include <cstdio>
#include <iostream>
#include <functional>
#include <string>

namespace callable_ns1 {
    using namespace std;
    // 在C++中存在“可调用对象”这么一个概念。准确来说，可调用对象有如下几种定义：
    // 1.是一个函数指针
    int print(int a, double b) {
        cout << a << b << endl;
        return 0;
    }
    void test1() {
        // 定义函数指针
        typedef int (*func_ptr)(int, double);
        using func_ptr2 = int (*)(int, double);

        func_ptr f1 = print;
        func_ptr2 f2 = print;
        int (*f3)(int, double) = print;

        f1(1, 2.1);
        f2(1, 2.1);
        f3(1, 2.1);
    }

    // 2.是一个具有operator()成员函数的类对象（仿函数）
    struct Test2 {
        // ()操作符重载
        void operator()(string msg) {
            cout << "msg: " << msg << endl;
        }
    };
    void test2() {
        Test2 t;
        t("sss");
    }

    // 3.是一个可被转换为函数指针的类对象
    using func_ptr3 = void (*)(int, string);
    struct Test3 {
        static void print(int a, string b) {
            cout << "name: " << b << ", age: " << a << endl;
        }
        void print2(int a, string b) {
            cout << "name: " << b << ", age: " << a << endl;
        }

        // 将类对象转换为函数指针
        operator func_ptr3() {
            return print;
            // 错误// return print2; // Reference to non-static member function must be calledclang(bound_member_function)
        }
        // 仿函数
        void operator()() {
            std::cout << "仿函数" << std::endl;
        }
    };
    void test3() {
        Test3 t;
        t(1, "xxx"); // 将类对象转换为函数指针
        t(); // 仿函数
    }
    // 4.是一个类成员函数指针或者类成员指针
    struct Test4 {
        void print(int a, string b) {
            cout << "name: " << b << ", age: " << a << endl;
        }
        static void world(int a, string b) {
            cout << "world: name: " << b << ", age: " << a << endl;
        }
        int m_num;
    };
    void test4() {
        Test4 t;
        // 定义类成员函数指针指向类成员函数
        void (Test4::*func_ptr)(int, string) = &Test4::print;
        // 通过类成员函数指针调用类成员函数
        (t.*func_ptr)(19, "Monkey D. Luffy");
        // 类成员指针指向类成员变量
        int Test4::*obj_ptr = &Test4::m_num;
        // 通过类成员指针初始化类成员变量
        t.*obj_ptr = 1;
        cout << "number is: " << t.m_num << endl;

        // 类的静态函数指针
        using func_ptr2 = void (*)(int, string);
        func_ptr2 f2 = &Test4::world;
        f2(2, "222");

        // 类的成员函数指针
        using func_ptr3 = void (Test4::*)(int, string);
        func_ptr3 f3 = &Test4::print;
        Test4 t3;
        (t3.*f3)(1, "xxx");

        // 类的成员变量指针
        using ptr2 = int Test4::*;
        ptr2 p2 = &Test4::m_num;
        (t.*p2) = 1000;
    }
} // namespace callable_ns1

// std::function是可调用对象的包装器。它是一个类模板，
// 可以容纳除了类(非静态)成员（函数）指针之外的所有可调用对象。
// 通过指定它的模板参数，它可以用统一的方式处理函数、函数对象、函数指针，
// 并允许保存和延迟执行它们。
namespace function_ns {
    using namespace std;
    void print(int i, std::string s) {
        std::cout << "int: " << i << ", string: " << s << std::endl;
    }
    using funcptr = void (*)(int, string);
    using funcptr2 = void (*)(string);
    struct Test {
        // 仿函数
        void operator()(int i, string s) {
            std::cout << "Test::operator->" << "int: " << i << ", string: " << s << std::endl;
        }
        // 将类对象转换为函数指针
        operator funcptr() {
            return world;
        }
        operator funcptr2() {
            return world2;
        }
        void hello(int i, string s) {
            std::cout << "Test::hello->" << "int: " << i << ", string: " << s << std::endl;
        }
        static void world(int i, std::string s) {
            std::cout << "Test::world->" << "int: " << i << ", string: " << s << std::endl;
        }
        static void world2(string s) {
            std::cout << "Test::world2->" << "string: " << s << std::endl;
        }
        int m_id = 520;
        string m_name = "luffy";
    };
    // 2.1 基本用法
    // std::function必须要包含一个叫做functional的头文件，可调用对象包装器使用语法如下:
    // #include <functional>
    // std::function<返回值类型(参数类型列表)> diy_name = 可调用对象;
    void test1() {
        // 绑定一个普通函数
        function<void(int, string)> f1 = print;
        f1(1, "11"); // int: 1, string: 11
        // 绑定一个静态类成员函数
        function<void(int, string)> f2 = &Test::world;
        f2(2, "22"); // Test::world->int: 2, string: 22
        // 绑定一个仿函数
        Test ta;
        function<void(int, string)> f3{ ta };
        f3(3, "33"); // Test::operator->int: 3, string: 33
        // 包装转换为函数指针的对象(由于仿函数与类对象函数指针，返回值相同，参数列表相同，所以必须使用static_cast<funcptr>)
        std::function<void(int, string)> f4 = static_cast<funcptr>(ta);
        f4(4, "44"); // Test::world->int: 4, string: 44
        // 包装转换为函数指针的对象
        std::function<void(string)> f5 = ta;
        f5("55"); // Test::world2->string: 55
    }
    // 2.2 作为回调函数使用
    // 因为回调函数本身就是通过函数指针实现的，使用对象包装器可以取代函数指针的作用，来看一下下面的例子：
    class A {
    public:
        // 构造函数参数是一个包装器对象
        A(const function<void(int, string)> &f) : callback(f) {
        }
        void notify(int id, string msg) {
            callback(id, msg); // 调用通过构造函数得到的函数指针
        }

    private:
        function<void(int, string)> callback;
    };
    class A2 {
    public:
        // 构造函数参数是一个包装器对象
        A2(const function<void(string)> &f) : callback2(f) {
        }
        void notify2(string msg) {
            callback2(msg); // 调用通过构造函数得到的函数指针
        }

    private:
        function<void(string)> callback2;
    };
    void test2() {
        // 绑定一个普通函数
        A a(print);
        a.notify(1, "aa"); // int: 1, string: aa
        // 绑定一个静态类成员函数
        A b(&Test::world);
        b.notify(2, "bb"); // Test::world->int: 2, string: bb
        // 绑定一个仿函数
        A c(Test{});
        c.notify(3, "cc"); // Test::operator->int: 3, string: cc
        // 绑定函数指针的对象
        A d(static_cast<funcptr>(Test{}));
        d.notify(4, "44"); // Test::world->int: 4, string: 44
        // 绑定函数指针的对象
        A2 e(Test{});
        e.notify2("55"); // Test::world2->string: 55
    }
} // namespace function_ns
namespace bind_ns {
    // 3. 绑定器
    // std::bind用来将可调用对象与其参数一起进行绑定。
    // 绑定后的结果可以使用std::function进行保存，并延迟调用到任何我们需要的时候。
    // 通俗来讲，它主要有两大作用：
    // 1.将可调用对象与其参数一起绑定成一个仿函数。
    // 2.将多元（参数个数为n，n>1）可调用对象转换为一元或者（n-1）元可调用对象，即只绑定部分参数。

    // 绑定器函数使用语法格式如下：
    // // 绑定非类成员函数/变量
    // auto f = std::bind(可调用对象地址, 绑定的参数/占位符);
    // // 绑定类成员函/变量
    // auto f = std::bind(类函数/成员地址, 类实例对象地址, 绑定的参数/占位符);
    using namespace std;
    void call_func(int x, int y, const function<void(int, int)> &f) {
        if (x % 2 == 0) {
            f(x, y);
        }
    }
    void output(int x) {
        cout << x << "\n";
    }
    void output_add(int x, int y) {
        std::printf("x=%d, y=%d, x+y=%d \n", x, y, x + y);
    }
    // 绑定非类成员函数/变量
    void test1() {
        for (int i = 0; i < 10; i++) {
            auto f = std::bind(output_add, i + 100, i + 200);
            call_func(i, i, f);
            function<void(int, int)> f2 = std::bind(output_add, std::placeholders::_1, std::placeholders::_2);
            call_func(i, i, f2);
            // call_func(i, i, output_add);
        }
        // x=100, y=200, x+y=300
        // x=0, y=0, x+y=0
        // x=102, y=202, x+y=304
        // x=2, y=2, x+y=4
        // x=104, y=204, x+y=308
        // x=4, y=4, x+y=8
        // x=106, y=206, x+y=312
        // x=6, y=6, x+y=12
        // x=108, y=208, x+y=316
        // x=8, y=8, x+y=16
    }
    // 占位符
    // std::bind绑定器返回的是一个仿函数类型，得到的返回值可以直接赋值给一个std::function，
    // 在使用的时候我们并不需要关心绑定器的返回值类型，使用auto进行自动类型推导就可以了。
    // placeholders::_1是一个占位符，代表这个位置将在函数调用时被传入的第一个参数所替代。
    // 同样还有其他的占位符placeholders::_2、placeholders::_3、placeholders::_4、placeholders::_5等……
    void output2(int x, int y) {
        cout << x << ", " << y << endl;
    }
    void test2() {
        bind(output2, 1, 2)(100, 200); // 1, 2
        bind(output2, 1, 2)(); // 1, 2
        bind(output2, placeholders::_1, 2)(1); // 1, 2
        bind(output2, 1, placeholders::_1)(2); // 1, 2

        // bind(output2, 1, placeholders::_2)(2); // error
        bind(output2, 1, placeholders::_2)(1, 2); // 1, 2
        bind(output2, placeholders::_1, placeholders::_2)(1, 2); // 1, 2
        bind(output2, placeholders::_2, placeholders::_1)(1, 2); // 2, 1
    }

    // 绑定类成员函/变量
    // 可调用对象包装器std::function是不能实现对类成员函数指针或者类成员指针的包装的，但是通过绑定器std::bind的配合之后，就可以完美的解决这个问题了，再来看一个例子，然后再解释里边的细节：
    class Test {
    public:
        void output(int x, int y) {
            std::printf("x=%d, y=%d, x+y=%d \n", x, y, x + y);
        }
        int m_number = 100;
    };
    void test3() {
        // 成员函数绑定
        Test t{};
        std::bind(&Test::output, &t, 1, 2)(); // x=1, y=2, x+y=3
        std::bind(&Test::output, &t, std::placeholders::_1, std::placeholders::_2)(1, 2); // x=1, y=2, x+y=3
        std::bind(&Test::output, &t, std::placeholders::_1, 2)(1); // x=1, y=2, x+y=3
        auto f2 = std::bind(&Test::output, &t, 100, std::placeholders::_1);
        f2(50); // x=100, y=50, x+y=150
        std::function<void(int)> f22 = f2;
        f22(120); // x=100, y=120, x+y=220
        std::function<void(int)> f222 = std::bind(&Test::output, &t, 100, std::placeholders::_1);
        f222(130); // x=100, y=130, x+y=230 

        // 成员变量绑定
        auto f3 = std::bind(&Test::m_number, t);
        std::cout << "std::bind(&Test::m_number, t); " << f3() << "\n"; // std::bind(&Test::m_number, t); 100
        f3() = 666;
        std::cout << "std::bind(&Test::m_number, t); " << f3() << "\n"; // std::bind(&Test::m_number, t); 666
        std::function<int &()> f33 = f3;
        f33() = 999;
        std::cout << "std::function<int&()> f33: " << f33() << "\n"; // std::function<int&()> f33: 999
    }
} // namespace bind_ns
int main() {
    callable_ns1::test4();
    function_ns::test1();
    function_ns::test2();
    bind_ns::test1();
    bind_ns::test2();
    bind_ns::test3();
}