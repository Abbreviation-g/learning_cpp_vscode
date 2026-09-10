#include <chrono>
#include <cstdlib>
#include <format>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
// https://www.bilibili.com/video/BV1HXMh62EE9/
// https://subingwen.cn/cpp/shared_lock/
// 多线程之读写锁

// 读写锁->线程同步 std::shared_timed_mutex // 带超时功能的共享互斥量
// 物理层面(内存里面): 一个对象，一把锁
// 逻辑层面(使用时): 3中状态：无锁,共享锁(读锁),独占锁(写锁)
//
// 读写锁的使用机制, 读共享，写独占
// 1.共享模式: 线程的读操作 ，一个线程获取了读锁，其它线程也能够同时获取读锁读取数据，其它写入数据的线程需要先阻塞，知道读写锁被释放处于无锁状态解除阻塞
// 2.独占模式: 写数据的线程独占锁资源，一个线程独占，其它读写线程阻塞，知道写数据的线程释放了写入锁

// 读写锁的使用: 需要用到三个类
// 1. std::shared_timed_mutex: 读写锁类
// 2. std::shared_lock: 读锁管理器(读模式下的加锁解锁)
// 3. std::unique_lock: 写锁管理器(写模式下的加锁解锁)
namespace shared_timed_mutex_ns {
    // 注意: 本机 libstdc++ 的 std::format 并未为 std::thread::id 提供
    // formatter, 直接 std::format("{}", std::this_thread::get_id()) 会报
    // "std::formatter must be specialized for each type being formatted"。
    // 因此这里用 ostringstream 把 thread::id 转成字符串再拼接。
    std::string thread_id_str() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }

    // 模拟一个简单的数据缓存，多个读线程，一个写线程
    // 定义共享数据类， 读/写
    class SharedData {
    public:
        SharedData(const std::string &data) : m_data(data) {
        }
        // 读数据
        std::string read() const {
            std::shared_lock<std::shared_timed_mutex> locker{ m_rw_mutex };
            std::cout << "[read: " << thread_id_str() << " 线程] 正在读数据" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            return m_data;
        }
        void write(const std::string &data) {
            std::unique_lock<std::shared_timed_mutex> locker{ m_rw_mutex };

            std::cout << "[write: " << thread_id_str() << " 线程] 正在写数据" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            m_data = data;

            std::cout << "[write: " << thread_id_str() << " 线程] ，数据更新完毕, 新数据:" << m_data << std::endl;
        }
        std::string try_read_for(int duration_ms) const {
            // 创建时不加锁
            std::shared_lock<std::shared_timed_mutex> locker{ m_rw_mutex, std::defer_lock };
            if (locker.try_lock_for(std::chrono::milliseconds(duration_ms))) {
                std::cout << "[try_read_for: " << thread_id_str() << " 线程] 正在读数据" << std::endl;
                return m_data;
            } else {
                std::cout << "[try_read_for: " << thread_id_str() << " 线程] 读取失败，等待超时" << std::endl;
                return std::string{};
            }
        }

    private:
        mutable std::shared_timed_mutex m_rw_mutex;
        std::string m_data;
    };

    void test() {
        SharedData data("初始化数据");

        // 创建三个读取线程
        std::vector<std::thread> read_threds;
        for (size_t i = 0; i < 3; i++) {
            read_threds.emplace_back([&data]() {
                for (int i = 0; i < 5; i++) {
                    data.read();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 + (std::rand() % 1000)));
                }
            });
        }
        // 写线程
        std::thread write_thread{ [&data]() {
            for (int i = 0; i < 5; i++) {
                data.write("这是第" + std::to_string(i + 2) + "条数据");
                std::this_thread::sleep_for(std::chrono::milliseconds(100 + (std::rand() % 100)));
            }
        } };
        // 读取超时线程
        std::thread timeout_read_thread{ [&data]() {
            for (int i = 0; i < 3; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                data.try_read_for(300);
            }
        } };

        // 主线程不能退出，否则子线程全部终止
        // 通过各个子线程的join方法
        write_thread.join();
        timeout_read_thread.join();
        for (auto &t : read_threds) {
            t.join();
        }

        std::cout << "最终的数据: " << data.read() << std::endl;
    }
} // namespace shared_timed_mutex_ns
int main() {
    shared_timed_mutex_ns::test();
}