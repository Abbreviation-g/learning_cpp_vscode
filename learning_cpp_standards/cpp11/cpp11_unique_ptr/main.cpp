// https://subingwen.cn/cpp/unique_ptr/
// 独占的智能指针
#include <functional>
#include <iostream>
#include <memory>

namespace unique_ptr_ns {
    //     1. 初始化
    // std::unique_ptr是一个独占型的智能指针，它不允许其他的智能指针共享其内部的指针，可以通过它的构造函数初始化一个独占智能指针对象，但是不允许通过赋值将一个unique_ptr赋值给另一个unique_ptr。
    // std::unique_ptr不允许复制，但是可以通过函数返回给其他的std::unique_ptr，还可以通过std::move来转译给其他的std::unique_ptr，这样原始指针的所有权就被转移了，这个原始指针还是被独占的。

    using namespace std;
    class Test {
    public:
        Test() {
            cout << "construct Test..." << endl;
        }
        Test(int x) {
            cout << "construct Test, x = " << x << endl;
        }
        Test(string str) {
            cout << "construct Test, str = " << str << endl;
        }
        ~Test() {
            cout << "destruct Test ..." << endl;
        }
        void set_value(int value) {
            m_data = value;
        }
        int get_value() const {
            return m_data;
        }
        void print_value() const {
            std::cout << "value: " << m_data << "\n";
        }

    private:
        int m_data = 1000;
    };
    void test1() {
        // 通过构造函数初始化
        unique_ptr<int> ptr1{ new int{ 11 } };
        // 通过移动构造初始化
        unique_ptr<int> ptr2 = std::move(ptr1);
        // 通过reset初始化
        ptr2.reset(new int{ 12 });

        // 获取原始指针
        unique_ptr<Test> ptr3{ new Test{} };
        Test *pt = ptr3.get();
        pt->set_value(10086);
        pt->print_value();
        ptr3->set_value(10087);
        ptr3->print_value();
    }
} // namespace unique_ptr_ns
namespace unique_ptr_ns2 {
    // 2. 删除器
    // unique_ptr指定删除器和shared_ptr指定删除器是有区别的，unique_ptr指定删除器的时候需要确定删除器的类型，所以不能像shared_ptr那样直接指定删除器
    void test1() {
        std::shared_ptr<int> ptr1(new int{ 10 }, [](int *p) {
            delete p;
        });
        // std::unique_ptr<int> ptr2(new int{10}, [](int* p){delete p;}); // 编译报错

        using func_ptr = void (*)(int *);
        std::unique_ptr<int, func_ptr> ptr2(new int{ 10 }, [](int *p) {
            delete p;
        }); // func_ptr的类型和lambda表达式的类型是一致的

        //    std::unique_ptr<int, func_ptr> ptr3(new int{ 10 }, [&](int *p) {
        //         delete p;
        //     });  // 如果捕获了变量，编译时则会报错
        // 错误原因是这样的，在lambda表达式没有捕获任何外部变量时，可以直接转换为函数指针，一旦捕获了就无法转换了，如果想要让编译器成功通过编译，那么需要使用可调用对象包装器来处理声明的函数指针
        // clang-format off
        std::unique_ptr<int, std::function<void(int *)>> ptr3 { 
        new int{ 13 }, 
        [&](int *p) {
            delete p;
        } };
        // clang-format on
    }
    void test2() {
        using Test = unique_ptr_ns::Test;

        std::unique_ptr<Test[]> ptr{ new Test[3] };
        // construct Test...
        // construct Test...
        // construct Test...
        // destruct Test ...
        // destruct Test ...
        // destruct Test ...
    }
    void test3() {
        using Test = unique_ptr_ns::Test;

        std::shared_ptr<Test> ptr{ new Test[3] };
        // construct Test...
        // construct Test...
        // construct Test...
        // destruct Test ...
    }
    void test4() {
        using Test = unique_ptr_ns::Test;

        std::shared_ptr<Test[]> ptr{ new Test[3] };
        // construct Test...
        // construct Test...
        // construct Test...
        // destruct Test ...
        // destruct Test ...
        // destruct Test ...
    }
} // namespace unique_ptr_ns2
int main() {
    std::cout << "------------------------unique_ptr_ns::test1();" << "\n";
    unique_ptr_ns::test1();
    std::cout << "------------------------unique_ptr_ns2::test1();" << "\n";
    unique_ptr_ns2::test1();
    std::cout << "------------------------unique_ptr_ns2::test2();" << "\n";
    unique_ptr_ns2::test2();
    // std::cout <<"------------------------unique_ptr_ns2::test3();" << "\n";
    // unique_ptr_ns2::test3(); // test3有错误，所以test4不执行了
    std::cout << "------------------------unique_ptr_ns2::test4();" << "\n";
    unique_ptr_ns2::test4();
}