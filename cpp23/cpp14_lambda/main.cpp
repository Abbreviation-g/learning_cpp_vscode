#include <array>
#include <cmath>
#include <cstddef>
#include <ios>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <functional>

namespace lambda_ns {
    void test_lambda_11() {
        auto num = 100;
        auto ch = 'c';
        auto sum = [](int x, int y) -> int {
            return x + y;
        };
        std::cout << "test_lambda_11: " << sum(num, ch) << std::endl;
    }
    void test_lambda_14() {
        auto num = 100;
        auto ch = 'c';
        auto sum = [](auto x, auto y) -> decltype(x) {
            return x + y;
        };

        std::cout << "test_lambda_14: " << sum(num, ch) << std::endl;
        std::cout << "test_lambda_14: " << sum(3.3, 2.2) << std::endl;
        using namespace std::string_literals;
        std::cout << "test_lambda_14: " << sum("hello"s, "world"s) << std::endl;

        // 比较两个变量的值是否相等
        auto compare = [](auto p1, auto p2) -> decltype(p1 == p2) {
            return p1 == p2;
        };
        std::cout << std::boolalpha;
        std::cout << "compare: " << compare(1, 1) << std::endl;
        std::cout << "compare: " << compare("hello", "world") << std::endl;
        std::cout << std::noboolalpha;

        // 通过泛型lambda进行容器功能封装 -> 序列容器，没有自己的find方法，std::find
        // std::vector, std::array, std::deque, std::list, std::forward_list
        auto find = [](const auto &container, const auto &value) -> bool {
            const auto &it = std::find(container.begin(), container.end(), value);
            return it != container.end();
        };
        std::vector nums{ 1, 2, 3, 4, 5 };
        std::list<std::string> words{ "apple", "banana", "cherry" };
        std::array<int, 6> arr{ 1, 2, 3, 4, 5, 6 };

        std::cout << std::boolalpha;
        std::cout << find(nums, 6) << std::endl;
        std::cout << find(words, "12") << std::endl;
        std::cout << find(arr, 6) << std::endl;
        std::cout << std::noboolalpha;
    }

} // namespace lambda_ns

namespace lambda_ns_sort {
    // 打印方式
    auto print = [](const auto &p) -> void {
        std::cout << "降序排序之后" << std::endl;
        for (auto &v : p) {
            std::cout << v << ", ";
        }
        std::cout << std::endl;
    };
    void test_lambda_14_sort() {
        // 实现泛型排序

        // 指定排序规则
        auto descending = [](const auto &p1, const auto &p2) -> bool {
            return p1 > p2;
        };

        std::vector<int> nums = { 1, 2, 3, 4, 5 };
        std::vector<std::string> words = { "apple", "banana", "cherry" };
        std::sort(nums.begin(), nums.end(), descending);
        std::sort(words.begin(), words.end(), descending);
        print(nums);
        print(words);
    }

    struct People {
        std::string name;
        int age;
        // 声明友元函数：允许全局 operator<< 访问 People 的成员
        friend std::ostream &operator<<(std::ostream &os, const People &p);
    };
    std::ostream &operator<<(std::ostream &os, const People &p) {
        os << "{" << "name:" << p.name << ", age:" << p.age << "}";
        return os;
    }
    void sort_people() {
        std::vector<People> peoples{
            People{ .name = "Alice", .age = 19 },
            People{ .name = "Tom", .age = 20 },
            People{ .name = "Lucy", .age = 21 },
            People{ .name = "Bobo", .age = 22 },
        };

        auto sort_by_member = [](const auto &p1, const auto &p2) -> bool {
            return p1.age > p2.age;
        };
        // 对象比较大小，要求通过函数能够根据任意一个成员进行比较
        // 相当于是一个工厂函数，用于生产一个比较函数
        auto sort_by_member_plus = [](const auto memberPtr) {
            // 接收sort排序函数传递过来的参数
            return [memberPtr](const auto &p1, const auto &p2) -> bool {
                return p1.*memberPtr > p2.*memberPtr;
            };
        };

        std::sort(peoples.begin(), peoples.end(), sort_by_member_plus(&People::age));
        print(peoples);
        std::sort(peoples.begin(), peoples.end(), sort_by_member_plus(&People::name));
        print(peoples);
    }

