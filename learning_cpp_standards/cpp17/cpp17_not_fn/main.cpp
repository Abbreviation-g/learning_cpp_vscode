#include <algorithm>
#include <functional>
#include <ios>
#include <iostream>
#include <utility>
#include <vector>
// https://subingwen.cn/cpp/not_fn/
// 通用否定器 - std::not_fn
// C++17 彻底解决了这个问题，推出了 std::not_fn。

// 它是谁：一个函数适配器。
// 它能干什么：接受任何可调用对象，返回一个新的可调用对象。这个新对象的逻辑是：调用原对象，并对返回值进行逻辑非（!）操作。
// 通用性：无论是一元、二元还是 N 元谓词；无论是函数指针、Lambda 还是成员函数，统统兼容

// 2. std::not_fn 应用
// 2.1 处理 Lambda
// 处理 Lambda
// 这是最常用的场景。在算法中过滤数据时，写非条件是很常见的。示例代码需求：删除容器中所有大于等于 20
// 的元素。
namespace ns1 {
    void test1() {
        std::vector<int> nums{ 1, 2, 20, 21, 22, 3, 23, 4, 24, 5 };

        auto print_v = [](const auto &items) {
            for (const auto &item : items) {
                std::cout << item << ", ";
            }
            std::cout << "\n";
        };
        // 定义一个简单的判断条件
        auto is_less_than_20 = [](int x) {
            return x < 20;
        };

        // 使用 std::not_fn 包装它，变为 "大于等于 20"
        auto new_end = std::remove_if(nums.begin(), nums.end(), std::not_fn(is_less_than_20));
        print_v(nums);
        nums.erase(new_end, nums.end());
        // 输出: 5 10 15 1 2
        print_v(nums);

        // std::remove_if是一个不稳定的重排操作。它把保留的元素挤在一起放在容器的头部，剩下的尾部元素是未定义的（可能是原来的值，也可能是移动后的残留）。
        // std::remove_if 并不会真正删除容器中的元素，也不会改变容器的大小。真正的删除需要两步走：
        // 调用 std::remove_if 把不要的元素挤到后面，获取新的结尾迭代器。
        // 调用容器的 erase 方法，从那个新结尾一直删到 end()。

        // 因为 std::remove_if 不会缩短 vector
        // 的长度，它只把符合条件的元素移到末尾区域，并返回新的逻辑结尾 new_end。
        // 你的条件是 std::not_fn(is_less_than_20)，也就是 x >= 20。
        // remove_if 保留小于 20的元素，并按原顺序前移：
        // 原数据：       1, 2, 20, 21, 22, 3, 23, 4, 24, 5
        // 前移保留元素后：1, 2,  3,  4,  5, 3, 23, 4, 24, 5
        //                                 ↑ new_end
        // 所以第一次打印发生在 erase 之前：vector 仍有 10个元素，后半部分是残留区域中的元素。
        // 这个区域的值是未指定的，不能依赖当前看到的内容。随后：
        // 才真正删除尾部元素，第二次打印因此是 1, 2, 3, 4, 5。
    }
} // namespace ns1

//     2.2 多参数取反
// std::not_fn 不在乎参数个数。
namespace ns2 {
    bool is_divisible(int a, int b) {
        return (a % b) == 0;
    }
    void test() {
        std::vector<std::pair<int, int>> pairs = { { 10, 2 }, { 10, 3 }, { 20, 5 }, { 20, 6 } };

        auto print_pair = [](const auto &pair) {
            std::cout << "[" << pair.first << ", " << pair.second << "], ";
        };

        // 需求：保留那些 不可整除 的 pair
        // 我们直接用 std::not_fn 包装 is_divisible
        auto is_not_divisible = std::not_fn(is_divisible);

        std::cout << std::boolalpha;
        for (const auto &p : pairs) {
            bool result = is_not_divisible(p.first, p.second);
            std::cout << p.first << " % " << p.second << " != 0 ? : " << result << std::endl;
        }
        std::cout << std::noboolalpha;

        auto end = std::remove_if(pairs.begin(), pairs.end(), [&](const auto &pair) {
            return is_not_divisible(pair.first, pair.second);
        });
        pairs.erase(end, pairs.end());
        for (const auto &p : pairs) {
            print_pair(p);
        }
        std::cout << "\n";
    }
} // namespace ns2

