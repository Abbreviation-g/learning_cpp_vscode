#include <iostream>
#include <memory>
// https://subingwen.cn/cpp/smart_pointer/
// 智能指针的增强

namespace shared_ptr_array {
    // C++17 允许在模板参数中添加 [] 来表明这是一个数组指针，shared_ptr<T[]> 现在被编译器识别为了一种专门管理数组的智能指针。对于
    // std::shared_ptr<T>，我们通常使用 *ptr 或 ptr->，但对于 std::shared_ptr<T[]>：
    // 析构时自动调用 delete[]。
    // 提供了数组下标运算符 operator[]，可以直接像数组一样访问。
    // operator* 和 operator-> 被禁用（编译报错）。
    void test() {
        // 【关键修改】必须写成 int[] 而不是 int
        // 只有 int[] 才支持 operator[] 并且知道要用 delete[] 释放
        constexpr int len = 5;
        std::shared_ptr<int[]> arr{ new int[len]{} };
        // 赋值：现在编译器能找到 operator[] 了
        for (int i = 0; i < len; i++) {
            arr[i] = i * 10;
        }
        // 读取
        std::cout << "第一个元素: " << arr[0] << std::endl;
        std::cout << "第三个元素: " << arr[2] << std::endl;

        int x = arr[0]; // OK
        // int y = *arr; // 编译错误！不能解引用数组
        // arr->me();// 编译错误！数组没有成员访问

        // 注意事项：C++17 中，std::make_shared 依然不支持创建数组！
        // C++17 编译错误！C++20 编译通过
        // auto p = std::make_shared<int[]>(10);
    }
} // namespace shared_ptr_array
namespace weak_ptr_ns {
    void test() {
        std::weak_ptr<int[]> weak_arr;
        {
            constexpr int len = 5;
            std::shared_ptr<int[]> strong_arr(new int[len]);
            for (int i = 0; i < len; i++) {
                strong_arr[i] = i + 1;
            }
            weak_arr = strong_arr; // 弱引用关联
            if (auto temp = weak_arr.lock()) {
                std::cout << "访问数组元素: " << temp[2] << std::endl; // 输出 3
                std::cout << "引用计数: " << temp.use_count() << std::endl; // 2
            }
        } // strong_arr 离开作用域，数组被销毁
        {
            if (weak_arr.expired()) {
                std::cout << "观察的对象已经阵亡了..." << std::endl; // 观察的对象已经阵亡了...
            } else {
                std::cout << "观察的对象还活着 ! " << std::endl;
            }
            // 此时 weak_arr 已过期
            if (auto temp = weak_arr.lock()) {
                std::cout << "观察的对象还活着 !" << std::endl;
            } else {
                std::cout << "观察的对象已经阵亡了..." << std::endl; // 观察的对象已经阵亡了...
            }
        }
    }

    // 正确的多线程写法：
    void test2() {
        std::weak_ptr<int[]> wp;
        std::shared_ptr<int[]> strong_arr(new int[3]);
        wp = strong_arr;

        // 错误写法
        if (!wp.expired()) {
            // 这里依然不安全！因为在这两行代码之间，对象可能被别的线程销毁
            // auto sp = wp.lock();
        }

        // 正确且线程安全的写法
        auto sp = wp.lock(); // 1. 尝试获取所有权
        if (sp) {
            // 2. 如果 sp 不为空，说明我们成功持有了对象，
            // 且引用计数已 +1，对象在这一刻绝对不会被销毁。
            // sp->doSomething(); // 安全
        } else {
            std::cout << "对象已失效" << std::endl;
        }
    }
    // expired()：仅适用于我不想使用对象，我只是想看看它还在不在的场景。
    // lock()：适用于我想使用对象，如果在我就用，不在就算了的场景。
    // 只要我们想访问对象，必须使用 lock()，并且判断 lock()的返回值，千万不要用 expired() 预判。
} // namespace weak_ptr_ns
int main() {
    shared_ptr_array::test();
    weak_ptr_ns::test();
}