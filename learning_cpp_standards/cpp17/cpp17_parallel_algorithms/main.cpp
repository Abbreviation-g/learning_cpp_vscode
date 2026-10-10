#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

void test1() {
    std::vector<int> v(1'000'000);
    // 填充数据
    for (int i = 0; i < v.size(); i++) {
        v[i] = (std::rand() % v.size()) + 1;
    }

    {
        // --- 旧方式 (串行) ---
        // 单线程排序
        auto start = std::chrono::steady_clock::now();
        std::sort(v.begin(), v.end());
        auto end = std::chrono::steady_clock::now();
        auto elapsed_sec = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "std::sort execution time: " << elapsed_sec.count() << " microseconds\n";
    }

    {
        // --- 新方式 (并行) ---
        // 多线程排序，自动利用多核 CPU
        // 编译器会自动根据 CPU 核心数分配线程
        auto start = std::chrono::steady_clock::now();
        std::sort(std::execution::par, v.begin(), v.end());
        auto end = std::chrono::steady_clock::now();
        auto elapsed_sec = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "std::sort execution time: " << elapsed_sec.count() << " microseconds\n";
    }
}
void test2() {
    //     2.1 数据竞争
    // 在 std::execution::par 模式下，传递给算法的函数（如 Lambda 表达式）可能会被多个线程同时调用。
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    {
        int sum = 0;

        std::for_each(std::execution::par, v.begin(), v.end(), [&](int x) {
            sum += x;
        });
        std::cout << "Sum: " << sum << std::endl;
    }
    {
        std::atomic_int sum{ 0 };
        std::for_each(std::execution::par, v.begin(), v.end(), [&](int x) {
            sum += x;
        });
        std::cout << "Sum: " << sum << std::endl;
    }
}
void test3() {
    std::vector<int> v(100);
    std::iota(v.begin(), v.end(), 1);
    auto print_v = [](const auto &v) {
        for (const auto &i : v) {
            std::cout << i << ", ";
        }
        std::cout << "\n";
    };

    // --- 多线程并行执行 (性能提升) ---
    int sum_par = std::reduce(std::execution::par, v.begin(), v.end(), 0); // 0是初始值
    std::cout << "Parallel Sum: " << sum_par << std::endl; // 5050

    // 计算乘积
    std::vector<int> v1{ 1, 2, 3, 4, 5 };
    // 使用 std::multiplies 函数对象
    auto product = std::reduce(std::execution::par, v1.begin(), v1.end(), 1, std::multiplies<>());
    std::cout << "Product: " << product << std::endl; // 输出: 120

    // 比较长度，保留更长者
    std::vector<std::string> words = { "Apple", "Banana", "Pear", "Watermelon", "Kiwi" };
    auto longest_string = std::reduce(std::execution::par, words.begin(), words.end(),
        std::string{ "" }, [](const auto &a, const auto &b) {
            return a.size() > b.size() ? a : b;
        });
    std::cout << "Longest word: " << longest_string << std::endl;

    print_v(v);
}

// 1. Map-Reduce 模式（变换后归约）：这是最常用的形式。
// template<class ExecutionPolicy, class ForwardIt1, class ForwardIt2, class T, class BinaryOp1,
// class BinaryOp2> T transform_reduce(ExecutionPolicy&& policy,
//                    ForwardIt1 first1, ForwardIt1 last1,
//                    ForwardIt2 first2,
//                    T init,
//                    BinaryOp1 binary_op1,  // 归约操作
//                    BinaryOp2 binary_op2); // 变换操作
void test4() {
    std::vector<double> v1 = { 1.0, 2.0, 3.0, 4.0 };
    std::vector<double> v2 = { 5.0, 6.0, 7.0, 8.0 };

    // 使用 std::plus 归约，使用 std::multiplies 变换
    // 策略：par (并行)

    double dp = std::transform_reduce(std::execution::par,
        // 范围1 和 范围2的起点
        v1.begin(), v1.end(), v2.begin(),
        0.0, // 初始值
        std::plus<>(), // 归约：加法
        std::multiplies<>() // 变换：乘法
    );
    std::cout << "Dot Product: " << dp << std::endl;
    // 结果: 1*5 + 2*6 + 3*7 + 4*8 = 70
}

// 2. 单范围变换后归约：不需要第二个范围。
// template<class ExecutionPolicy, class UnaryOp, class T, class BinaryOp>
// T transform_reduce(ExecutionPolicy&& policy,
//                    ForwardIt first, ForwardIt last,
//                    T init,
//                    BinaryOp binary_op1, // 归约操作
//                    UnaryOp unary_op);   // 变换操作
// 应用场景：对容器中每个元素应用一元函数（如 x * x），然后累加。
// 计算数组元素的平方和用到的就是单范围变换后归约，示例代码如下：
void test5() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    // 目标：1*1 + 2*2 + 3*3 + ...
    auto sum_of_squares =
        std::transform_reduce(std::execution::par, v.begin(), v.end(), 0, // 初始值
            std::plus<>(), // 归约：累加
            [](int x) {
                return x * x;
            } // 变换：平方 (Lambda)
        );
    std::cout << "Sum of Squares: " << sum_of_squares << std::endl; // 55
}
int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}