    // 泛型函数组合器 -> 本质上是高阶函数
    // 1. 接收函数作为参数(泛型lambda)，函数组合->实现新的功能更，类型安全
    void my_compose() {
        // 子函数
        auto increment = [](auto x) -> decltype(x) {
            return x + 1;
        };
        auto square = [](auto x) -> decltype(x) {
            return x * x;
        };
        auto to_string = [](auto x) -> std::string {
            return std::to_string(x);
        };

        // 组合器函数 -> 传递两个函数作为参数, f(g(x))
        auto compose = [](auto f, auto g) {
            return [f, g](auto value) {
                return f(g(value));
            };
        };

        // 需求1，将一个整数+1再平方
        auto increment_square = compose(square, increment);
        // 需求2， 求一个数的平方，再转换为字符串
        auto square_to_string = compose(to_string, square);

        auto r1 = increment_square(2);
        auto r2 = square_to_string(4);
        std::cout << "increment_square(2)" << r1 << std::endl;
        std::cout << "square_to_string(4)" << r2 << std::endl;
    }

    // 2. 捕获变量的增强
    // C++14 引入了 初始化捕获（也称为 广义 lambda 捕获），允许在 lambda 捕获列表中直接初始化捕获的变量。这是在 C++11 基础上对 lambda
    // 功能的重大扩展。基本语法如下：
    //  C++14 初始化捕获语法
    //  [捕获变量 = 初始化表达式](参数列表) -> 返回类型 { 函数体 }
    // lambda表达式是闭包(函数和捕获的变量的组合)
    void test_lambda_catch() {
        int num = 100;
        auto func = [num](int x, int y) {
            return num * (x + y);
        };

        // func等价于xxx_lambda这个类
        class xxx_lambda {
        public:
            xxx_lambda(int value) : num(value) {
            }
            auto operator()(int x, int y) const {
                return num * (x + y);
            }

        private:
            int num;
        };
    }

    // 值捕获
    void test_lambda_value_catch() {
        int number = 100; // 外部变量
        auto lambda = [z = number](int x, int y) {
            // z = 101; //  只读类型，不能修改
            std::cout << "// z = 101; //  只读类型，不能修改" << std::endl;
            return x + y + z;
        };
        std::cout << "lambda " << lambda(0, 1) << ", number: " << number << std::endl;
        auto lambda1 = [z = number](int x, int y) mutable {
            // z = 101; //  只读类型，不能修改
            std::cout << "// z = 101; //  只读类型，不能修改" << std::endl;
            z = 10001;
            return x + y + z;
        };
        std::cout << "lambda1 " << lambda1(0, 1) << ", number: " << number << std::endl;

        using namespace std::string_literals;
        std::string str = "hello"s;
        std::vector<std::string> words = { "apple", "banana", "cherry" };
        auto lambda2 = [s = str, w = words]() {
            std::cout << "string: " << s << std::endl;
            std::cout << "w.size(): " << w.size() << std::endl;
            std::cout << "w[0]: " << w[0] << std::endl;
        };
        str = "world"s;
        words.push_back(str);
        lambda2();

        // 捕获表达式的返回值
        int base = 2;
        int ex = 5;
        auto lambda3 = [res = static_cast<int>(std::pow(base, ex))]() {
            std::cout << "std::pow(base, ex)\t" << res << std::endl;
        };
        lambda3();
    }

