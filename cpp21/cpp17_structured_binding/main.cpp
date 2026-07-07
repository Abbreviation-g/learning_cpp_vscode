
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <utility>

// ============================================================
// C++17 Structured Bindings
// ============================================================

// ---- 1. Binding an array ----
void test_bind_array()
{
    std::cout << "=== 1. Binding an array ===\n\n";

    int arr1[3] = { 1, 2, 3 };

    auto [x, y, z] = arr1;      // copy
    std::cout << "x:" << x << ", y:" << y << ", z:" << z << "\n";
    x = 4;
    std::cout << "arr1[0]:" << arr1[0] << " (unchanged, copy)\n";

    auto& [x2, y2, z2] = arr1;  // reference
    std::cout << "x2:" << x2 << ", y2:" << y2 << ", z2:" << z2 << "\n";
    x2 = 5;
    std::cout << "arr1[0]:" << arr1[0] << " (changed, ref)\n";

    const auto& [x3, y3, z3] {arr1};
    const auto& [x4, y4, z4](arr1);
    std::cout << "x3:" << x3 << ", x4:" << x4 << "\n";
}

// ---- 2. Binding a tuple-like type ----
void test_bind_tuple()
{
    std::cout << "\n=== 2. Binding a tuple-like type ===\n\n";

    std::tuple<std::string, int, float> foo{ "csdn", 8, 6.6f };

    const auto& [x3, y3, z3] = foo;
    std::cout << "x3:" << x3 << ", y3:" << y3 << ", z3:" << z3 << "\n";

    auto& [x4, y4, z4] = foo;
    x4 = "github";
    y4 = 6;
    z4 = 8.8f;
    std::cout << "foo: " << std::get<0>(foo) << "," << std::get<1>(foo)
              << "," << std::get<2>(foo) << "\n";

    int a = 1, b = 2;
    const auto& [x6, y6] = std::tie(a, b);
    x6 = 6;
    auto [x7, y7] = std::tie(a, b);
    std::cout << "x7:" << x7 << ", a:" << a << "\n";
    x7 = 8;
    std::cout << "x6:" << x6 << ", a:" << a << "\n";
    if (&x6 != &x7)
        std::cout << "Error: &x6 != &x7\n";
}

// ---- 3. Binding to data members (struct) ----
namespace {

struct Info {
    mutable std::string name;
    volatile int number;
};

inline Info func() { return Info{ "csdn", 666 }; }

struct Data {
    int b{ 1 }, d{ 2 }, p{ 3 }, q{ 4 };
};

} // namespace

void test_bind_struct()
{
    std::cout << "\n=== 3. Binding to data members ===\n\n";

    const auto [x5, y5] = func();
    std::cout << "x5:" << x5 << ", y5:" << y5 << "\n";
    x5 = "github";

    const auto [b1, d1, p1, q1] = Data{};
    std::cout << "b1:" << b1 << ", d1:" << d1 << ", p1:" << p1 << ", q1:" << q1 << "\n";

    const auto [b2, d2, p2, q2] = Data{ 4, 3, 2, 1 };
    std::cout << "b2:" << b2 << ", d2:" << d2 << ", p2:" << p2 << ", q2:" << q2 << "\n";

    Data s;
    auto& [b3, d3, p3, q3] = s;
    std::cout << "b3:" << b3 << ", d3:" << d3 << ", p3:" << p3 << ", q3:" << q3 << "\n";
    b3 = 4; d3 = 3; p3 = 2; q3 = 1;
    std::cout << "s.b:" << s.b << ", s.d:" << s.d << ", s.p:" << s.p << ", s.q:" << s.q << "\n";
}

// ---- 4. map iteration with structured bindings ----
void test_bind_map()
{
    std::cout << "\n=== 4. map iteration with structured bindings ===\n\n";

    const std::map<std::string, std::string> addrs{
        {"csdn",   "https://blog.csdn.net/fengbingchun/"},
        {"github", "https://github.com/fengbingchun"}
    };

    for (const auto& [key, val] : addrs) {
        std::cout << "key: " << key << ", addr: " << val << "\n";
    }
}

// ---- 5. move semantics with structured bindings ----
void test_bind_move()
{
    std::cout << "\n=== 5. move semantics with structured bindings ===\n\n";

    Info info{ "github", 888 };
    auto&& [u, v] = std::move(info);
    std::cout << "info.name: " << info.name << " (still holds value)\n";

    Info info2{ "csdn", 666 };
    auto [u2, v2] = std::move(info2);
    std::cout << "info2.name: '" << info2.name << "' (moved-from, empty)\n";
}

// ============================================================
// Main
// ============================================================

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "C++17 Structured Bindings Demo\n";
    std::cout << "==============================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_bind_array();
    test_bind_tuple();
    test_bind_struct();
    test_bind_map();
    test_bind_move();

    std::cout << "\nDone!\n";
    return 0;
}
