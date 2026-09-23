// https://subingwen.cn/cpp/thread/
// C++线程的使用

#include <functional>
#include <iostream>
#include <thread>
namespace thread_ns1 {
    using namespace std;
    void func(int num, string str) {
        for (int i = 0; i < 10; ++i) {
            cout << "子线程: i = " << i << "num: " << num << ", str: " << str << endl;
        }
    }

    void func1() {
        for (int i = 0; i < 10; ++i) {
            cout << "子线程: i = " << i << endl;
        }
    }
    void test() {
        cout << "主线程的线程ID: " << this_thread::get_id() << endl;
        auto f1 = std::bind(func, std::placeholders::_1, std::placeholders::_2);
        thread t1(f1, 10086, "cmcc");
        auto f2 = std::bind(func1);
        thread t2(f2);

        t1.join();
        t2.join();

        cout << "线程t 的线程ID: " << t1.get_id() << endl;
        cout << "线程t1的线程ID: " << t1.get_id() << endl;
    }
    void test2() {
        cout << "主线程的线程ID: " << this_thread::get_id() << endl;
        thread t(func, 520, "i love you");
        thread t1(func1);
        cout << "线程t 的线程ID: " << t.get_id() << endl;
        cout << "线程t1的线程ID: " << t1.get_id() << endl;
        t.detach();
        t1.detach();
        // 让主线程休眠, 等待子线程执行完毕
        this_thread::sleep_for(chrono::seconds(5));
    }
} // namespace thread_ns1
namespace joinable_ns {
    // joinable()函数用于判断主线程和子线程是否处理关联（连接）状态，一般情况下，二者之间的关系处于关联状态，该函数返回一个布尔类型：
    // 返回值为true：主线程和子线程之间有关联（连接）关系
    // 返回值为false：主线程和子线程之间没有关联（连接）关系

    using namespace std;
    void foo() {
        this_thread::sleep_for(std::chrono::seconds(1));
    }

    void main() {
        thread t;
        cout << "before starting, joinable: " << t.joinable() << endl; // 0
        // 在创建的子线程对象的时候，如果没有指定任务函数，那么子线程不会启动，主线程和这个子线程也不会进行连接

        t = thread(foo);
        cout << "after starting, joinable: " << t.joinable() << endl; // 1
        // 在创建的子线程对象的时候，如果指定了任务函数，子线程启动并执行任务，主线程和这个子线程自动连接成功

        t.join();
        cout << "after joining, joinable: " << t.joinable() << endl; // 0
        // 在子线程调用了join()函数，子线程中的任务函数继续执行，直到任务处理完毕，这时join()会清理（回收）当前子线程的相关资源，所以这个子线程和主线程的连接也就断开了，因此，调用join()之后再调用joinable()会返回false。

        thread t1(foo);
        cout << "after starting, joinable: " << t1.joinable() << endl; // 1
        t1.detach();
        cout << "after detaching, joinable: " << t1.joinable() << endl; // 0
        // 子线程调用了detach()函数之后，父子线程分离，同时二者的连接断开，调用joinable()返回false
    }
} // namespace joinable_ns
int main() {
    thread_ns1::test2();
    std::cout << "---------------\n";
    joinable_ns::main();

    std::cout << "---------------\n";
    int num = std::thread::hardware_concurrency();
    std::cout << "CPU number: " << num << "\n"; // 8
}