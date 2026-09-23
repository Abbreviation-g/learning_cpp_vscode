// https://subingwen.cn/cpp/mutex/
// C++线程同步之互斥锁

// 解决多线程数据混乱的方案就是进行线程同步，最常用的就是互斥锁，在C++11中一共提供了四种互斥锁：
// std::mutex：独占的互斥锁，不能递归使用
// std::timed_mutex：带超时的独占互斥锁，不能递归使用
// std::recursive_mutex：递归互斥锁，不带超时功能
// std::recursive_timed_mutex：带超时的递归互斥锁

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace mutex_ns1 {
    using namespace std;
    int g_num = 0;
    mutex g_num_mutex;
    void slow_increment(int id) {
        g_num_mutex.lock();
        ++g_num;
        cout << "id: " << id << "=>" << g_num << endl;
        g_num_mutex.unlock();

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    void test() {
        thread t1(slow_increment, 1001);
        thread t2(slow_increment, 1002);
        t1.join();
        t2.join();
    }
} // namespace mutex_ns1
namespace lock_guard_ns {
    using namespace std;
    int g_num = 0;
    mutex g_num_mutex;
    void slow_increment(int id) {
        // 使用哨兵锁管理互斥锁
        lock_guard<mutex> lock(g_num_mutex);
        ++g_num;
        cout << "id: " << id << "=>" << g_num << endl;

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    void test() {
        thread t1(slow_increment, 1001);
        thread t2(slow_increment, 1002);
        t1.join();
        t2.join();
    }
} // namespace lock_guard_ns
namespace recursice_mutex_ns {
    // 递归互斥锁std::recursive_mutex允许同一线程多次获得互斥锁，可以用来解决同一线程需要多次获取互斥量时死锁的问题
    using namespace std;
    struct Calculate {
    public:
        Calculate() : m_i(6) {}
        void mul(const int &x) {
            lock_guard<mutex> locker(m_mutex);
            m_i *= x;
        }
        void div(const int &x) {
            lock_guard<mutex> locker(m_mutex);
            m_i /= x;
        }
        void both(const int &x, const int &y) {
            lock_guard<mutex> locker(m_mutex);
            mul(x);
            div(y);
        }

    private:
        int m_i;
        mutex m_mutex;
    };
    void test() {
        // 调用之后，程序就会发生死锁，在both()中已经对互斥锁加锁了，继续调用mult()函数，已经得到互斥锁所有权的线程再次获取这个互斥锁的所有权就会造成死锁（在C++中程序会异常退出，使用C库函数会导致这个互斥锁永远无法被解锁，最终阻塞所有的线程）。要解决这个死锁的问题，一个简单的办法就是使用递归互斥锁std::recursive_mutex，它允许一个线程多次获得互斥锁的所有权。
        Calculate cal;
        // cal.both(6, 3);
    }
} // namespace recursice_mutex_ns
namespace recursice_mutex_ns2 {
    // 递归互斥锁std::recursive_mutex允许同一线程多次获得互斥锁，可以用来解决同一线程需要多次获取互斥量时死锁的问题
    using namespace std;
    struct Calculate {
    public:
        Calculate() : m_i(6) {}
        void mul(const int &x) {
            lock_guard<decltype(m_mutex)> locker(m_mutex);
            m_i *= x;
        }
        void div(const int &x) {
            lock_guard<decltype(m_mutex)> locker(m_mutex);
            m_i /= x;
        }
        void both(const int &x, const int &y) {
            lock_guard<decltype(m_mutex)> locker(m_mutex);
            mul(x);
            div(y);
        }

    private:
        int m_i;
        recursive_mutex m_mutex;
    };
    void test() {
        Calculate cal;
        cal.both(6, 3);
    }
} // namespace recursice_mutex_ns2
namespace timed_mutex_ns {
    using namespace std;
    timed_mutex g_mutex;
    void work() {
        chrono::seconds timeout{ 1 };
        while (true) {
            // 通过阻塞一定时长来争取获得互斥锁所有权
            if (g_mutex.try_lock_for(timeout)) {
                cout << "当前线程id: " << this_thread::get_id() << ", 获得互斥锁所有权" << "\n";
                // 模拟处理任务使用一定时间
                this_thread::sleep_for(chrono::seconds{ 10 });
                // 互斥锁解锁
                g_mutex.unlock();
                cout << "当前线程id: " << this_thread::get_id() << ", 解锁" << "\n";
                break;
            } else {
                cout << "当前线程id: " << this_thread::get_id() << ", 没有获得互斥锁所有权" << "\n";
                // 模拟处理其它任务使用一定时间
                this_thread::sleep_for(chrono::milliseconds{ 500 });
            }
        }
    }
    void test() {
        thread t1(work);
        thread t2(work);
        t1.join();
        t2.join();
    }
} // namespace timed_mutex_ns
int main() {
    std::cout << "==================\n";
    mutex_ns1::test();
    std::cout << "==================\n";
    lock_guard_ns::test();
    std::cout << "==================\n";
    recursice_mutex_ns::test();
    std::cout << "==================\n";
    recursice_mutex_ns2::test();
    std::cout << "==================\n";
    timed_mutex_ns::test();
}