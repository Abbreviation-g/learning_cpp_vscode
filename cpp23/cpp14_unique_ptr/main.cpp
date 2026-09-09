#include <iostream>
#include <memory>
// unique_ptr 在 C++14 中的优化
// https://subingwen.cn/cpp/unique_ptr_14/

namespace unique_ptr_ns {
    using namespace std;
    class Base {
    public:
        Base() = default;
        Base(int i, string s) : id(i), name(s) {
        }

    private:
        int id;
        string name;
    };
    // 定义通用的函数模板
    template <typename T, typename... Args>
    unique_ptr<T> my_make_unique_ptr(Args &&...args) {
        // (cout << ... << args) << "\n"; // 折叠表达式（C++17，推荐）
        ((cout << args << ", "),...) << "\n"; // 带分隔符的折叠表达式（C++17）

        return unique_ptr<T>{ new T{ forward<Args>(args)... } };
    }
    void test_cpp11() {
        unique_ptr<int> ptr1(new int(10));
        unique_ptr<Base> ptr2(new Base{ 1, "name" });
        auto ptr3 = move(ptr1);
        
        auto ptr4 = my_make_unique_ptr<int>(12);
        auto ptr5 = my_make_unique_ptr<Base>(22, "alice");
    }
    void test_cpp14(){
        // cpp14 可以自动创建对象
        auto ptr6 = std::make_unique<int>(2);
        auto ptr7 = std::make_unique<Base>(12, "xxx");
        // make_unique创建数组, 
        // 1.make_unique的参数为元素个数; 
        // 2.模板参数类型必须书T[]，[]不能丢; 释放内存时通过delete[]释放内存
        // 3.数组元素初始化，使用默认构造函数
        auto ptr8 = std::make_unique<int[]>(2);
        ptr8[0] = 02;
        ptr8[1] = 12;
        auto ptr9 = std::make_unique<Base[]>(2);
        ptr9[0] = Base{1,"2"};
        ptr9[0] = Base{2,"2"};
        auto ptr10 = std::make_unique<Base*[]>(2);
        ptr10[0] = new Base{1,"2"};
        ptr10[1] = new Base{1,"2"};
    }
}
int main() {
    unique_ptr_ns::test_cpp11();
    unique_ptr_ns::test_cpp14();
}