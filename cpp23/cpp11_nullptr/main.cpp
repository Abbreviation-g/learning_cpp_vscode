#include <iostream>

// https://subingwen.cn/cpp/nullptr/
// 指针空值类型 - nullptr

// 通过输出的结果可以看出，nullptr 无法隐式转换为整形，但是可以隐式匹配指针类型。在 C++11 标准下，相比 NULL 和 0，使用 nullptr
// 初始化空指针可以令我们编写的程序更加健壮。
namespace test1_ns {
    using namespace std;
    void func(char *p) {
        cout << "void func(char *p)" << endl;
    }
    void func(int p) {
        cout << "void func(int p)" << endl;
    }

    void test() {
        int *ptr1 = NULL;
        char *ptr2 = NULL;
        double *ptr3 = NULL;
        void *ptr4 = NULL;
        // int* ptr5 = ptr4; // Cannot initialize a variable of type 'int *' with an lvalue of type 'void *'clang(init_conversion_failed)

        func(10); // void func(int p)
        // func(NULL); // error: call of overloaded 'func(NULL)' is ambiguous
        func(nullptr); // void func(char *p)
    }

} // namespace test1_ns
int main() {
    test1_ns::test();
}