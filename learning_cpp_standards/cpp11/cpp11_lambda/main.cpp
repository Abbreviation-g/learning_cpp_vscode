#include <functional>
#include <iostream>
// https://subingwen.cn/cpp/lambda/
//  Lambda表达式

// 1. 基本用法
// lambda表达式是C++11最重要也是最常用的特性之一，这是现代编程语言的一个特点，lambda表达式有如下的一些优点：

// 声明式的编程风格：就地匿名定义目标函数或函数对象，不需要额外写一个命名函数或函数对象。
// 简洁：避免了代码膨胀和功能分散，让开发更加高效。
// 在需要的时间和地点实现功能闭包，使程序更加灵活。
// lambda表达式定义了一个匿名函数，并且可以捕获一定范围内的变量。lambda表达式的语法形式简单归纳如下：

// [capture](params) opt -> ret {body;};
namespace lambda_ns1 {
    void func(int x, int y) {
        int a = 1000;
        int b;
        [=, &x, &y]() mutable {
            int c = a;
            int d = b;
            a = 1; //
        }();
        std::cout << a << "\n"; // 1000
    }

    //     2. 捕获列表
    // lambda表达式的捕获列表可以捕获一定范围内的变量，具体使用方式如下：

    // [] - 不捕捉任何变量
    // [&] - 捕获外部作用域中所有变量, 并作为引用在函数体内使用 (按引用捕获)
    // [=] - 捕获外部作用域中所有变量, 并作为副本在函数体内使用 (按值捕获)
    // 拷贝的副本在匿名函数体内部是只读的
    // [=, &foo] - 按值捕获外部作用域中所有变量, 并按照引用捕获外部变量 foo
    // [bar] - 按值捕获 bar 变量, 同时不捕获其他变量
    // [&bar] - 按引用捕获 bar 变量, 同时不捕获其他变量
    // [this] - 捕获当前类中的this指针
    // 让lambda表达式拥有和当前类成员函数同样的访问权限
    // 如果已经使用了 & 或者 =, 默认添加此选项
    class Test {
    public:
        void output(int x, int y) {
            // auto x1 = [] { return m_number; }; // error
            auto x11 = [] {
                return 0;
            };
            auto x3 = [&] {
                return m_number + x + y;
            };
            auto x4 = [this] {
                return m_number;
            };
            // auto x5 = [this]{return m_number+x+y;}; // error
            auto x6 = [this, x, y] {
                return m_number + x + y;
            };
            auto x7 = [this] {
                return ++m_number;
            };

            using func_ptr = int (*)(void);
            func_ptr f2 = x11;
            // func_ptr f3 = x3; // error
        }
        int m_number = 100;
    };
    void test2() {
        int a = 10, b = 20;
        // auto f1 = [] {return a; };                        // error
        auto f2 = [&] {
            return a++;
        }; // ok
        auto f3 = [=] {
            return a;
        }; // ok
        // auto f4 = [=] {return a++; };                     // error
        // auto f5 = [a] {return a + b; };                   // error
        auto f6 = [a, &b] {
            return a + (b++);
        }; // ok
        auto f7 = [=, &b] {
            return a + (b++);
        }; // ok
    }

    //     3. 返回值
    // 很多时候，lambda表达式的返回值是非常明显的，因此在C++11中允许省略lambda表达式的返回值。
    void test3() {
        // 完整的lambda表达式定义
        auto f = [](int a) -> int {
            return a + 10;
        };

        // 忽略返回值的lambda表达式定义
        auto f2 = [](int a) {
            return a + 10;
        };
        // error，不能推导出返回值类型
        // auto f1 = [](){
        //     return {1, 2};	// 基于列表初始化推导返回值，错误
        // }
    }

    // 4. 函数本质
    // 使用lambda表达式捕获列表捕获外部变量，如果希望去修改按值捕获的外部变量，那么应该如何处理呢？这就需要使用mutable选项，被mutable修改是lambda表达式就算没有参数也要写明参数列表，并且可以去掉按值捕获的外部变量的只读（const）属性。
    // 因为lambda表达式在C++中会被看做是一个仿函数，因此可以使用std::function和std::bind来存储和操作lambda表达式：
    // 最后再剖析一下为什么通过值拷贝的方式捕获的外部变量是只读的:
    // lambda表达式的类型在C++11中会被看做是一个带operator()的类，即仿函数。
    // 按照C++标准，lambda表达式的operator()默认是const的，一个const成员函数是无法修改成员变量值的。

    // 因为lambda表达式在C++中会被看做是一个仿函数，因此可以使用std::function和std::bind来存储和操作lambda表达式：
    void test4() {
        // 包装可调用函数
        std::function<int(int)> f1 = [](int a) {
            return a;
        };
        // 绑定可调用函数
        std::function<int(int)> f2 = std::bind(
            [=](int a) {
                return a;
            },
            std::placeholders::_1);
        auto f22 = std::bind(
            [=](int a) {
                return a;
            },
            std::placeholders::_1);
        // 函数调用
        std::cout << f1(100) << std::endl;
        std::cout << f2(200) << std::endl;

        // 对于没有捕获任何变量的lambda表达式，还可以转换成一个普通的函数指针：
        using func_ptr = int (*)(int);
        func_ptr f3 = [](int) {
            return 0;
        };
        f3(1111);
        // func_ptr f4 = [=](int) {return 0;}; // 错误
    }
} // namespace lambda_ns1
int main() {
    lambda_ns1::func(9, 11);
}