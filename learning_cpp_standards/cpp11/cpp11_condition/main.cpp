// https://subingwen.cn/cpp/condition/
//  C++线程同步之条件变量
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

namespace sync_queue_ns {
    using namespace std;
    class SyncQueue {
    public:
        SyncQueue(int max_size) : m_max_size(max_size) {}

        void put(const int &x) {
            unique_lock<mutex> locker(m_mutex);
            // 判断任务队列是否已满
            while (m_queue.size() == m_max_size) {
                cout << "任务队列已满，请耐心等待" << "\n";
                // 阻塞线程
                m_notfull.wait(locker);
            }
            // 将任务放入任务队列
            m_queue.push_back(x);
            cout << x << "被生产" << "\n";
            // 通知消费者去消费
            m_notempty.notify_one();
        }
        int take() {
            unique_lock<mutex> locker(m_mutex);
            while (m_queue.empty()) {
                cout << "任务已满, 请耐心等待" << "\n";
                m_notempty.wait(locker);
            }
            // 从任务队列中取出任务(消费)
            int x = m_queue.front();
            m_queue.pop_front();
            // 通知生产之去生产
            m_notfull.notify_one();
            cout << x << "被消费" << "\n";
            return x;
        }
        bool empty() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.empty();
        }
        bool full() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size() == m_max_size;
        }
        size_t size() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size();
        }

    private:
        // 存储队列数据
        list<int> m_queue;
        // 互斥锁
        mutex m_mutex;
        // 不为空的条件变量
        condition_variable m_notempty;
        // 不为满的条件变量
        condition_variable m_notfull;
        // 任务队列的最大任务个数
        int m_max_size;
    };
    void test() {
        SyncQueue taskQ(50);
        auto produce = std::bind(&SyncQueue::put, &taskQ, placeholders::_1);
        auto consume = std::bind(&SyncQueue::take, &taskQ);

        const int thread_count = 4;
        thread t1[thread_count];
        thread t2[thread_count];
        for (int i = 0; i < thread_count; i++) {
            t1[i] = thread(produce, i + 1 + 1000);
            t2[i] = thread(consume);
        }
        for (int i = 0; i < thread_count; i++) {
            t1[i].join();
            t2[i].join();
        }
    }
} // namespace sync_queue_ns
namespace sync_queue_ns2 {
    // 条件变量condition_variable类的wait()还有一个重载的方法，
    // 可以接受一个条件，这个条件也可以是一个返回值为布尔类型的函数，
    // 条件变量会先检查判断这个条件是否满足，
    // 如果满足条件（布尔值为true），则当前线程重新获得互斥锁的所有权，结束阻塞，继续向下执行；
    // 如果不满足条件（布尔值为false），当前线程会释放互斥锁（解锁）同时被阻塞，等待被唤醒。

    using namespace std;
    class SyncQueue {
    public:
        SyncQueue(int max_size) : m_max_size(max_size) {}

        void put(const int &x) {
            unique_lock<mutex> locker(m_mutex);
            // 判断任务队列是否已满
            m_notfull.wait(locker, [this]() {
                return m_queue.size() != m_max_size;
            });
            // 将任务放入任务队列
            m_queue.push_back(x);
            cout << x << "被生产" << "\n";
            // 通知消费者去消费
            m_notempty.notify_one();
        }
        int take() {
            unique_lock<mutex> locker(m_mutex);
            m_notempty.wait(locker, [this]() {
                return !m_queue.empty();
            });
            // 从任务队列中取出任务(消费)
            int x = m_queue.front();
            m_queue.pop_front();
            // 通知生产之去生产
            m_notfull.notify_one();
            cout << x << "被消费" << "\n";
            return x;
        }
        bool empty() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.empty();
        }
        bool full() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size() == m_max_size;
        }
        size_t size() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size();
        }

    private:
        // 存储队列数据
        list<int> m_queue;
        // 互斥锁
        mutex m_mutex;
        // 不为空的条件变量
        condition_variable m_notempty;
        // 不为满的条件变量
        condition_variable m_notfull;
        // 任务队列的最大任务个数
        int m_max_size;
    };
    void test() {
        SyncQueue taskQ(50);
        auto produce = std::bind(&SyncQueue::put, &taskQ, placeholders::_1);
        auto consume = std::bind(&SyncQueue::take, &taskQ);

        const int thread_count = 4;
        thread t1[thread_count];
        thread t2[thread_count];
        for (int i = 0; i < thread_count; i++) {
            t1[i] = thread(produce, i + 1 + 1000);
            t2[i] = thread(consume);
        }
        for (int i = 0; i < thread_count; i++) {
            t1[i].join();
            t2[i].join();
        }
    }
} // namespace sync_queue_ns2
namespace sync_queue_ns3 {
    using namespace std;
    class SyncQueue {
    public:
        SyncQueue(int max_size) : m_max_size(max_size) {}

        void put(const int &x) {
            lock_guard<mutex> locker(m_mutex);
            // 判断任务队列是否已满
            m_notfull.wait(m_mutex, [this]() {
                return m_queue.size() != m_max_size;
            });
            // 将任务放入任务队列
            m_queue.push_back(x);
            cout << x << "被生产" << "\n";
            // 通知消费者去消费
            m_notempty.notify_one();
        }
        int take() {
            lock_guard<mutex> locker(m_mutex);
            m_notempty.wait(m_mutex, [this]() {
                return !m_queue.empty();
            });
            // 从任务队列中取出任务(消费)
            int x = m_queue.front();
            m_queue.pop_front();
            // 通知生产之去生产
            m_notfull.notify_one();
            cout << x << "被消费" << "\n";
            return x;
        }
        bool empty() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.empty();
        }
        bool full() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size() == m_max_size;
        }
        size_t size() {
            lock_guard<mutex> locker(m_mutex);
            return m_queue.size();
        }

    private:
        // 存储队列数据
        list<int> m_queue;
        // 互斥锁
        mutex m_mutex;
        // 不为空的条件变量
        condition_variable_any m_notempty;
        // 不为满的条件变量
        condition_variable_any m_notfull;
        // 任务队列的最大任务个数
        int m_max_size;
    };
    void test() {
        SyncQueue taskQ(50);
        auto produce = std::bind(&SyncQueue::put, &taskQ, placeholders::_1);
        auto consume = std::bind(&SyncQueue::take, &taskQ);

        const int thread_count = 4;
        thread t1[thread_count];
        thread t2[thread_count];
        for (int i = 0; i < thread_count; i++) {
            t1[i] = thread(produce, i + 1 + 1000);
            t2[i] = thread(consume);
        }
        for (int i = 0; i < thread_count; i++) {
            t1[i].join();
            t2[i].join();
        }
    }
} // namespace sync_queue_ns3
int main() {
    std::cout << "----------------\n";
    sync_queue_ns::test();
    std::cout << "----------------\n";
    sync_queue_ns2::test();
    std::cout << "----------------\n";
    sync_queue_ns3::test();
}

// condition_variable 配合 unique_lock 使用更灵活一些，可以在在任何时候自由地释放互斥锁，
// 而condition_variable_any 如果和lock_guard 一起使用必须要等到其生命周期结束才能将互斥锁释放。
// 但是，condition_variable_any 可以和多种互斥锁配合使用，应用场景也更广，
// 而 condition_variable 只能和独占的非递归互斥锁（mutex）配合使用，有一定的局限性。
