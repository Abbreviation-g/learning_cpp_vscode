#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
// https://subingwen.cn/cpp/for/
// 基于范围的for循环

namespace for_range_ns {
    using namespace std;
    // 1. for循环新语法
    // 在介绍新语法之前，先来看一个使用迭代器遍历容器的例子：
    void test1() {
        vector<int> t{ 1, 2, 3, 4, 5, 6 };
        for (auto it = t.begin(); it != t.end(); ++it) {
            cout << *it << " ";
        }
        cout << endl;

        for (auto value : t) {
            cout << value << " ";
        }
        cout << endl;
    }
    void test2() {
        // 在上面的例子中，是将容器中遍历的当前元素拷贝到了声明的变量value中，
        // 因此无法对容器中的元素进行写操作，如果需要在遍历过程中修改元素的值，需要使用引用。
        vector<int> t{ 1, 2, 3, 4, 5, 6 };
        cout << "遍历修改之前的容器: ";
        for (auto &value : t) {
            cout << value++ << " ";
        }
        cout << endl << "遍历修改之后的容器: ";

        // 对容器的遍历过程中，如果只是读数据，不允许修改元素的值，
        // 可以使用const定义保存元素数据的变量，在定义的时候建议使用const auto &，这样相对于const auto效率要更高一些。
        for (const auto &value : t) {
            cout << value << " ";
        }
        cout << endl;
    }
} // namespace for_range_ns

namespace for_range_ns2 {
    using namespace std;
    // 2.1 关系型容器
    // 使用基于范围的for循环有一些需要注意的细节，先来看一下对关系型容器map的遍历：
    void test1() {
        map<int, string> m{ { 1, "lucy" }, { 2, "lily" }, { 3, "tom" } };

        // 基于范围的for循环方式
        // 使用基于范围的for循环遍历关联性容器，auto自动推导出的类型是容器中的value_type，
        // 相当于一个对组（std::pair）对象，提取键值对的方式如下：
        for (auto &it : m) {
            cout << "id: " << it.first << ", name: " << it.second << endl;
        }
        for (const auto &it : m) {
            cout << "id: " << it.first << ", name: " << it.second << endl;
        }

        // 普通的for循环方式
        // 使用普通的for循环方式（基于迭代器）遍历关联性容器，
        // auto自动推导出的是一个迭代器类型，需要使用迭代器的方式取出元素中的键值对（和指针的操作方法相同）：
        for (auto it = m.begin(); it != m.end(); ++it) {
            cout << "id: " << it->first << ", name: " << it->second << endl;
        }
    }
    // 2.2 元素只读
    void test2() {
        // 通过对基于范围的for循环语法的介绍可以得知，在for循环内部声明一个变量的引用就可以修改遍历的表达式中的元素的值，
        // 但是这并不适用于所有的情况，对应set容器来说，内部元素都是只读的，这是由容器的特性决定的，因此在for循环中auto&会被视为const auto & 。
        set<int> st{ 1, 2, 3, 4, 5, 6 };
        for (auto &item : st) {
            // const int*
            // cout << item++ << endl; // error, 不能给常量赋值
        }
        // 在遍历关联型容器时也会出现同样的问题，基于范围的for循环中，虽然可以得到一个std::pair引用，但是我们是不能修改里边的first值的，也就是key值。
        map<int, string> m{ { 1, "lucy" }, { 2, "lily" }, { 3, "tom" } };

        for (auto &item : m) {
            // item.first 是一个常量
            // cout << "id: " << item.first++ << ", name: " << item.second << endl; // error
        }
    }
    // 2.3 访问次数
    vector<int> v{ 1, 2, 3, 4, 5, 6 };
    vector<int> &getRange() {
        cout << "get vector range..." << endl;
        return v;
    }
    void test3() {
        for (auto val : getRange()) {
            cout << val << " ";
        }
        cout << endl;
    }
} // namespace for_range_ns2
int main() {
    for_range_ns::test1();
    for_range_ns::test2();
    for_range_ns2::test3();
}