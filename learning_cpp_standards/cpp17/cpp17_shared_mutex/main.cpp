#include <array>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>
// https://subingwen.cn/cpp/shared_mutex/
// 多线程之读写锁
// 1. 读写分离
// 在多线程编程中，数据的并发访问通常分为两类场景：

// 写-写互斥：多个线程同时修改数据会导致冲突，必须串行化。
// 读-读并发：多个线程同时读取数据通常是安全的，不需要互斥。
// C++11 提供了 std::mutex，它是一个独占锁。即使在只读场景下，同一时间也只允许一个线程访问数据。这在读多写少的场景下会严重限制性能。

// C++14 引入了一种读写锁std::shared_timed_mutex，也被称为共享独占互斥量。它允许多个线程同时读共享数据，但写数据时必须独占访问。相比普通的
// std::mutex，它在读多写少的场景下能显著提高性能。此外，它支持超时机制，即可以设定等待锁的最长时间。

// C++17 引入了
// std::shared_mutex，这也是一种读写锁，主要目的是为了瘦身和高性能。大多数情况下，我们不需要超时功能（超时逻辑非常复杂且容易出错）。std::shared_mutex
// 去掉了时间相关的接口，实现上通常比 std::shared_timed_mutex 更轻量，占用内存更少，速度更快。

// 和std::shared_timed_mutex 一样，std::shared_mutex支持两种锁模式：

// 共享模式：用于只读操作。多个线程可以同时持有共享锁（即多个读者）。
// 独占模式：用于写操作。如果有一个线程持有独占锁（写者），其他线程无法获取共享锁或独占锁。
// std::shared_mutex 虽然提供了 lock() 和 unlock() 方法，但一般情况下我们是不去使用的。直接调用 lock/unlock 比较危险，容易忘记解锁导致死锁。通常我们使用
// std::unique_lock 管理写锁，使用 std::shared_lock 管理读锁。

// 独占锁（写锁） - std::unique_lock：用于修改数据。

// 方法	描述
// lock()	阻塞当前线程，直到获取独占锁。
// try_lock()	尝试获取独占锁。如果成功返回 true，失败返回 false（不阻塞）。
// unlock()	释放独占锁。
// 共享锁（读锁）- std::shared_lock：用于读取数据。

// 方法	描述
// lock_shared()	阻塞当前线程，直到获取共享锁。
// try_lock_shared()	尝试获取共享锁。如果成功返回 true，失败返回 false。
// unlock_shared()	释放共享锁。

namespace shared_mutex_ns {
    class ThreadSafeDictionary {
    public:
        // 写操作：使用 unique_lock（独占锁）
        void add_entry(const std::string &key, int value) {
            // std::unique_lock 在构造时自动调用 lock()
            std::unique_lock<std::shared_mutex> lock{ rw_mutex };
            // 析构时自动调用 unlock()
            data[key] = value;
            std::cout << "Added: " << key << std::endl;
        }
        // 读操作：使用 shared_lock（共享锁）
        bool get_value(const std::string &key, int &value) const {
            // std::shared_lock 在构造时自动调用 lock_shared()
            std::shared_lock<std::shared_mutex> lock{ rw_mutex };
            auto it = data.find(key);
            if (it != data.end()) {
                value = it->second;
                return true;
            }
            return false;
        }

    private:
        std::map<std::string, int> data;
        // 注意 shared_mutex 前面的 mutable。因为 get_value() 是 const 函数，但我们要在内部改变锁的状态（加锁/解锁），所以锁成员变量必须是 mutable 的。
        mutable std::shared_mutex rw_mutex;
    };

    void test() {
        ThreadSafeDictionary dict{};

        // 启动 3 个写线程
        constexpr int count3 = 3;
        std::array<std::thread, count3> writers;
        for (int i = 0; i < count3; i++) {
            writers[i] = std::thread{ [&dict, i]() {
                dict.add_entry("key" + std::to_string(i), i * 10);
            } };
        }
        // 启动 5 个读线程
        constexpr int count5 = 3;
        std::array<std::thread, count5> readers;
        for (int i = 0; i < count5; i++) {
            readers[i] = std::thread{ [&dict]() {
                int val{};
                // 注意：即使写线程还没跑，读线程也会并发执行
                for (int j = 0; j < 3; ++j) {
                    if (dict.get_value("key" + std::to_string(j), val)) {
                        std::cout << "Read key" << j << ": " << val << std::endl;
                    }
                }
            } };
        }

        for (auto &t : writers) {
            t.join();
        }
        for (auto &t : readers) {
            t.join();
        }
    }
} // namespace shared_mutex_ns

int main() {
    shared_mutex_ns::test();
}