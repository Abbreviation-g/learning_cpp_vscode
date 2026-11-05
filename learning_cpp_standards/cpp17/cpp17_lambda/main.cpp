#include <iostream>
#include <functional>
#include <array>
#include <string>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ns1 {
    // ============================================================
    // Feature 1: lambda capture of *this (C++17)
    // ============================================================

    // 辅助函数：测试 *this 捕获
    namespace {

        class S {
        public:
            void f() {
                int i{ 0 };

                auto l1 = [=] { use(i, x); }; // 拷贝 i, 拷贝 this 指针
                i = 1;
                x = 1;
                l1(); // use(0, 1)

                auto l2 = [i, this] { use(i, x); }; // 等价于上面, 显式写法
                i = 2;
                x = 2;
                l2(); // use(1, 2)

                auto l3 = [&] { use(i, x); }; // 引用捕获 i, 拷贝 this 指针
                i = 3;
                x = 2;
                l3(); // use(3, 2)

                auto l4 = [i, *this] { use(i, x); }; // C++17: 拷贝 *this, 包括 x 的副本
                i = 4;
                x = 4;
                l4(); // use(3, 2) — i 和 x 都按值拷贝
            }

        private:
            int x{ 0 };

            void use(int i, int x) const {
                std::cout << "i = " << i << ", x = " << x << "\n";
            }
        };

        struct MyObj {
            int value{ 123 };

            auto getValueCopy() {
                return [*this] { return value; }; // C++17: 拷贝 *this
            }

            auto getValueRef() {
                return [this] { return value; }; // C++11: 通过引用捕获 this
            }
        };

        class Data {
        private:
            std::string name;

        public:
            Data(const std::string &s) : name(s) {}

            // C++17: 返回一个持有 *this 拷贝的 lambda, 即使原对象析构了也安全
            auto getLambdaWithCopyOfThis() const {
                return [*this] { return std::string("name(copy): ") + name; };
            }

            auto getLambdaWithThisRef() const {
                return [this] { return std::string("name(ref):  ") + name; };
            }

            void setName(const std::string &s) {
                name = s;
            }
        };

    } // namespace

    void test_lambda_17_this() {
        std::cout << "=== Feature 1: lambda capture of *this ===\n\n";

        // 1. 基本 *this vs this 对比
        std::cout << "--- 1. S::f() 各种捕获方式 ---\n";
        S s;
        s.f();

        // 2. MyObj: *this 拷贝 vs this 引用
        std::cout << "\n--- 2. MyObj: *this 拷贝 vs this 引用 ---\n";
        MyObj mo;
        auto valueCopy = mo.getValueCopy(); // 拷贝 *this
        auto valueRef = mo.getValueRef(); // 引用 this
        std::cout << "valueCopy: " << valueCopy() << ", valueRef: " << valueRef()
                  << "\n"; // 123, 123

        mo.value = 321;
        // valueCopy 持有的是旧值(拷贝), valueRef 引用当前对象
        std::cout << "After mo.value = 321:\n";
        std::cout << "valueCopy: " << valueCopy() << ", valueRef: " << valueRef()
                  << "\n"; // 123, 321

        // 3. Data: *this 拷贝避免悬垂引用(模拟线程安全场景)
        std::cout << "\n--- 3. Data: *this 拷贝避免悬垂引用 ---\n";
        std::function<std::string()> fnCopy, fnRef;
        {
            Data d{ "c1" };
            fnCopy = d.getLambdaWithCopyOfThis(); // 拷贝 *this, name="c1"
            fnRef = d.getLambdaWithThisRef(); // 引用 this
            d.setName("changed");
            // 离开作用域前: fnCopy 持有独立副本, fnRef 引用原对象
            std::cout << "Inside scope:  " << fnCopy() << "\n"; // name(copy): c1
            std::cout << "Inside scope:  " << fnRef() << "\n"; // name(ref):  changed
        } // d 离开作用域被析构! fnCopy 仍然安全(fnRef 变成悬垂引用!)
        // fnCopy 持有 *this 的拷贝, 仍然有效
        std::cout << "Outside scope: " << fnCopy() << " (still safe!)\n"; // name(copy): c1
        // 注意: fnRef() 这里会导致未定义行为, 因为 d 已析构!
        std::cout
            << "Outside scope: fnRef() would be dangling ref — undefined behavior if called!\n";
    }

    // ============================================================
    // Feature 2: constexpr lambda (C++17)
    // ============================================================

    constexpr int addOne(int n) {
        return [n] { return n + 1; }();
    }
    constexpr auto addOne2(int n) {
        return [n] { return n + 1; };
    }
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

    void test_lambda_17_constexpr() {
        std::cout << "\n=== Feature 2: constexpr lambda ===\n\n";

        // 1. constexpr lambda 作为函数指针
        auto Fwd = [](int (*fp)(int), auto a) { return fp(a); };
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
        auto answer = [](int n) { return 32 + n; };
        constexpr int response = answer(10);
        static_assert(response == 42);
        std::cout << "response: " << response << "\n";

        // 9. 隐式 constexpr 用于模板参数 (编译期计算)
        std::array<int, squared(5)> arr; // => std::array<int, 50>
        std::cout << "arr.size(): " << arr.size() << "\n"; // 50

        // 10. constexpr lambda 转函数指针
        auto Increment = [](int n) { return n + 1; };
        constexpr int (*inc)(int) = Increment;
        std::cout << "inc(10): " << inc(10) << "\n"; // 11

        // 11. 最短 lambda: []{}
        auto empty = [] {};
        std::cout << "shortest lambda []{} works!\n";
    }
} // namespace ns1
// ============================================================
// Main
// ============================================================

