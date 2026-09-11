#include <iostream>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>
// https://subingwen.cn/cpp/template/
// 模板的优化

// 1. 模板的右尖括号
// 在泛型编程中，模板实例化有一个非常繁琐的地方，那就是连续的两个右尖括号（>>）
// 会被编译器解析成右移操作符，而不是模板参数表的结束。
// 我们先来看一段关于容器遍历的代码，在创建的类模板Base中提供了遍历容器的操作函数traversal():
namespace dual_ns {
    using namespace std;
    template <typename T>
    class Base {
    public:
        void traversal(T &t) {
            auto it = t.begin();
            for (; it != t.end(); ++it) {
                cout << *it << " ";
            }
            cout << endl;
        }
    };
    void test() {
        vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        Base<vector<int>> b;
        b.traversal(v);
    }

    template <typename T>
    class Container {
    public:
        void print(const T &t) {
            for (auto it = t.begin(); it != t.end(); it++) {
                cout << it->first << ", " << it->second << "; ";
            }
        }
    };
    void test2() {
        map<int, const char *> mp;
        mp.insert(make_pair(1, "aa"));
        mp.insert(make_pair(2, "bb"));

        Container<map<int, const char *>> container;
        container.print(mp);
    }
} // namespace dual_ns

// 2. 默认模板参数
// 在C++98/03标准中，类模板可以有默认的模板参数：
// 但是不支持函数的默认模板参数，在C++11中添加了对函数模板默认参数的支持:
namespace typename_ns {
    template <typename T = int, T t = 200>
    class Test {
    public:
        void operator()() {
            std::cout << "current value: " << t << std::endl;
        }
    };
    // 默认的函数参数不可用于类型推导
    template<typename T=long,typename U =int>
    void mytest(T t='A', U u='B'){
        std::cout << "t: "<<t <<", u: " << u<< std::endl;
    }
    template<typename R,typename T,typename U>
    R add_func(T t, U u){
        return t+u;
    }

    void test_func(){
        // 当所有模板参数都有默认参数时，函数模板的调用如同一个普通函数。
        // 但对于类模板而言，哪怕所有参数都有默认参数，在使用时也必须在模板名后跟随<>来实例化。
        Test<>{}(); //  current value: 200
        mytest(); // t: 65, u: 66

        Test<int*, nullptr>{}(); // current value: 0
        mytest("c", 'C'); // t: c, u: C
        mytest<int>('c', 'C');  // t: 99, u: C
        mytest<char>('c', 'C');  // t: c, u: C
        mytest<int, char>('c', 'C'); // t: 99, u: C
        mytest<char, int>('c', 'C');  // t: c, u: 67
        mytest<const char*, int>("c", 'C'); // t: c, u: 67

        auto ret1 = add_func<double>(1,4);
        std::cout << ret1<< std::endl; // 5
        auto ret2 = add_func<int>(1,4);
        std::cout << ret2 << std::endl; // 5
        auto ret3 = add_func<int, long, long>(1,4);
        std::cout << ret3 << std::endl; // 5
        
    }
} // namespace typename_ns
int main() {
    dual_ns::test();
    dual_ns::test2();

    typename_ns::test_func();
}