#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

// https://subingwen.cn/cpp/using/
//  using的使用
// using语法和typedef一样，并不会创建出新的类型，它们只是给某些类型定义了新的别名。using相较于typedef的优势在于定义函数指针别名时看起来更加直观，并且可以给模板定义别名。
// 要在子类中调用父类的 print，最直接的方法是：
// Base::print();
// 如果是想让父类的同名函数在子类作用域中可见，再用：
// using Base::print;
namespace inherit_ns {
    class Base {
    public:
        virtual void print() {
            std::cout << "class Base print()" << std::endl;
        }
    };
    class Derived : public Base {
    public:
        using Base::print;
        void print() override {
            Base::print();
            std::cout << "class Derived print()" << std::endl;
        }
        void print2() {
        }
    };

    void test1() {
        Base{}.print();
        Derived{}.print();
    }
} // namespace inherit_ns

namespace typedef_using_ns {
    // 使用typedef 定义别名
    typedef int *(*func_int_ptr)(int, double);

    // 使用using定义别名
    using func_int_ptr2 = int *(*)(int, double);

    static int global_number = 10;
    void call_func_int_ptr(func_int_ptr fip) {
        int *ret = fip(1, 1.0);
        std::cout << "test_func_int_ptr: " << *ret << std::endl;
    }
    void call_func_int_ptr2(func_int_ptr2 fip) {
        int *ret = fip(1, 1.0);
        std::cout << "test_func_int_ptr2: " << *ret << std::endl;
    }
    void test1() {
        auto lambda1 = [](int i, double d) {
            global_number = global_number + i + static_cast<int>(d);
            return &global_number;
        };
        call_func_int_ptr(lambda1);
        call_func_int_ptr2(lambda1);
        func_int_ptr fip = lambda1;
        func_int_ptr2 fip2 = lambda1;
        call_func_int_ptr(fip);
        call_func_int_ptr(fip2);
        call_func_int_ptr2(fip);
        call_func_int_ptr2(fip2);
        fip(3, 4.0);
        (*fip)(3, 4.0);

        typedef long double ld;
        using ld2 = long double;

        ld a = 0.0L;
        ld2 b = 0.0L;
    }
} // namespace typedef_using_ns

// 2. 模板的别名
namespace using_template_ns {
    namespace s = std;

    // 外敷类
    template <typename T>
    struct MyMap {
        typedef std::map<int, T> map_type;
    };
    template <typename T>
    class Container {
    public:
        void print(const T &t) {
            for (auto it = t.begin(); it != t.end(); it++) {
                std::cout << it->first << ", " << it->second << "; ";
            }
            std::cout << "\n";
        }
    };
    void test1() {
        MyMap<std::string>::map_type m;
        m.insert(std::make_pair(1, "T2 &&y"));
        Container<decltype(m)>{}.print(m);

        MyMap<const char *>::map_type m2;
        m2.insert(std::make_pair(2, "T2 &&yxxx"));
        Container<decltype(m2)>{}.print(m2);

        MyMap<long double>::map_type m3;
        m3.insert(std::make_pair(3, 3.1L));
        Container<decltype(m3)>{}.print(m3);
    }

    // 在C++11中，新增了一个特性就是可以通过使用using来为一个模板定义别名，对于上面的需求可以写成这样：
    template <typename T>
    using mymap = std::map<int, T>;
    void test2() {
        mymap<std::string> m;
        m.insert(std::make_pair(1, "T2 &&y"));
        Container<decltype(m)>{}.print(m);

        mymap<const char *> m2;
        m2.insert(std::make_pair(2, "T2 &&yxxx"));
        Container<decltype(m2)>{}.print(m2);

        mymap<long double> m3;
        m3.insert(std::make_pair(3, 3.1L));
        Container<decltype(m3)>{}.print(m3);
    }
} // namespace using_template_ns
int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        std::cout << "argv: " << *(argv + i) << std::endl;
    }
    inherit_ns::test1();
    typedef_using_ns::test1();

    using_template_ns::test1();
    using_template_ns::test2();
}
