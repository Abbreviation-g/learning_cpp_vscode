#include <cstdio>
#include <iostream>
#include <new>
#include <utility>

// https://subingwen.cn/cpp/nullptr/
// 指针空值类型 - nullptr

// 通过输出的结果可以看出，nullptr 无法隐式转换为整形，但是可以隐式匹配指针类型。在 C++11 标准下，相比 NULL 和 0，使用 nullptr
// 初始化空指针可以令我们编写的程序更加健壮。
namespace test1_ns {
    using namespace std;
    // 返回类型后置: auto 占位 + -> 返回类型, 非模板函数也可以使用, 与 void func(char *p) 等价
    auto func(const char *p) -> void {
        cout << "func(const char *p)" << endl;
    }
    auto func(const char *&p) -> void {
        cout << "func(const char *&p)" << endl;
    }
    auto func(int &p) -> void {
        cout << "func(int &p)" << endl;
    }
    auto func(const int &p) -> void {
        cout << "func(const int &p)" << endl;
    }
    auto func(const int *&p) -> void {
        cout << "func(const int *&p)" << endl;
    }

    void test() {
        const int *ptr1 = NULL;
        char *ptr2 = NULL;
        double *ptr3 = NULL;
        void *ptr4 = NULL;
        // int* ptr5 = ptr4; // Cannot initialize a variable of type 'int *' with an lvalue of type 'void *'clang(init_conversion_failed)

        func(10); // func(const int &p)
        func("xxx"); // func(const char *p)
        func(ptr1); // func(const int *&p)
        // func(NULL); // error: call of overloaded 'func(NULL)' is ambiguous
        func(nullptr); // func(const char *p)

        // 调用 func(const char *&p): 非 const 左值引用只能绑定到可修改的左值,
        // 因此需要一个具名的 const char * 变量(不能是字符串字面量或临时对象)。
        const char *ptr5 = "yyy";
        // func(ptr5); // error: call of overloaded 'func(const char*&)' is ambiguous
        // 原因: func(const char *) 与 func(const char *&) 对 const char* 左值都是精确匹配,
        //       标准中没有"引用绑定优于按值传递"的规则, 所以直接调用无法确定唯一最佳匹配。
        // 做法: 用函数指针显式指定形参类型, 由目标类型反选重载。
        void (*fref)(const char *&) = func;
        fref(ptr5); // func(const char *&p)
    }

    class Test {
    public:
        Test() : m_data(new int{ 11 }) {
            std::cout << "Test() : m_data(new int{ 11 })" << "\n";
        }
        explicit Test(int value) : m_data(new int{ value }) {
            std::cout << "Test(int value) : m_data(new int{ value })" << "\n";
        }
        Test(const Test &t) : m_data(new int{ *t.m_data }) {
            std::cout << "Test(const Test &t) : m_data(new int{ *t.m_data })" << "\n";
        }
        Test(Test &&t) noexcept : m_data(t.m_data) {
            std::cout << "Test(Test &&t) : m_data(t.m_data)" << "\n";
            t.m_data = nullptr;
        }
        ~Test() {
            std::cout << "~Test()\t" << m_data << "\n";
            delete m_data;
        }
        auto operator=(Test &&t) noexcept -> Test & {
            std::cout << "Test &operator=(Test &&t) noexcept" << "\n";
            if (this != &t) {
                delete m_data;
                m_data = t.m_data;
                t.m_data = nullptr;
            }
            return *this;
        }
        auto operator=(const Test &t) noexcept -> Test & {
            std::cout << "Test &operator=(const Test &t) noexcept" << "\n";
            if (this != &t) {
                delete m_data;
                m_data = t.m_data == nullptr ? nullptr : new (std::nothrow) int(*t.m_data);
            }
            return *this;
        }
        auto operator=(int value) noexcept -> Test & {
            std::cout << "Test& operator=(int value) noexcept" << "\n";
            delete m_data;
            m_data = new (std::nothrow) int{ value };
            return *this;
        }
        explicit operator int() const {
            return *m_data;
        }
        explicit operator int *() const {
            return m_data;
        }
        auto operator()() const -> int {
            return *m_data;
        }
        auto operator()(int x) const -> int {
            int back = *m_data;
            *m_data = x;
            return back;
        }
        int *m_data;
    };
    void test2() {
        std::cout << "-------\n";
        Test t1{ 19 }; // Test(int value) : m_data(new int{ value })
        Test t2; // Test() : m_data(new int{ 11 })
        t2 = t1; // Test &operator=(const Test &t) noexcept
        std::cout << *t1.m_data << "\t" << t1.m_data << std::endl; // 19      0x20346b28ea0
        std::cout << *t2.m_data << "\t" << t2.m_data << std::endl; // 19      0x20346b38dc0
        t2 = std::move(t1); // Test &operator=(Test &&t) noexcept
        std::cout << *t2.m_data << "\t" << t2.m_data << std::endl; // 19      0x20346b28ea0
        std::cout << "-------\n";
        // ~Test() 0x20346b28ea0
        // ~Test() 0
    }
    void test3() {
        std::cout << "-------\n";

        Test t3{ 1 }; // Test(int value) : m_data(new int{ value })
        t3 = 2; // Test& operator=(int value) noexcept
        std::cout << "-------\n";
        //~Test() 0x1df48cb8ea0
    }
    void test4() {
        Test t{ 10086 };
        int value = static_cast<int>(t);
        int *ptr = static_cast<int *>(t);
        std::printf("value: %d, ptr: %p\n", value, ptr);
        int v2 = t();
    }
} // namespace test1_ns
int main() {
    test1_ns::test();
    // test1_ns::test2();
    test1_ns::test3();
    test1_ns::test4();
}