#include <iostream>
#include <functional>
#include <array>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// ============================================================
// Feature 1: lambda capture of *this (C++17)
// ============================================================

// 辅助函数：测试 *this 捕获
namespace {

class S {
public:
    void f()
    {
        int i{ 0 };

        auto l1 = [=] { use(i, x); };     // 拷贝 i, 拷贝 this 指针
        i = 1; x = 1; l1();                // use(0, 1)

        auto l2 = [i, this] { use(i, x); }; // 等价于上面, 显式写法
        i = 2; x = 2; l2();                 // use(1, 2)

        auto l3 = [&] { use(i, x); };      // 引用捕获 i, 拷贝 this 指针
        i = 3; x = 2; l3();                // use(3, 2)

        auto l4 = [i, *this] { use(i, x); }; // C++17: 拷贝 *this, 包括 x 的副本
        i = 4; x = 4; l4();                  // use(3, 2) — i 和 x 都按值拷贝
    }

private:
    int x{ 0 };

    void use(int i, int x) const { std::cout << "i = " << i << ", x = " << x << "\n"; }
};

struct MyObj {
    int value{ 123 };

    auto getValueCopy() {
        return [*this] { return value; }; // C++17: 拷贝 *this
    }

    auto getValueRef() {
        return [this] { return value; };  // C++11: 通过引用捕获 this
    }
};

class Data {
private:
    std::string name;

public:
    Data(const std::string& s) : name(s) { }

    // C++17: 返回一个持有 *this 拷贝的 lambda, 即使原对象析构了也安全
    auto getLambdaWithCopyOfThis() const {
        return [*this] { return std::string("name(copy): ") + name; };
    }

    auto getLambdaWithThisRef() const {
        return [this] { return std::string("name(ref):  ") + name; };
    }

    void setName(const std::string& s) { name = s; }
};

} // namespace

void test_lambda_17_this()
{
    std::cout << "=== Feature 1: lambda capture of *this ===\n\n";

    // 1. 基本 *this vs this 对比
    std::cout << "--- 1. S::f() 各种捕获方式 ---\n";
    S s;
    s.f();

    // 2. MyObj: *this 拷贝 vs this 引用
    std::cout << "\n--- 2. MyObj: *this 拷贝 vs this 引用 ---\n";
    MyObj mo;
    auto valueCopy = mo.getValueCopy();  // 拷贝 *this
    auto valueRef = mo.getValueRef();    // 引用 this
    std::cout << "valueCopy: " << valueCopy() << ", valueRef: " << valueRef() << "\n"; // 123, 123

    mo.value = 321;
    // valueCopy 持有的是旧值(拷贝), valueRef 引用当前对象
    std::cout << "After mo.value = 321:\n";
    std::cout << "valueCopy: " << valueCopy() << ", valueRef: " << valueRef() << "\n"; // 123, 321

    // 3. Data: *this 拷贝避免悬垂引用(模拟线程安全场景)
    std::cout << "\n--- 3. Data: *this 拷贝避免悬垂引用 ---\n";
    std::function<std::string()> fnCopy, fnRef;
    {
        Data d{ "c1" };
        fnCopy = d.getLambdaWithCopyOfThis();  // 拷贝 *this, name="c1"
        fnRef  = d.getLambdaWithThisRef();     // 引用 this
        d.setName("changed");
        // 离开作用域前: fnCopy 持有独立副本, fnRef 引用原对象
        std::cout << "Inside scope:  " << fnCopy() << "\n";         // name(copy): c1
        std::cout << "Inside scope:  " << fnRef() << "\n";          // name(ref):  changed
    } // d 离开作用域被析构! fnCopy 仍然安全(fnRef 变成悬垂引用!)
    // fnCopy 持有 *this 的拷贝, 仍然有效
    std::cout << "Outside scope: " << fnCopy() << " (still safe!)\n"; // name(copy): c1
    // 注意: fnRef() 这里会导致未定义行为, 因为 d 已析构!
    std::cout << "Outside scope: fnRef() would be dangling ref — undefined behavior if called!\n";
}