namespace ns2 {
    // C++17 规定，Lambda 表达式默认就是隐式的 constexpr（前提是它的函数体满足 constexpr
    // 的要求）。这意味着我们可以在编译期调用 Lambda，比如在 constexpr 函数里使用 Lambda、把 Lambda
    // 当作模板参数传递。这极大地增强了 C++ 的元编程能力。

    // C++17: constexpr 函数中可以使用 Lambda
    constexpr auto get_square_lambda() {
        // 这个 Lambda 满足 constexpr 条件
        return [](int n) { return n * n; };
    }
    void test() {
        // 1. 在静态断言中使用 Lambda
        auto is_even = [](int n) { return n % 2 == 0; };
        static_assert(is_even(4), "4应该是偶数");
        static_assert(!is_even(5), "5不应该是偶数");
        std::cout << "静态断言测试通过！" << std::endl;

        // 2. 在模板参数中使用 Lambda
        constexpr auto add = [](int a, int b) { return a + b; };
        // 使用编译期计算的结果作为模板参数
        std::array<int, add(10, 5)> arr{}; // 创建大小为15的数组
        static_assert(arr.size() == 15);

        // 3. 在 constexpr if 中使用
        constexpr int value = 42;
        if constexpr ([](int n) { return n > 0; }(value)) {
            std::cout << "value是正数" << std::endl;
        }

        // 4. 在编译期计算结果
        auto square = get_square_lambda();
        constexpr int result = square(5);
        std::cout << "5 的平方是 (编译期计算): " << result << std::endl;
        static_assert(result == 25);

        // 5. 也可以用于运行期
        int x = 10;
        std::cout << "10的平方是" << square(x) << "\n";
        // constexpr int result2 = square(x); // Constexpr variable 'result2' must be initialized by
        // a constant expressionclang(constexpr_var_requires_const_init)

        //  当 lambda 捕获列表为空 、函数体无运行时操作时，满足 C++17 隐式 constexpr
        // 的条件，如果自定义的lambda表达式捕获了外部变量，这样会影响到该Lambda的属性，比如：

        // 如果捕获的是编译期常量，并在函数体内使用，这个Lambda不是constexpr
        // 如果捕获的是编译期常量，但在函数体中没有使用它，Lambda 仍是 constexpr
        // 如果捕获的是运行时变量，并在函数体内使用，这个Lambda不是constexpr
        // 如果捕获的是运行时变量，但在函数体中没有使用它，Lambda 仍是 constexpr
        // 如果没有捕获外部变量，但是函数体包含非法操作（如 IO 操作、static
        // 局部变量、try/catch、new等 ），也不是 constexpr
    }

