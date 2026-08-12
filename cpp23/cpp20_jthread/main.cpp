#include <format>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;
// https://blog.csdn.net/fengbingchun/article/details/152172174
// C++20中线程类std::jthread的使用
// C++20中的std::jthread类表示一个执行线程。它与std::thread具有相同的常规行为，但std::jthread在析构(destruction)时会自动重新加入(rejoin)，并且可以在某些情况下被取消/停止(cancelled/stopped)。

// 与std::thread不同，std::jthread在逻辑上持有一个std::stop_source类型的内部私有成员，该成员维护一个共享的停止状态(stop-state)。std::jthread构造函数接受一个以std::stop_token作为第一个参数的函数，该参数由std::jthread从其内部std::stop_source传入。这使得该构造函数能够检查在执行过程中是否已请求停止，如果已请求则返回。

// std::jthread对象也可能处于不代表任何线程的状态(after default construction, move from, detach, or join)，并且执行线程可能不与任何std::jthread 对象关联(after detach)。

// 任何两个std::jthread对象都不能代表同一个执行线程；std::jthread不可拷贝构造或拷贝赋值，但可移动构造和移动赋值。

// 如果std::jthead无法启动，则抛std::system_error异常。

// std::jthread构造函数的参数按值移动或拷贝。如果需要将引用参数传递给线程函数，则必须对其进行包装(例如，使用std::ref或std::cref)。

// std::jthread的析构函数：如果当前线程有一个关联线程即joinable()为true，则会先调用request_stop()，然后再调用join()：

// (1).如果std::jthread之前被请求stop，则调用request_stop()无效。

// (2).std::jthread对象在以下情况后没有关联线程：它是被默认构造的、它被移出(moved from)、已调用join()、已调用detach()。

// (3).如果join()抛出异常，则std::terminate()可能被调用。

// std::jthread中的函数：

// joinable()：如果std::jthread对象标识了一个活动的执行线程，则返回true，否则返回false。已完成代码执行但尚未joined的线程仍被视为活动的执行线程，因此是jonable的。

// get_id()：返回std::jthread::id的值(std::thread::id的类型别名)，用于标识与*this关联的线程。如果没有关联的线程，则返回默认构造的std::jthread::id。

// hardware_concurrency()：静态函数，返回支持的并发线程数。该值仅供参考。如果该值定义不明确或无法计算，则返回0。

// join()：阻塞当前线程，直到*this标识的线程完成执行。*this本身不执行同步。多个线程同时对同一个std::jthread对象调用join()会造成数据争用，从而导致未定义的行为。

// detach()：将执行线程与std::jthread对象分离，允许继续独立执行。线程退出后，所有分配的资源都将被释放。调用detach后，*this不再拥有任何线程。

// swap ：交换两个std::jthread对象的底层句柄。

int test_jthread_constructor()
{
    auto func1 = [](int val) {
        for (auto i = 0; i < 2; ++i) {
            std::cout << ++val << std::endl;
            std::this_thread::sleep_for(100ms);
        }
    };
 
    auto func2 = [](int& val) {
        val = 88;
    };
 
    std::jthread jth1(func1, 5);
    std::jthread jth2{};
    std::cout << std::format("jth1 joinable: {}; jth2 joinable: {}", jth1.joinable(), jth2.joinable()) << std::endl;
    std::cout << "jth1 id: " << jth1.get_id() << "; jth2  id: " << jth2.get_id() << std::endl;
 
    jth2 = std::move(jth1);
    std::cout << std::format("jth1 joinable: {}; jth2 joinable: {}", jth1.joinable(), jth2.joinable()) << std::endl;
    std::cout << "jth1 id: " << jth1.get_id() << "; jth2  id: " << jth2.get_id() << std::endl;
 
    int val{ -1 };
    std::jthread jth3(func2, std::ref(val));
    jth3.join();
    std::cout << std::format("jth3 joinable: {}, val: {}", jth3.joinable(), val) << std::endl;
    std::cout << "jth3 id: " << jth3.get_id() << std::endl;
 
    std::cout << "concurrent threads are supported: " << std::jthread::hardware_concurrency() << std::endl;
 
    std::jthread jth4([] {
        std::cout << "jth4 running ..." << std::endl;
        std::this_thread::sleep_for(1s);
        std::cout << "jth4 end" << std::endl;
    });
    std::cout << "jth4 joinable: " << jth4.joinable() << ", id: " << jth4.get_id() << std::endl;
 
    jth4.detach();
    std::this_thread::sleep_for(3s);
    std::cout << "jth4 joinable: " << jth4.joinable() << ", id: " << jth4.get_id() << std::endl;
 
    jth4.swap(jth2);
    std::cout << std::format("jth2 joinable: {}; jth4 joinable: {}", jth2.joinable(), jth4.joinable()) << std::endl;
 
    std::swap(jth2, jth4);
    std::cout << std::format("jth2 joinable: {}; jth4 joinable: {}", jth2.joinable(), jth4.joinable()) << std::endl;
 
    return 0;
}

int main(){
    test_jthread_constructor();
    return 0;
}