// ============================================================
// Feature 2: constexpr lambda (C++17)
// ============================================================

constexpr int addOne(int n) { return [n] { return n + 1; }(); }
constexpr auto addOne2(int n) { return [n] { return n + 1; }; }
constexpr auto addOne3 = [](int n) { return n + 1; };

// 隐式 constexpr lambda
auto squared = [](auto val) {
    constexpr int x{ 10 };
    return val * x;
};

// 显式 constexpr lambda (C++17)
auto squared3 = [](auto val) constexpr { return val * val; };
auto squared3i = [](int val) constexpr -> int { return val * val; };

// constexpr lambda 用于编译期初始化
auto squared1 = [](auto val) constexpr { return val * val; };
constexpr auto squared2 = [](auto val) { return val * val; };
constexpr auto squared4 = [](auto val) constexpr { return val * val; };

void test_lambda_17_constexpr()
{
    std::cout << "\n=== Feature 2: constexpr lambda ===\n\n";

    // 1. constexpr lambda 作为函数指针
    auto Fwd = [](int(*fp)(int), auto a) { return fp(a); };
    auto C = [](auto a) { return a; };
    static_assert(Fwd(C, 3) == 3);
    std::cout << "Fwd(C, 3) == " << Fwd(C, 3) << "\n";

    // 2. 显式 constexpr
    auto identity = [](int n) constexpr { return n; };
    static_assert(identity(123) == 123);
    std::cout << "identity(123) == " << identity(123) << "\n";

    // 3. 嵌套 constexpr lambda
    constexpr auto add = [](int x, int y) {
        auto L = [=] { return x; };
        auto R = [=] { return y; };
        return [=] { return L() + R(); };
    };
    static_assert(add(1, 2)() == 3);
    std::cout << "add(1, 2)() == " << add(1, 2)() << "\n";

    // 4. addOne family
    static_assert(addOne(1) == 2);
    static_assert(addOne2(1)() == 2);
    std::cout << "addOne(1): " << addOne(1) << ", addOne2(1)(): " << addOne2(1)() << "\n";

    static_assert(addOne3(1) == 2);
    std::cout << "addOne3(1): " << addOne3(1) << "\n";

    // 5. 立即执行 lambda
    int v = [](int x, int y) { return x + y; }(5, 4);
    std::cout << "v: " << v << "\n"; // 9

    // 6. 嵌套 lambda
    int v2 = [](int x) { return [](int y) { return y * 2; }(x) + 3; }(5);
    std::cout << "v2: " << v2 << "\n"; // 13

    // 7. mutable: 修改按值捕获的变量
    int m = 0, n = 0;
    [&, n](int a) mutable { m = ++n + a; }(4);
    std::cout << "m: " << m << ", n: " << n << "\n"; // m:5, n:0

    // 8. 隐式 constexpr
    auto answer = [](int n) {
        return 32 + n;
    };
    constexpr int response = answer(10);
    static_assert(response == 42);
    std::cout << "response: " << response << "\n";

    // 9. 隐式 constexpr 用于模板参数 (编译期计算)
    std::array<int, squared(5)> arr; // => std::array<int, 50>
    std::cout << "arr.size(): " << arr.size() << "\n"; // 50

    // 10. constexpr lambda 转函数指针
    auto Increment = [](int n) {
        return n + 1;
    };
    constexpr int(*inc)(int) = Increment;
    std::cout << "inc(10): " << inc(10) << "\n"; // 11

    // 11. 最短 lambda: []{}
    auto empty = []{};
    std::cout << "shortest lambda []{} works!\n";
}

// ============================================================
// Main
// ============================================================

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "C++17 Lambda Expressions -- New Features Demo\n";
    std::cout << "==============================================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    test_lambda_17_this();
    test_lambda_17_constexpr();

    std::cout << "\nDone!\n";
    system("pause");
    return 0;
}