    void test2() {
        using namespace std;
        int base = 10086;

        // 如果捕获的是运行时变量，但在函数体中没有使用它，Lambda 仍是 constexpr
        auto add1 = [base](int a) { return a; };
        if constexpr (add1(4) == 4) {
            cout << "add1 ok..." << endl;
        }

        // 如果捕获的是运行时变量，并在函数体内使用，这个Lambda不是constexpr
        auto add2 = [base](int a) { return a + base; };
        // if constexpr (add2(4) == (10086 + 4)) { // error
        //     cout << "add2 ok..." << endl;
        // }

        constexpr int count = 9;
        // 如果捕获的是编译期常量，但是没有使用, 这个Lambda是constexpr
        auto add3 = [count](int a) { return a; };
        if constexpr (add3(9)) {
            cout << "ok..." << endl;
        }

        // 如果捕获的是编译期常量，并且在函数体内使用, 这个Lambda不是constexpr
        auto add4 = [count](int a) { return a + count; };
        // if constexpr (add4(9)) { // error
        //     cout << "ok..." << endl;
        // }

        auto bad1 = [](int a) {
            static int x = 0; // constexpr禁止static局部变量
            return x++ + a;
        };
        // static_assert(bad1(12)); // error
    }
} // namespace ns2
namespace ns3 {
    //     3. 捕获 *this 的拷贝
    // C++17 引入了 [*this] 捕获方式。它不是捕获指针，而是把当前对象（整个实体）复制一份传给
    // Lambda。这样做的好处是 Lambda 内部持有的是对象的副本，即使外部的原对象被销毁了，Lambda
    // 依然可以安全地使用副本中的数据。这是线程安全的。

    class Worker {
    public:
        explicit Worker(std::string name) : m_name(std::move(name)) {}

        // 模拟一个耗时任务，返回一个 Lambda
        std::function<void()> get_task_old_style() {
            // C++11/14 风格：捕获 this 指针
            // 危险！如果 Worker 对象在 task 执行前被销毁，this 就悬空了
            return
                [this]() { std::cout << "[Old Style] My name is: " << this->m_name << std::endl; };
        }

        std::function<void()> get_task_new_style() {
            // C++17 风格：捕获 *this (对象副本)
            // 安全！Lambda 内部持有对象的拷贝，与原对象解耦
            return [*this]() { std::cout << "[New Style] My name is: " << m_name << std::endl; };
        }

    private:
        std::string m_name;
    };

    void test() {
        {
            auto task = Worker{ "alice" }.get_task_new_style();
            // 此时 Worker("Alice") 临时对象已经销毁了
            // 但因为是值捕获副本，task 依然可以安全调用
            task();
        }
        {
            auto task1 = Worker("Bob").get_task_old_style();
            // 此时 Worker("Bob") 临时对象已经销毁了
            // task1();
            Worker {"Hello, world"};
            task1();
        }
    }
} // namespace ns3

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "C++17 Lambda Expressions -- New Features Demo\n";
    std::cout << "==============================================\n";
    std::cout << "__cplusplus = " << __cplusplus << "\n\n";

    ns1::test_lambda_17_this();
    ns1::test_lambda_17_constexpr();

    std::cout << "\nDone!\n";

    ns2::test();
    ns2::test2();

    ns3::test();
    system("pause");
    return 0;
}
