// https://subingwen.cn/cpp/noexcept/
// noexcept

#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace exception_ns1 {
    void throw_e(int i) throw(int, std::logic_error) {
        switch (i) {
        case 0:
            throw 0;
        case 1:
            throw std::invalid_argument("1111");
        case 2:
            throw std::out_of_range("222");
        case -1:
            throw -1;
        default:
            std::cout << "vaild value" << "\n";
        }
    }
    void test() {
        for (int i = 0; i < 20; i++) {
            int value = std::rand() % 11 - 5;
            try {
                throw_e(value);
            } catch (int i) {
                std::cerr << "catch " << i << "\n";
            } catch (std::logic_error e) {
                std::cerr << "catch " << e.what() << "\n";
            }
        }
    }
    // 1.2 异常接口声明
    // 为了加强程序的可读性，可以在函数声明中列出可能抛出的所有异常类型，常用的有如下三种书写方式：
    using namespace std;
    // 1.显示指定可以抛出的异常类型
    struct MyException {
        MyException(string s) : msg(s) {}
        string msg;
    };
    double divisionMethod(int a, int b) throw(MyException, int) {
        if (b == 0) {
            throw MyException("division by zero!!!");
            // throw 100;
        }
        return a / b;
    }
    void test2() {
        try {
            double v = divisionMethod(100, 0);
            cout << "value: " << v << endl;
        } catch (int e) {
            cout << "catch except: " << e << endl;
        } catch (MyException e) {
            cout << "catch except: " << e.msg << endl;
        }
    }
} // namespace exception_ns1
// noexcept 形如其名，表示其修饰的函数不会抛出异常 。
// 不过与 `throw()`动态异常声明不同的是，
// `在 C++11 中如果 noexcept 修饰的函数抛出了异常，
// 编译器可以选择直接调用 std::terminate() 函数来终止程序的运行，
// 这比基于异常机制的 throw() 在效率上会高一些`。
// 这是因为异常机制会带来一些额外开销，比如函数抛出异常，
// 会导致函数栈被依次地展开（栈解旋），
// 并自动调用析构函数释放栈上的所有对象。
namespace noexcept_ns {
    using namespace std;
    // noexcept(常量表达式);
    // 值为 true，表示函数不会抛出异常
    // 值为 false，表示有可能抛出异常这里
    // 不带常量表达式的noexcept相当于声明了noexcept（true），即不会抛出异常。

    double divisionMethod(int a, int b) noexcept(true) {
        if (b == 0) {
            cout << "division by zero!!!" << endl;
            return -1;
        }
        return a / b;
    }
    void test2() {
        try {
            double v = divisionMethod(100, 0);
            cout << "value: " << v << endl;
        } catch (int e) {
            cout << "catch except: " << e << endl;
        } catch (std::exception e) {
            cout << "catch except: " << e.what() << endl;
        }
    }
} // namespace noexcept_ns
int main() {
    std::cout << "--------" << "" << "--------" << "\n";
    exception_ns1::test();
    std::cout << "--------" << "" << "--------" << "\n";
    exception_ns1::test2();
}
