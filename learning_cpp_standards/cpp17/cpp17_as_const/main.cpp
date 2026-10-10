// https://subingwen.cn/cpp/as_const/
// 将左值引用转换为常量引用 - std::as_const
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
namespace ns1 {
    //     1. std::as_const 概述
    // 在 C++ 编程中，我们经常需要重载成员函数，分别提供 const 版本和非 const
    // 版本，以便在对象是只读（const）或可变（non-const）时采取不同的行为。最经典的例子是容器类中的下标运算符
    // operator[]：
    template <typename T>
    class MyVector {
    public:
        // 非 const 版本：用于读写
        // 为了让代码更加简洁，开发者通常希望 非 const 版本的函数能够直接调用 const 版本的函数。
        // 在 C++17 之前，要实现这种代码复用非常麻烦，因为在一个非 const 的成员函数内部，this
        // 指针的类型是 MyVector*（非 const），如果我们直接调用 operator[]，可能会导致无限递归调用非
        // const 版本。 解决方案： 我们需要显式地将 this 指针转换为 const 类型，即 static_cast<const
        // MyVector&>(*this)[index]。这行代码很长且易错。std::as_const
        // 就是为了简化这种转换而诞生的。 const 版本：用于只读
        T &operator[](size_t index) {
            std::cout << "非 const 版本：用于读写" << std::endl;
            return const_cast<T &>(static_cast<const MyVector<T> &>(*this)[index]);
        }
        // const 版本：用于只读
        const T &operator[](size_t index) const {
            std::cout << "const 版本：用于只读" << std::endl;
            return data[index];
        }

    private:
        std::vector<T> data{ 1002, 1003, 1004 };
    };

    void test1() {
        MyVector<int> mv{};
        const MyVector<int> cmv{};

        mv[1] = 90001; // 非 const 版本：用于读写// const 版本：用于只读
        int &i = mv[1]; // 非 const 版本：用于读写 // const 版本：用于只读
        i = 10086;

        // cmv[1] = 0;//error
        const int &j = cmv[1]; // const 版本：用于只读
    }
} // namespace ns1
namespace ns2 {
    template <typename T>
    class DataSource {
    public:
        // 只负责获取数据的逻辑（假设逻辑很复杂，不想写两遍）
        const T &get_value(size_t index) const {
            std::cout << "执行复杂的边界检查逻辑..." << std::endl;
            return data.at(index);
        }
        // 在非 const 函数中，想复用上面的复杂逻辑，但要带回写的权限
        T &get_value(size_t index) {
            return const_cast<T &>(std::as_const(*this).get_value(index));
        }

    private:
        std::vector<T> data{ 10, 20, 30 };
    };
    void test1() {
        DataSource<int> ds;
        // 调用非 const 版本
        ds.get_value(1) = 9999; // 修改了数据
        const DataSource<int> ds2 = std::as_const(ds);
        std::cout << "修改后: " << ds2.get_value(1) << std::endl;
    }
    // 关于return const_cast<int&>(std::as_const(*this).getValue(index));这行代码的解释：
    // std::as_const(*this) 将*this转换为 const DataStore&
    // 调用 const 版本的 getValue(index)，返回 const int&
    // const_cast 去掉 const 属性，返回 int&
    // 这里仍然需要
    // const_cast，是因为我们需要把返回值的只读属性去掉，这是安全的，因为原始对象本身就不是 const
    // 的。 如果没有 std::as_const，我们必须写成：const_cast<int&>(((const
    // DataStore&)(*this)).getValue(index))，这简直是噩梦。
} // namespace ns2
namespace ns3 {
    class Test {
    public:
        // 非 const 成员函数
        void func() {
            std::cout << "非 const 函数被调用" << std::endl;
        }

        // const 成员函数
        void func() const {
            std::cout << "const 函数被调用" << std::endl;
        }
    };
    void test1() {
        Test obj1; // 非 const 对象
        const Test obj2; // const 对象

        std::cout << "调用 obj1.func(): ";
        obj1.func(); // 输出：非 const 函数被调用

        std::cout << "调用 obj2.func(): ";
        obj2.func(); // 输出：const 函数被调用

        // 强制行为：通过 static_cast 或 std::as_const 调用 const
        std::as_const(obj1).func();
        // 或者
        static_cast<const Test &>(obj1).func();
    }
} // namespace ns3
namespace ns4 {

    // 这是一个配置管理器
    class ConfigManager {
    public:
        std::vector<int> &getSettings() {
            return settings_;
        }

    private:
        std::vector<int> settings_ = { 100, 200, 300 };
    };
    //     2.2 防止意外修改
    // 在使用 std::vector 或 std::map 时，直接传递非引用非常危险，因为 operator[] 通常是可写的。
    // 打印函数：我只想打印配置，不想改配置
    void printSettings(ConfigManager &mgr) {
        // 错误做法：
        // auto& s = mgr.getSettings();
        // s[0] = 999; // 一旦手抖加上这行，配置就被暗中篡改了，且很难排查。

        // 正确做法（防御性编程）：
        // 强制获取一个 const 视图
        const auto &readonly_settings = std::as_const(mgr.getSettings());

        std::cout << "Setting 0: " << readonly_settings[0] << std::endl;

        // readonly_settings[1] = 0; // ❌ 编译报错！拦截了非法修改
    }
    void test1() {
        ConfigManager mgr;
        printSettings(mgr);
    }

    //     2.3 配合 Range-based For 循环
    // 配合基于范围的 For
    // 循环也是std::as_const一个很帅的用法。当我们遍历一个容器的元素，并且明确地在循环体里对元素进行只读操作时可以用它。
    void test2() {
        std::vector<int> nums = { 1, 2, 3, 4 };

        // 使用 std::as_const 的写法（显得比较高端，且强制只读）
        for (auto &&item : std::as_const(nums)) {
            std::cout << item << " ";
            // item = 100; // 编译错误！item 是 const 引用
        }
        // auto&& item
        // 这是什么？这被称为转发引用或万能引用。
        // 为什么这么写？auto&&会根据初始化它的表达式的值类别（左值/右值）和 const 属性自动推导：
        // 如果右边是 T&，item 就变成 T&。
        // 如果右边是 const T&，item 就变成 const T&。
        // 如果右边是 T&&，item 就变成 T&&。

        //         为了大家你更清楚它的定位，我们对比一下常见的写法：

        // for (auto item : nums) —— 差
        // 发生拷贝。如果 vector 里存的是大对象，性能极差。

        // for (const auto& item : nums) —— 好（常用）
        // 显式地声明了只读引用。
        // 效果和 as_const 版本几乎一样，也是零拷贝且只读。

        // for (const auto& item : std::as_const(nums)) —— 更好（强调）
        // 如果不确定 nums 本身会不会在某些条件下变成非 const，使用 std::as_const 可以强制锁定来源。

        // for (auto&& item : std::as_const(nums)) —— 最佳（现代主义）
        // 结合了 auto&& 的通用性和 std::as_const
        // 的语义锁定。它告诉编译器：“给我这个容器的只读视图，并且我要用最通用的引用方式来接收元素。”
    }
} // namespace ns4
int main() {
    ns1::test1();
    ns2::test1();
    ns3::test1();
    ns4::test1();
    ns4::test2();
}