#include <utility>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <map>
#include <string>
#include <random>
#include <algorithm>

// ============================================================
// Feature 1: if with init-statement (C++17)
// 在 if 条件中声明并初始化变量，作用域限于整个 if-else 块
// ============================================================

constexpr int get_value() {
    return 66;
}

void test_if_init() {
    std::cout << "=== Feature 1: if with init-statement ===\n\n";

    std::map<std::string, std::string> addrs{ { "csdn", "https://blog.csdn.net/fengbingchun/" },
        { "github", "https://github.com/fengbingchun" } };

    // C++17: if (init; condition)
    if (auto it = addrs.find("github"); it != addrs.end())
        std::cout << "github: " << it->second << "\n";

    // else 分支中也能使用 init 中声明的变量
    if (auto it = addrs.find("csdn"); it == addrs.end())
        std::cout << "no find\n";
    else
        std::cout << "csdn: " << it->second << "\n";

    // 配合结构化绑定
    if (auto [it, inserted] = addrs.insert({ "gitee", "https://gitee.com/fengbingchun/test.git" });
        !inserted)
        std::cout << "already exists\n";
    else
        std::cout << "inserted: key=" << it->first << ", value=" << it->second << "\n";

    // 与 constexpr 函数配合
    if (auto x = get_value(); x == 66)
        std::cout << "x is: " << x << "\n";

    // 初始化多个变量
    if (auto val1 = addrs.cbegin()->first, val2 = addrs.crbegin()->first; val1 != val2)
        std::cout << "val1: " << val1 << ", val2: " << val2 << "\n";

    // 配合 any_of 使用
    const std::string str{ "if" };
    if (auto keywords = { "if", "for", "while" };
        std::any_of(keywords.begin(), keywords.end(), [&str](const char *kw) { return str == kw; }))
        std::cout << "Error: Token must not be a keyword\n";
}

// ============================================================
// Feature 2: switch with init-statement (C++17)
// ============================================================

void test_switch_init() {
    std::cout << "\n=== Feature 2: switch with init-statement ===\n\n";

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 100);

    // C++17: switch (init; expression)
    switch (auto val = dist(mt); val) {
    default:
        std::cout << "random val: " << val << "\n";
    }

    // 另一个例子：在 switch 中初始化并限定作用域
    switch (auto n = 42; n % 7) {
    case 0:
        std::cout << "42 is divisible by 7 (remainder=0)\n";
        break;
    default:
        std::cout << "remainder: " << n % 7 << "\n";
    }
}

// ============================================================
// Feature 3: if constexpr (C++17)
// 编译期条件分支，只编译匹配的代码路径
// ============================================================

template<typename T>
auto show(T t) {
    // if constexpr 在编译期求值，非指针路径的 *t 不会被编译
    if constexpr (std::is_pointer_v<T>)
        return *t; // 仅对指针类型编译此分支
    else
        return t; // 仅对非指针类型编译此分支
}

template<typename T>
void print_value(const T &value) {
    if constexpr (std::is_same_v<T, std::string>)
        std::cout << "type: std::string, value: " << value << ", length: " << value.length()
                  << "\n";
    else if constexpr (std::is_same_v<T, int>)
        std::cout << "type: int, value: " << value << "\n";
    else if constexpr (std::is_same_v<T, float>)
        std::cout << "type: float, value: " << value << "\n";
    else
        std::cout << "unsupported type\n";
}

void test_if_constexpr() {
    std::cout << "\n=== Feature 3: if constexpr ===\n\n";

    int a = 66;
    int *p = &a;

    std::cout << "show(a): " << show(a) << "\n"; // 66
    std::cout << "show(p): " << show(p) << "\n"; // 66 (解引用)

    std::string str{ "hello" };
    print_value(str); // type: std::string, value: hello, length: 5
    print_value(a); // type: int, value: 66
    float val{ .6f };
    print_value(val); // type: float, value: 0.6
    print_value(p); // unsupported type
}

// ============================================================
// Main
// ============================================================

// if/switch 语句中的初始化器
// https://subingwen.cn/cpp/if-switch/
namespace ns1 {
    void test() {
        std::map<int, std::string> my_data{ { 1, "apple" }, { 2, "banana" } };

        // --- 痛点写法 (C++17之前) ---
        // 第 1 步：我们必须在 if 外面先声明这个变量
        std::map<int, std::string>::iterator it;
        // 第 2 步：在 if 里面进行初始化和赋值
        if ((it = my_data.find(2)) != my_data.end()) {
            // 第 3 步：在这里使用 it
            std::cout << "found: " << it->second << "\n";
        }

        // C++17 允许我们把变量的初始化直接写在 if 或 switch 的条件判断之前
        if (auto it2 = my_data.find(1); it2 != my_data.end()) {
            // 第 3 步：在这里使用 it
            std::cout << "found: " << it2->second << "\n";
        }
    }

    // 1. 模拟一个可能获取不到资源的函数
    std::pair<bool, int> get_resource(int id) {
        if (id > 0) {
            return { true, id * 100 };
        }
        return { false, 0 };
    }
    void test2() {
        std::map<int, std::string> my_data = { { 1, "Apple" }, { 2, "Banana" } };
        // --- C++17 新写法 ---
        // 1. 定义变量 it
        // 2. 初始化 it (执行 find)
        // 3. 检查 it 是否等于 end()
        // 中间用分号 ; 隔开
        if (auto it = my_data.find(2); it != my_data.end()) {
            std::cout << "Found inside if: " << it->second << std::endl;
        } else {
            std::cout << "Not found." << std::endl;
        }

        if (auto [success, value] = get_resource(5); success) {
            std::cout << "成功: 获取到值 " << value << std::endl;
        } else {
            std::cout << "失败: 资源不可用" << std::endl;
        }
    }

    enum class Command : char { Start, Stop, Pause };
    std::vector<std::string> system_logs{};
    void test3() {
        // 1.2 switch 语句初始化器详解
        // 同样的规则也适用于
        // switch。这在处理状态机或枚举时非常有用。假设我们有一个设备状态，需要根据控制器的命令来把详细的日志信息构建出来并存储。
        Command cmd = Command::Start;
        std::string device_status = "Idle";

        std::cout << "=== 执行命令 ===" << std::endl;

        // --- C++17 Switch 写法 (改进版) ---
        // 初始化临时日志变量 msg
        switch (std::string msg = "Log Device"; cmd) {
        case Command::Start:
            msg += "started successfully.";
            device_status = "Running";
            // 逻辑意义：把处理结果存入系统日志
            system_logs.push_back(msg);
            break;
        case Command::Stop:
            msg += "stopped by user.";
            device_status = "Stopped";
            system_logs.push_back(msg);
            break;

        case Command::Pause:
            msg += "paused temporarily.";
            device_status = "Paused";
            system_logs.push_back(msg);
            break;

        default:
            break;
        }
        // switch 结束，msg 被销毁，但它留下的痕迹在 system_logs 里
        // 打印刚才生成的日志，证明 msg 确实起作用了
        std::cout << "System Logs: [" << system_logs.back() << "]" << std::endl;
    }
} // namespace ns1

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "C++17 if / switch -- New Features Demo\n";
    std::cout << "========================================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_if_init();
    test_switch_init();
    test_if_constexpr();

    std::cout << "\nDone!\n";

    ns1::test();
    ns1::test2();
    ns1::test3();
    return 0;
}
