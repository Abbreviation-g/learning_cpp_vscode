#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>

// 万能调用神技 std::invoke
// https://subingwen.cn/cpp/invoke/

// 2.1 调用普通函数和 Lambda
// 调用普通函数和Lambda这是最简单的用法，虽然看起来和直接调用差不多，但它是通用的基础。
namespace ns1 {
    // 1. 一个普通的自由函数
    void print_hello(const std::string &name) {
        std::cout << "你好, " << name << "!" << std::endl;
    }
    // 2. 一个函数对象（仿函数）
    struct Printer {
    public:
        void operator()(int x) {
            std::cout << "数字是: " << x << std::endl;
        }
    };
    void test() {
        // --- 调用普通函数 ---
        std::invoke(print_hello, "allice");
        // --- 调用 Lambda 表达式 ---
        auto lambda = [](double d) {
            std::cout << "Pi = " << d << std::endl;
        };
        std::invoke(lambda, 3.14);
        // --- 调用函数对象 ---
        Printer p{};
        std::invoke(p, 0b110);
    }
} // namespace ns1
// 2.2 调用类的成员函数
// 调用类的成员函数这是 std::invoke
// 最闪光的地方。以前处理成员函数非常麻烦，因为需要把对象和函数指针拼起来。调用类的成员函数时，语法主要取决于对象是以引用、指针还是智能指针的形式传递。

// std::invoke(&ClassName::MethodName, instance, args...);
// 参数1：成员函数的指针必须使用取址符 &。
// 参数2：类的实例对象或者对象引用、对象指针或者智能指针。
// 参数3…：传递给成员函数的参数。
namespace ns2 {
    class Player {
    private:
        std::string name;
        int score;

    public:
        explicit Player(std::string n, int s) : name(n), score(s) {}
        // 为了方便打印，我们增加一个 getter
        int get_score() const {
            return score;
        }
        std::string get_name() const {
            return name;
        }
        int get_effective_score() const {
            return score;
        }
    };

    // 通用比较器
    template <auto Member, typename Compare = std::less<>>
    struct CompareBy {
        template <typename T>
        bool operator()(const T &a, const T &b) const {
            return Compare{}(std::invoke(Member, a), std::invoke(Member, b));
        }
    };
    void test() {
        std::vector<Player> players{ Player{ "Bob", 100 }, Player{ "Alice", 200 },
            Player{ "Charlie", 150 } };
        std::cout << "排序前: " << "\n";
        auto print_players = [](const auto &items) {
            for (const auto &p : items) {
                std::cout << p.get_name() << ": " << p.get_score() << std::endl;
            }
            std::cout << "-------------------" << std::endl;
        };
        print_players(players);

        // 使用 std::sort 进行排序
        // 这里传入了 CompareBy 的实例，并指定成员函数指针
        std::sort(players.begin(), players.end(), CompareBy<&Player::get_effective_score>{});
        std::cout << "排序后 (升序):" << std::endl;
        print_players(players);

        std::sort(players.begin(), players.end(),
            CompareBy<&Player::get_effective_score, std::greater<>>{});
        std::cout << "排序后 (降序):" << std::endl;
        print_players(players);
    }
} // namespace ns2

// 2.3 调用重载的成员函数
//  如果成员函数被重载了（即有几个同名函数，参数不同），直接写 &ClassName::Func
//  会导致编译器无法确定是哪一个。必须显式转换为正确的函数指针类型。
namespace ns3 {
    class Widget {
    public:
        void process(int x) {
            std::cout << "Int: " << x << std::endl;
        }
        void process(double x) {
            std::cout << "Double: " << x << std::endl;
        }
    };
    void test() {
        Widget w{};

        // 错误：无法推断是 process(int) 还是 process(double)
        // std::invoke(&Widget::process, w, 10);

        // 正确：指定函数指针类型
        using FuncType = void (Widget::*)(int);
        std::invoke(static_cast<FuncType>(&Widget::process), w, 10);

        using FuncType2 = void (Widget::*)(double);
        std::invoke(static_cast<FuncType2>(&Widget::process), w, 10.001);
    }
} // namespace ns3

// 2.4 访问类的数据成员
// std::invoke 的核心设计理念是：统一调用可调用对象。而在 C++
// 中，成员变量（非静态数据成员）也被视为一种可调用对象。当你使用 std::invoke
// 配合成员变量指针时，它实际上执行的是成员访问操作，而不是函数调用。对于成员变量，std::invoke
// 的行为等效于：

// 对象或指针 ：std::invoke(&Class::member, obj) 等价于 obj.member、obj->member
// 对象引用：std::invoke(&Class::member, std::ref(obj)) 等价于 obj.member
// 假设有一个通用的打印函数，它既可以处理成员函数（获取结果），也可以直接处理成员变量（直接读取值），完全不需要重载。
namespace ns4 {
    struct User {
        std::string name;
        int age;
        friend std::ostream &operator<<(std::ostream &os, const User &user);

        int get_age() const {
            return age;
        }
        static std::string get_role() {
            return "Admin";
        }
    };
    std::ostream &operator<<(std::ostream &os, const User &user) {
        return os << "User{name: " << user.name << ", age: " << user.age << "}";
    }

    // 通用打印函数：不关心 T 是成员变量还是成员函数
    template <auto Member, typename Obj>
    void print_member(const Obj &obj) {
        std::cout << "Value: " << std::invoke(Member, obj) << std::endl;
    }
    // 通用打印函数：调用静态函数
    template <auto Member>
    void print_member() {
        std::cout << "Value: " << std::invoke(Member) << std::endl;
    }

    void test() {
        User u{ "Alice", 30 };
        // 场景 A：直接“调用”成员变量 name
        std::cout << "Direct Field: ";
        print_member<&User::name>(u);

        // 场景 B：直接“调用”成员变量 age
        std::cout << "Direct Field: ";
        print_member<&User::age>(u);

        // 场景 C：调用真正的成员函数
        std::cout << "Method Call: ";
        print_member<&User::get_age>(u);

        // 场景 D：调用静态函数
        std::cout << "static Method Call: ";
        print_member<&User::get_role>();

        std::cout << "Friend function: " << u << std::endl;
    }
} // namespace ns4
int main(int argc, char *argv[]) {
    ns1::test();

    ns2::test();

    ns3::test();

    ns4::test();
}