// https://subingwen.cn/cpp/async/
// 多线程异步操作
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
namespace future_promise_ns {
    using namespace std;

    // 通过promise传递数据的过程一共分为5步：
    // 在主线程中创建std::promise对象
    // 将这个std::promise对象通过引用的方式传递给子线程的任务函数
    // 在子线程任务函数中给std::promise对象赋值
    // 在主线程中通过std::promise对象取出绑定的future实例对象
    // 通过得到的future对象取出子线程任务函数中返回的值。
    void test() {
        // 子线程任务函数执行期间，让状态就绪
        std::promise<int> pr;
        thread t1(
            [](promise<int> &p) {
                p.set_value(100);
                this_thread::sleep_for(chrono::seconds(3));
                cout << "执行完毕" << "\n";
            },
            ref(pr));

        future<int> f = pr.get_future();
        int value = f.get();
        cout << "value: " << value << "\n";

        t1.join();
        // value: 100
        // 执行完毕
        // 子线程的任务函数指定的是一个匿名函数，在这个匿名的任务函数执行期间通过p.set_value(100);
        // 传出了数据并且激活了状态，数据就绪后，外部主线程中的int value = f.get();
        // 解除阻塞，并将得到的数据打印出来，5秒钟之后子线程休眠结束，匿名的任务函数执行完毕。
    }
    // 子线程任务函数执行结束，让状态就绪
    void test2() {
        promise<int> pr;
        thread t1(
            [](promise<int> &p) {
                p.set_value_at_thread_exit(100);
                this_thread::sleep_for(chrono::seconds(3));
                cout << "执行完毕" << "\n";
            },
            std::ref(pr));
        future<int> f = pr.get_future();
        int value = f.get();
        cout << "value: " << value << "\n";
        t1.join();
        // 执行完毕
        // value: 100

        // 线程的这个匿名的任务函数中通过p.set_value_at_thread_exit(100);
        // 在执行完毕并退出之后才会传出数据并激活状态，数据就绪后，外部主线程中的int value =
        // f.get();
        // 解除阻塞，并将得到的数据打印出来，因此子线程在休眠5秒钟之后主线程中才能得到传出的数据。
    }
    // 在这两个实例程序中有一个知识点需要强调，
    // 在外部主线程中创建的promise对象必须要通过引用的方式传递到子线程的任务函数中，
    // 在实例化子线程对象的时候，如果任务函数的参数是引用类型，那么实参一定要放到std::ref()函数中，
    // 表示要传递这个实参的引用到任务函数中。
} // namespace future_promise_ns
namespace future_packaged_task_ns {
    using namespace std;
    void test() {
        packaged_task<int(int)> task{ [](int x) {
            return x += 100;
        } };
        thread t1(std::ref(task), 100);

        future<int> f = task.get_future();
        int value = f.get();
        cout << "value: " << value << "\n";
        t1.join();

        // 在上面的示例代码中，通过packaged_task类包装了一个匿名函数作为子线程的任务函数，
        // 最终的得到的这个任务对象需要通过引用的方式传递到子线程内部，
        // 这样才能在主线程的最后通过任务对象得到future对象，
        // 再通过这个future对象取出子线程通过返回值传递出的数据。
    }
} // namespace future_packaged_task_ns

namespace future_async_ns {
    using namespace std;
    // 当wait_until()和wait_for()函数返回之后，并不能确定子线程当前的状态，因此我们需要判断函数的返回值，这样就能知道子线程当前的状态了：
    // 常量	解释
    // future_status::deferred	子线程中的任务函仍未启动
    // future_status::ready	子线程中的任务已经执行完毕，结果已就绪
    // future_status::timeout	子线程中的任务正在执行中，指定等待时长已用完

    void test() {
        cout << "主线程ID: " << this_thread::get_id() << "\n";
        // 调用函数直接创建线程执行任务
        future<int> f = async(
            [](int x) {
                cout << "子线程ID: " << this_thread::get_id() << "\n";
                this_thread::sleep_for(chrono::seconds(8));
                return x += 100;
            },
            100);
        future_status status;
        do {
            status = f.wait_for(chrono::seconds(1));
            if (status == future_status::deferred) {
                cout << "子线程中的任务函仍未启动\n";
            } else if (status == future_status::ready) {
                cout << "子线程中的任务已经执行完毕，结果已就绪 " << "value: " << f.get() << "\n";
            } else if (status == future_status::timeout) {
                cout << "子线程中的任务正在执行中，指定等待时长已用完" << "\n";
            }
        } while (status != future_status::ready);
        // 主线程ID: 1
        // 子线程ID: 5
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务正在执行中，指定等待时长已用完
        // 子线程中的任务已经执行完毕，结果已就绪 value: 200

        // 调用async()函数时不指定策略就是直接创建线程并执行任务，
        // 示例代码的主线程中做了如下操作status = f.wait_for(chrono::seconds(1));
        // 其实直接调用f.get()就能得到子线程的返回值。
        // 这里为了给大家演示wait_for()的使用，所以写的复杂了些。
    }
    // 策略	说明
    // std::launch::async	调用async函数时创建新的线程执行任务函数
    // std::launch::deferred
    // 调用async函数时不执行任务函数，直到调用了future的get()或者wait()时才执行任务（这种方式不会创建新的线程）
    void test2() {
        cout << "主线程ID: " << this_thread::get_id() << "\n";
        future<int> f = async(
            launch::deferred,
            [](int x) {
                cout << "子线程ID: " << this_thread::get_id() << "\n";

                return x += 100;
            },
            100);
        this_thread::sleep_for(chrono::seconds(8));
        cout << "value: " << f.get() << "\n";
        // 主线程ID: 1
        // value: 子线程ID: 1
        // 200
        // 由于指定了launch::deferred
        // 策略，因此调用async()函数并不会创建新的线程执行任务，
        // 当使用future类对象调用了get()或者wait()方法后才开始执行任务（此处一定要注意调用wait_for()函数是不行的）。
        // 通过测试程序输出的结果可以看到，两次输出的线程ID是相同的，
        // 任务函数是在主线程中被延迟（主线程休眠了5秒）调用了。
    }
} // namespace future_async_ns
int main() {
    future_promise_ns::test();
    std::cout << "----------------------\n";
    future_promise_ns::test2();
    std::cout << "----------------------\n";
    future_packaged_task_ns::test();
    std::cout << "----------------------\n";
    future_async_ns::test();
    std::cout << "----------------------\n";
    future_async_ns::test2();
}
// 使用async()函数，是多线程操作中最简单的一种方式，不需要自己创建线程对象，并且可以得到子线程函数的返回值。
// 使用std::promise类，在子线程中可以传出返回值也可以传出其他数据，并且可选择在什么时机将数据从子线程中传递出来，使用起来更灵活。
// 使用std::packaged_task类，可以将子线程的任务函数进行包装，并且可以得到子线程的返回值。
