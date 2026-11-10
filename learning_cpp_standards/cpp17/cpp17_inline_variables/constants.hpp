// 2. 内联变量应用场景
// 2.1 全局常量和变量
// 最直接的用法：把常用的常量放在头文件里。

#pragma once

// 这些都可以安全地放在头文件中！
#include <iostream>
#include <string>
inline const int MAX_BUFFER_SIZE = 1024;
inline const std::string APP_NAME = "HELLO CPP";
inline const double PI = 3.141592653589793;

// 内联变量不一定非得是 const
inline int active_user = 0; // 非 const 也可以！

// 这两种方式在 C++17 中等效：
inline const int SIZE = 100;
constexpr int SIZE2 = 100; // 自动是 inline 的
inline constexpr int SIZE3 = 100; // 可以同时使用：inline constexpr（有些编译器可能需要）

// 内联变量与 constexpr 的关系：
// constexpr 变量隐式具有 inline 属性（从 C++17 开始）
// constexpr 强调编译期常量，inline 强调单一定义
// 可以同时使用：inline constexpr（有些编译器可能需要）

// 2.2 类的静态成员
// 内联变量最酷的应用之一：类内初始化静态成员。
class Settings {
public:
    // 传统方式：需要外部定义
    static int screen_width;
    // C++17 方式：可以直接初始化！
    inline static int screen_height = 1080;
    // const 也可以
    inline static const std::string version = "1.0.0";
    // constexpr 自动是 inline 的
    static constexpr int max_connections = 100; // C++17 起，这也是内联的！
};

// 2.3 模板变量
// C++17 还允许创建内联模板变量：
template<typename T>
inline T default_value = T{}; // 每个类型 T 都有自己的默认值
// template<typename T>：声明这是一个模板，T是类型参数
// inline：该关键字允许在多个翻译单元中定义，但是编译器会合并所有定义，保证只有一个实体存在
// T default_value：变量名是default_value，类型是模板参数T
// = T()：使用值初始化创建默认值

// 3. 在单例模式中使用内联变量
// 下面我们先写一个传统单例模式：
class Singleton {
public:
    static Singleton &getInstance() {
        if (!instance) {
            instance = new Singleton();
        }
        return *instance;
    }

private:
    static Singleton *instance;
    Singleton() {}
};
// 上面的代码不仅需要两个文件.h /.cpp，而且还存在线程安全问题，多线程同时调用 getInstance()
// 会创建多个实例，破坏单例。
// 线程 A：instance == nullptr，进入 if 块，开始执行new
// 线程 B：CPU 切换，此时instance还没被赋值，读到依然是nullptr，同样进入 if最终new
// 两次，产生两个对象，单例失效。

// 下面是使用内联变量的单例：解决了静态成员变量必须在
//     .cpp 文件中定义一次的麻烦，头文件可以直接包含。并且还解决了线程安全问题。
// 版本1
#include <mutex>
class Singleton2 {
public:
    static Singleton2 &getInstance() {
        if (!instance) {
            std::lock_guard<std::mutex> lock{ mtx };
            if (!instance) {
                instance = new Singleton2{};
            }
        }
        return *instance;
    }

    // 删除拷贝构造、移动和赋值操作（单例模式的常规操作）
    Singleton2(const Singleton2 &) = delete; // 删除拷贝构造
    Singleton2(Singleton2 &&) = delete; // 删除移动构造
    Singleton2 &operator=(const Singleton2 &) = delete; // 删除拷贝赋值
    Singleton2 &operator=(Singleton2 &&) = delete; // 删除移动赋值

private:
    inline static Singleton2 *instance = nullptr;
    inline static std::mutex mtx;
    Singleton2() {}
};
// 版本2
class Singleton3 {
public:
    // 类内只能声明自身类型的静态对象；定义需要等类完整后再写。
    // Singleton3 在类定义的闭合大括号 } 之前还属于不完整类型，因此不能在类内直接定义类型为 Singleton3 的对象：
    static Singleton3 instance;
    // 删除拷贝构造、移动和赋值操作（单例模式的常规操作）
    Singleton3(const Singleton3 &) = delete;
    Singleton3 &operator=(const Singleton3 &) = delete;
    Singleton3(Singleton3 &&) = delete;
    Singleton3 &operator=(Singleton3 &&) = delete;

private:
    Singleton3() = default;
    ~Singleton3() = default;
};
// C++17 的 inline 变量可在头文件中定义，并允许多个翻译单元包含此定义。
inline Singleton3 Singleton3::instance{};


// 最后我们来总结一下使用内联变量的核心优势：

// 简化代码：头文件中直接定义，不需要外部（源文件中）定义
// 提高可维护性：相关代码集中在一起（在一个文件中）
// 减少错误：避免忘记外部定义导致的链接错误
// 增强可读性：类的静态成员可以直接设置初始值，一步到位