    // 引用捕获，捕获的是引用本身，而不是值
    void test_lambda_reference_catch() {
        // cpp11的引用捕获
        int number = 100;
        auto lambda = [&number]() {
            number++;
            std::cout << "inside lambda\t" << number << std::endl;
        };
        lambda();
        std::cout << "after lambda\t" << number << std::endl;

        number = 100;
        // c++14 -> 初始化应用捕获
        auto lambda2 = [&y = number]() {
            y++;
            std::cout << "inside lambda2\t" << y << std::endl;
        };
        lambda2();
        std::cout << "after lambda2\t" << number << std::endl;

        // 使用ref进行引用捕获
        number = 100;
        // c++14 -> 初始化引用捕获，使用ref得到一个引用包装器
        auto lambda3 = [y = std::ref(number)]() {
            y++;
            // y = 10002; // Candidate function not viable: 'this' argument has type 'const reference_wrapper<int>', but method is not marked const
            y.get() = 100001;
            std::cout << "inside lambda3\t" << y.get() << std::endl;
        };
        lambda3();
        std::cout << "after lambda3\t" << number << std::endl;

        // 常量引用初始化
        auto lambda4 = [&y = static_cast<const int &>(number)]() {
            std::cout << "inside lambda4\t" << y << std::endl;
            // y = 1000; // Cannot assign to variable 'y' with const-qualified type 'const int &'
        };
        lambda4();
        std::cout << "after lambda4\t" << number << std::endl;

        auto lambda5 = [y = std::cref(number)]() {
            std::cout << "inside lambda5\t" << y << std::endl;
            // y.get() = 10086; // Function 'get' which returns const-qualified type 'const int &' declared here
        };
        lambda5();
        std::cout << "after lambda5\t" << number << std::endl;
    }

    // 移动捕获
    // 移动语义：将外部变量var的内容移动到lambda内部的var中
    // 转移所有权：外部var进入有效但未指定的状态（通常为空或默认状态）
    // 性能优化：避免不必要的拷贝，特别是对于大型对象或只移动类型（如std::unique_ptr）
    // var = std::move(var) ：对lambda外部的变量var进行移动构造
    void test_lambda_move_catch() {
        std::vector<int> num{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        // move的作用，将参数的左值进行了类型转换->右值引用(右值)
        // static_cast<vector<int>&&>(num)
        std::move(num); // 这一行产生了一个右值引用，但你并没有接收它，也没有用它来初始化或赋值给其他对象。
        print(num); // 1, 2, 3, 4, 5, 6, 7, 8, 9,  // 能够打印出来，是因为std

        auto num1 = std::move(
            num); // 移动构造函数,真正的“移动”（即资源所有权的转移，如指针指向变更、大小重置为0等）只有在发生‌初始化‌或‌赋值‌时才会触发。
        print(num); // 空的
        print(num1); // 1, 2, 3, 4, 5, 6, 7, 8, 9,

        // 普通对象的移动捕获
        auto lambda1 = [vec = std::move(num1)]() mutable {
            vec.push_back(100);
            print(vec); // 1, 2, 3, 4, 5, 6, 7, 8, 9, 100,
            return vec;
        };
        auto vec2 = lambda1();
        print(vec2); // 1, 2, 3, 4, 5, 6, 7, 8, 9, 100,
        print(num1); // 空的

        // 独占智能指针，只能进行移动捕获
        std::unique_ptr<int> ptr(new int(42));
        auto lambda2 = [ptr = std::move(ptr)]() {
            std::cout << "ptr value = " << *(ptr.get()) << std::endl; // ptr value = 42
            *ptr = 1000;
            std::cout << "new ptr value = " << *ptr << std::endl; // new ptr value = 1000
        };
        std::cout << "unique_ptr: " << (ptr == nullptr ? "null" : "not null") << std::endl;
        lambda2(); // unique_ptr: null

        // 1.3.2 移动其它类型
        std::string large_string = "这是一个很长的字符串...";
        auto lambda5 = [str = std::move(large_string)]() {
            std::cout << str << std::endl; // 这是一个很长的字符串...
        };
        lambda5();
        std::cout << "原始字符串: \"" << large_string << "\"" << std::endl; // 输出为空
    }

} // namespace lambda_ns_sort

int main() {
    lambda_ns::test_lambda_11();
    lambda_ns::test_lambda_14();

    lambda_ns_sort::test_lambda_14_sort();
    lambda_ns_sort::sort_people();
    lambda_ns_sort::my_compose();
    lambda_ns_sort::test_lambda_value_catch();
    lambda_ns_sort::test_lambda_reference_catch();
    lambda_ns_sort::test_lambda_move_catch();
    return 0;
}