// https://subingwen.cn/cpp/this_thread/
// 命名空间 - this_thread

#include <chrono>
#include <iostream>
#include <thread>
namespace get_id_ns {
    using namespace std;

    void func() {
        cout << "子线程: " << this_thread::get_id() << endl;
    }

    int main() {
        cout << "主线程: " << this_thread::get_id() << endl;
        thread t(func);
        t.join();
    }
} // namespace get_id_ns
namespace sleep_for_ns {
    using namespace std;
    void func() {
        for (int i = 0; i < 10; ++i) {
            this_thread::sleep_for(chrono::seconds(1));
            cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
        }
    }

    int main() {
        thread t(func);
        t.join();
    }
} // namespace sleep_for_ns
namespace sleep_until_ns {
    using namespace std;

    void func() {
        for (int i = 0; i < 10; ++i) {
            // 获取当前系统时间点
            chrono::system_clock::time_point now = chrono::system_clock::now();
            // 时间间隔为2s
            chrono::seconds sec{ 2 };
            chrono::system_clock::time_point next_time = now + sec;
            // 当前时间点之后休眠两秒
            this_thread::sleep_until(next_time);
            cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
        }
    }

    int main() {
        thread t(func);
        t.join();
    }
} // namespace sleep_until_ns
namespace yield_ns {
    using namespace std;

    void func() {
        for (int i = 0; i < 100000000000; ++i) {
            cout << "子线程: " << this_thread::get_id() << ", i = " << i << endl;
            this_thread::yield();
        }
    }

    int main() {
        thread t(func);
        thread t1(func);
        t.join();
        t1.join();
    }
} // namespace yield_ns
int main() {
    std::cout << "=================\n";
    get_id_ns::main();
    std::cout << "=================\n";
    sleep_for_ns::main();
    std::cout << "=================\n";
    sleep_until_ns::main();
    std::cout << "=================\n";
    yield_ns::main();
}