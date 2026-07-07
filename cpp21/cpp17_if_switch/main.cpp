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

constexpr int get_value() { return 66; }

void test_if_init()
{
    std::cout << "=== Feature 1: if with init-statement ===\n\n";

    std::map<std::string, std::string> addrs{
        {"csdn",   "https://blog.csdn.net/fengbingchun/"},
        {"github", "https://github.com/fengbingchun"}
    };

    // C++17: if (init; condition)
    if (auto it = addrs.find("github"); it != addrs.end())
        std::cout << "github: " << it->second << "\n";

    // else 分支中也能使用 init 中声明的变量
    if (auto it = addrs.find("csdn"); it == addrs.end())
        std::cout << "no find\n";
    else
        std::cout << "csdn: " << it->second << "\n";

    // 配合结构化绑定
    if (auto [it, inserted] = addrs.insert({"gitee", "https://gitee.com/fengbingchun/test.git"}); !inserted)
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
        std::any_of(keywords.begin(), keywords.end(),
                    [&str](const char* kw) { return str == kw; }))
        std::cout << "Error: Token must not be a keyword\n";
}

// ============================================================
// Feature 2: switch with init-statement (C++17)
// ============================================================

void test_switch_init()
{
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
auto show(T t)
{
    // if constexpr 在编译期求值，非指针路径的 *t 不会被编译
    if constexpr (std::is_pointer_v<T>)
        return *t;   // 仅对指针类型编译此分支
    else
        return t;    // 仅对非指针类型编译此分支
}

template<typename T>
void print_value(const T& value)
{
    if constexpr (std::is_same_v<T, std::string>)
        std::cout << "type: std::string, value: " << value
                  << ", length: " << value.length() << "\n";
    else if constexpr (std::is_same_v<T, int>)
        std::cout << "type: int, value: " << value << "\n";
    else if constexpr (std::is_same_v<T, float>)
        std::cout << "type: float, value: " << value << "\n";
    else
        std::cout << "unsupported type\n";
}

void test_if_constexpr()
{
    std::cout << "\n=== Feature 3: if constexpr ===\n\n";

    int a = 66;
    int* p = &a;

    std::cout << "show(a): " << show(a) << "\n";   // 66
    std::cout << "show(p): " << show(p) << "\n";   // 66 (解引用)

    std::string str{ "hello" };
    print_value(str);     // type: std::string, value: hello, length: 5
    print_value(a);       // type: int, value: 66
    float val{ .6f };
    print_value(val);     // type: float, value: 0.6
    print_value(p);       // unsupported type
}

// ============================================================
// Main
// ============================================================

int main()
{
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
    return 0;
}
