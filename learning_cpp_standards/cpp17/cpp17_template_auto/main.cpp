#include <iostream>
// https://subingwen.cn/cpp/template-auto/
// 带有 auto 类型的非类型模板参数
namespace ns1 {
    template <typename T, size_t Size>
    class FixedArray {
    private:
        T data[Size];

    public:
        int get_size() const {
            return Size;
        }
    };
    void test() {
        FixedArray<int, 10> arr1{};
        FixedArray<double, 1> arr2{};
    }

    //     在 C++17
    //     之前，当编写一个模板类或模板函数时，如果想让模板接收一个具体的值（比如一个数字、一个指针，而不是一个类型），必须显式地告诉
    //     C++ 这个值的类型。

    // C++17 允许在非类型模板参数中使用 auto
    // 关键字。这意味着编译器会自动推断传入的类型。我们不再需要关心传入的是 int, long, short 还是
    // size_t，只要它能用，编译器就会帮我们搞定。
    template <auto Value>
    class MyClass { // 类定义
    };
    // 用于函数模板
    template <auto Value>
    void myFunction() {
        // 函数实现
    }

    //     允许的类型：

    // 非类型模板参数本身（即使加了 auto）也只能接受以下类型：

    // 整形（int, char, long, bool 等）
    // 枚举类型
    // 指针类型
    // 左值引用类型
    // std::nullptr_t
    // 浮点数类型（float, double）（C++17不允许，C++20 放宽了限制，允许浮点数）。
    template <auto Value>
    struct AutoTemplate {
        static constexpr auto val = Value;
        void print() {
            std::cout << "Value: " << val << ", Type: " << typeid(val).name() << "\n";
        }
    };
    void test2() {
        AutoTemplate<42> intInstance;
        AutoTemplate<'A'> charInstance;
        AutoTemplate<true> doubleInstance;

        intInstance.print();
        charInstance.print();
        doubleInstance.print();
    }
} // namespace ns1

namespace ns2 {
    // 2.1 模版特化

    // 主模板
    template <auto Value>
    struct ValueProcessor {
        static void process() {
            std::cout << "Generic processing for value: " << Value << std::endl;
        }
    };
    // 整型特化
    template <int Value>
    struct ValueProcessor<Value> {
        static void process() {
            std::cout << "Integral processing for value: " << Value << " (square: " << Value * Value
                      << ")" << std::endl;
        };
    };
    // long 整型特化
    template <long Value>
    struct ValueProcessor<Value> {
        static void process() {
            std::cout << "Long processing for value: " << Value << " (square: " << Value * Value
                      << ")" << std::endl;
        }
    };
    // 指针特化
    template <auto *Ptr>
    struct ValueProcessor<Ptr> {
        static void process() {
            std::cout << "Pointer processing for pointer: " << Ptr << std::endl;
        }
    };
    // 特定值特化
    template <>
    struct ValueProcessor<42> {
        static void process() {
            std::cout << "Special processing for the answer to everything!" << std::endl;
        }
    };
    void test() {
        ValueProcessor<false>::process(); // Generic processing for value: 0
        ValueProcessor<10>::process(); // Integral processing for value: 10 (square: 100)
        ValueProcessor<10L>::process(); // Long processing for value: 10 (square: 100)
        ValueProcessor<nullptr>::process(); // Generic processing for value: nullptr
        ValueProcessor<42>::process(); // Special processing for the answer to everything!

        // 因为非类型模板参数必须是编译期常量，所以template<auto* Ptr> 中的
        // Ptr，必须在编译代码的时候就已经确定它的值，而 &x（取地址）只有当 x
        // 是静态存储期的变量时编译器才能确定。
        static int x = 100;
        ValueProcessor<&x>::process(); // Pointer processing for pointer: 0x7ff79a353000
    }
} // namespace ns2

namespace ns3 {
    // 2.2 模拟类似 Python 的 .format 或者状态机
    // 利用 C++17 的非类型模板参数这个特性，我们可以创建非常灵活的序列生成器。

    // 递归终止条件
    template <auto... Args>
    struct Sequence {};
    // 递归展开
    template <auto First, auto... Rest>
    struct Sequence<First, Rest...> {
        void print() {
            std::cout << First << " ";
            Sequence<Rest...> next;
            next.print();
        }
    };
    // 全特化版本，处理空参数
    template <>
    struct Sequence<> {
        void print() {
            std::cout << "(End)" << std::endl;
        }
    };

    void test() {
        // 这里的参数可以是混合的类型：int, bool, char
        Sequence<1, true, 'A', 42L> seq;
        std::cout << "Sequence values: ";
        seq.print();
    }
} // namespace ns3

namespace ns4 {
    // 使用 constexpr 函数配合折叠表达式，将递归逻辑扁平化
    template <auto... Args>
    struct Sequence {
        void print() {
            std::cout << "Sequence values: ";
            // 使用折叠表达式展开参数包
            ((std::cout << Args << " "), ...);
            std::cout << "(End)" << "\n";
        }
        // 如果需要更详细的类型信息
        void print_with_types() {
            std::cout << "Sequence with types: ";
            ((std::cout << "(" << Args << ":" << typeid(Args).name() << ")"), ...);
            std::cout << "\n";
        }
    };

    void test() {
        Sequence<1, true, 'A', 42L> seq;
        seq.print(); // Sequence values: 1 1 A 42 (End)

        seq.print_with_types(); // Sequence with types: (1:i)(1:b)(A:c)(42:l)
    }

    // 折叠表达式说明：
    // ((std::cout << Args << " "), ...) ：一元右折叠，使用逗号运算符 , 作为操作符。
    // , ：连接多个表达式，确保从左到右执行顺序，返回最后一个表达式的值
    // 这是 C++17 引入的特性，可以替代复杂的递归模板展开

} // namespace ns4
int main() {
    ns1::test();
    ns1::test2();

    ns2::test();

    ns3::test();

    ns4::test();
}