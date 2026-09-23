// https://subingwen.cn/cpp/call_once/
// call_once

// 在某些特定情况下，某些函数只能在多线程环境下调用一次，比如：要初始化某个对象，而这个对象只能被初始化一次，就可以使用std::call_once()来保证函数在多线程环境下只能被调用一次。使用call_once()的时候，需要一个once_flag作为call_once()的传入参数，该函数的原型如下：

// // 定义于头文件 <mutex>
// template< class Callable, class... Args >
// void call_once( std::once_flag& flag, Callable&& f, Args&&... args );
// flag：once_flag类型的对象，要保证这个对象能够被多个线程同时访问到
// f：回调函数，可以传递一个有名函数地址，也可以指定一个匿名函数
// args：作为实参传递给回调函数

#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>

namespace once_ns {
    using namespace std;
    once_flag g_flag;
    void do_once(int a, string b) {
        cout << "name: " << b << ", age: " << a << endl;
    }
    void do_something(int age, string name) {
        static int num = 1;
        call_once(g_flag, do_once, age, name);
        stringstream ss;
        ss << "do_something() function num = " << num++;
        cout << ss.str() << endl;
    }
    void test() {
        for (int i = 0; i < 20; i++) {
            thread t1{ do_something, i + 1, "ace" + std::to_string(i + 1) };
            t1.join();
        }
        thread t2{ do_something, 2, "bell" };
        thread t3{ do_something, 3, "carry" };

        t2.join();
        t3.join();

        // name: ace, age: 1
        // do_something() function num = 1
        // do_something() function num = 2
        // do_something() function num = 3

        // 虽然运行的三个线程中都执行了任务函数do_something()但是call_once()中指定的回调函数只被执行了一次
    }
} // namespace once_ns
int main() {
    once_ns::test();
}