// 2.3 与成员函数结合
// std::not_fn 内部使用
// std::invoke，所以它对成员函数的支持非常自然。示例代码需求：找出所有非活跃用户。
namespace ns3 {
    class User {
    public:
        std::string name;
        bool active;
        User(std::string n, bool a) : name(n), active(a) {}
        bool is_active() const {
            return active;
        }
        explicit operator bool() const {
            return is_active();
        }
        explicit operator bool() {
            return std::as_const(*this).operator bool();
        }
    };
    void test() {
        std::vector<User> users = { { "Alice", true }, { "Bob", false }, { "Charlie", true } };

        auto is_active_ptr = &User::is_active;
        auto is_not_active = std::not_fn(is_active_ptr);

        std::cout << "Inactive users:" << std::endl;
        for (const auto &u : users) {
            if (is_not_active(u)) {
                std::cout << "- " << u.name << std::endl;
            }
        }
    }
} // namespace ns3

// 2.4 引用与拷贝的细节
// 在使用 std::not_fn 时，最大的坑在于它内部到底存了什么？std::not_fn 直接存储传入的可调用对象 f。
// 如果 f
// 是左值引用，它会将引用移除，存储一个拷贝（或移动）。这意味着，如果你有一个巨大的仿函数对象，或者你的仿函数包含状态，你需要注意
// std::not_fn 创建的是一份新的副本。
// 如果你需要 std::not_fn 包装的对象引用原始数据，你需要显式地传引用类型，或者在 C++23 之前通过
// std::ref 包装。
// 下面这段代码通过一个具体的动态过滤器案例，演示了关于函数包装器、对象拷贝与引用传递的高级用法。
namespace ns4 {
    struct ThresholdFilter {
    public:
        int limit;
        explicit ThresholdFilter(int l) : limit(l) {}

        // 为了方便直接调用，重载 ()
        bool operator()(int x) const {
            return x > limit;
        };
    };

    void test() {
        ThresholdFilter my_filter(5);

        auto is_not_above_ref = std::not_fn(std::ref(my_filter));
        auto is_not_above_ref2 = std::not_fn(my_filter);
        auto print_is_not_above = [](const auto &items, const auto &pred) {
            for (const auto &item : items) {
                if (pred(item)) {
                    std::cout << item << " "; // 输出 <= 5 的数
                }
            }
            std::cout << std::endl;
        };

        std::vector<int> nums{ 4, 6, 8, 2 };

        std::cout << "Round 1 (Limit 5): " << std::endl;
        print_is_not_above(nums, is_not_above_ref); // 4 2  // 输出 <= 5 的数

        // --- 动态修改状态 ---
        std::cout << "\nChanging limit to 10..." << std::endl;
        my_filter.limit = 10;

        std::cout << "Round 2 (Limit 10): " << std::endl;
        print_is_not_above(nums, is_not_above_ref); // 4 6 8 2 // 输出 <= 10 的数

        std::cout << "Round 3 (Limit 10): " << std::endl;
        print_is_not_above(nums, is_not_above_ref2); // 4 2  // 输出 <= 5 的数

        // std::not_fn 默认会按值（拷贝）的方式存储传入的可调用对象，如果我们直接写
        // std::not_fn(my_filter)，它会复制一份 my_filter 存在内部，之后外界修改原始的
        // my_filter，std::not_fn 内部的副本不会受影响（状态隔离）。

        // std::ref 是一个引用包装器，用于将对象转换为 std::reference_wrapper 类型，它欺骗了
        // std::not_fn 的模板推导机制。虽然 std::not_fn
        // 依然按值存储，但它存的是一个轻量级的引用对象，通过
        // std::not_fn(std::ref(my_filter))，包装器不再拥有独立的数据，而是指向原始的 my_filter。
    }
} // namespace ns4
int main() {
    ns1::test1();

    ns2::test();

    ns3::test();

    ns4::test();
    return 0;
}