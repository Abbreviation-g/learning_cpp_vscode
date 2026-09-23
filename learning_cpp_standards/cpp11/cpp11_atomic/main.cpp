// https://subingwen.cn/cpp/atomic/
// 原子变量
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <functional>
namespace mutex_ns1 {
    using namespace std;
    struct Counter {
    public:
        void increment() {
            for (int i = 0; i < 10; i++) {
                lock_guard<mutex> locker(m_mutex);
                m_value++;
                cout << "increment number: " << m_value << ",  thread id: " << this_thread::get_id()
                     << "\n";
                int ms = (std::rand() % 100) + 150;
                this_thread::sleep_for(chrono::milliseconds(ms));
            }
        }
        void decrement() {
            for (int i = 0; i < 10; i++) {
                lock_guard<mutex> locker(m_mutex);
                m_value--;
                cout << "decrement number: " << m_value << ",  thread id: " << this_thread::get_id()
                     << "\n";
                int ms = (std::rand() % 100) + 150;
                this_thread::sleep_for(chrono::milliseconds(ms));
            }
        }

    private:
        int m_value = 0;
        mutex m_mutex;
    };
    void test() {
        Counter c;
        auto increment = bind(&Counter::increment, &c);
        auto decrement = bind(&Counter::decrement, &c);

        thread t1(increment);
        thread t2(decrement);
        t1.join();
        t2.join();
    }
} // namespace mutex_ns1
namespace atomic_ns1 {
    using namespace std;
    struct Counter {
    public:
        void increment() {
            for (int i = 0; i < 10; i++) {
                m_value++;
                stringstream ss;
                ss << "increment number: " << m_value.load() << ",  thread id: " << this_thread::get_id()
                   << "\n";
                cout << ss.str();
                int ms = (std::rand() % 100) + 150;
                this_thread::sleep_for(chrono::milliseconds(ms));
            }
        }
        void decrement() {
            for (int i = 0; i < 10; i++) {
                m_value--;
                stringstream ss;
                ss << "decrement number: " << m_value << ",  thread id: " << this_thread::get_id()
                   << "\n";
                cout << ss.str();
                int ms = (std::rand() % 100) + 150;
                this_thread::sleep_for(chrono::milliseconds(ms));
            }
        }

    private:
        // std::atomic<int> m_value{0};
        std::atomic_int m_value{ 0 };
    };
    void test() {
        Counter c;
        auto increment = bind(&Counter::increment, &c);
        auto decrement = bind(&Counter::decrement, &c);

        thread t1(increment);
        thread t2(decrement);
        t1.join();
        t2.join();
    }
} // namespace atomic_ns1
int main() {
    std::cout << "------------------------\n";
    mutex_ns1::test();
    std::cout << "------------------------\n";
    atomic_ns1::test();
}

// 通过代码的对比可以看出，使用了原子变量之后，就不需要再定义互斥量了，在使用上更加简便，并且这两种方式都能保证在多线程操作过程中数据的正确性，不会出现数据的混乱。
// 原子类型atomic<T> 可以封装原始数据最终得到一个原子变量对象，操作原子对象能够得到和操作原始数据一样的效果，当然也可以通过store()和load()来读写原子对象内部的原始数据。
