// https://subingwen.cn/cpp/move-forward/
// 转移和完美转发
#include <iostream>
#include <list>
#include <utility>

// 1. move
// 在C++11添加了右值引用，并且不能使用左值初始化右值引用，
// 如果想要使用左值初始化一个右值引用需要借助std::move()函数，
// 使用std::move方法可以将左值转换为右值。
// 使用这个函数并不能移动任何东西，而是和移动构造函数一样都具有移动语义，
// 将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存拷贝。

// 从实现上讲，std::move基本等同于一个类型转换：static_cast<T&&>(lvalue);，函数原型如下:
// template<class _Ty>
// _NODISCARD constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) _NOEXCEPT
// {	// forward _Arg as movable
//     return (static_cast<remove_reference_t<_Ty>&&>(_Arg));
// }

namespace move_ns {
    class Test {
    public:
        // Test(const Test &a) = delete;
        Test() : m_num(new int(100)) {
            std::cout << "constructor Test():m_num(new int(100)); " << "\n";
            std::printf("m_num的地址: %p\n", m_num);
        }
        // 拷贝构造
        Test(const Test &a) : m_num(new int(*a.m_num)) {
            std::cout << "copy constructor Test(const Test& a):m_num(new int(*a.m_num))" << "\n";
        }
        // 移动构造(复用另外一个对象里的资源(堆内存)) , m_num 浅拷贝
        Test(Test &&a) : m_num(a.m_num) {
            // a对象的指针指向空
            a.m_num = nullptr;
            std::cout << "move constructor Test(Test&& a): m_num(a.m_num)" << "\n";
        }
        ~Test() {
            // if (m_num != nullptr) {
            std::cout << "destructor " << std::endl;
            delete m_num;
            m_num = nullptr;
            // }
        }
        // 移动赋值函数
        Test &operator=(Test &&t) noexcept {
            if (this != &t) {
                delete m_num;
                m_num = t.m_num;
                t.m_num = nullptr;
            }
            return *this;
        }
        // 拷贝赋值函数
        Test &operator=(const Test &t) {
            if (this != &t) {
                int *new_num = t.m_num == nullptr ? nullptr : new int(*t.m_num);
                delete m_num;
                m_num = new_num;
            }
            return *this;
        }
        int *m_num;
    };
    void test1() {
        Test t;
        // Test && v1 = t;          // error
        Test &&v2 = std::move(t); // ok
    }
    void test2() {
        // 如果不使用std::move，拷贝的代价很大，性能较低。
        //  使用move几乎没有任何代价，只是转换了资源的所有权。
        //  如果一个对象内部有较大的堆内存或者动态数组时，
        //  使用move()就可以非常方便的进行数据所有权的转移。
        //  另外，我们也可以给类编写相应的移动构造函数（T::T(T&& another)）和
        //  和具有移动语义的赋值函数（T& T::operator=(T&& rhs)），
        //  在构造对象和赋值的时候尽可能的进行资源的重复利用，因为它们都是接收一个右值引用参数。
        std::list<std::string> ls1{ "hello", "world", "cpp" };
        std::list<std::string> ls2 = ls1; // 需要拷贝
        std::list<std::string> ls3 = std::move(ls1); // 移动构造
    }
} // namespace move_ns
// 2. forward
//  右值引用类型是独立于值的，一个右值引用作为函数参数的形参时，
//  在函数内部转发该参数给内部其他函数时，它就变成一个左值，
//  并不是原来的类型了。
// 如果需要按照参数原来的类型转发到另一个函数，可以使用C++11提供的std::forward()函数，
// 该函数实现的功能称之为完美转发。
namespace forward_ns {
    // 函数原型
    // template <class T> T&& forward (typename remove_reference<T>::type& t) noexcept;
    // template <class T> T&& forward (typename remove_reference<T>::type&& t) noexcept;

    // // 精简之后的样子
    // std::forward<T>(t);

    // 当T为左值引用类型时，t将被转换为T类型的左值
    // 当T不是左值引用类型时，t将被转换为T类型的右值
    using namespace std;
    template <typename T>
    void printValue(T &t) {
        cout << "l-value: " << t << endl;
    }

    template <typename T>
    void printValue(T &&t) {
        cout << "r-value: " << t << endl;
    }

    template <typename T>
    void testForward(T &&v) {
        printValue(v);
        printValue(std::move(v));
        printValue(std::forward<T>(v));
        cout << "\n";
    }
    void test() {
        testForward(520);
        // l-value: 520
        // r-value: 520
        // r-value: 520
        int num = 1314;
        testForward(num);
        // l-value: 1314
        // r-value: 1314
        // l-value: 1314
        testForward(forward<int>(num));
        // l-value: 1314
        // r-value: 1314
        // r-value: 1314
        testForward(forward<int &>(num));
        // l-value: 1314
        // r-value: 1314
        // l-value: 1314
        testForward(forward<int &&>(num));
        // l-value: 1314
        // r-value: 1314
        // r-value: 1314

        // testForward(520);函数的形参为未定引用类型T&&，实参为右值，初始化后被推导为一个右值引用
        // printValue(v);已命名的右值v，编译器会视为左值处理，实参为左值
        // printValue(move(v));已命名的右值编译器会视为左值处理，通过move又将其转换为右值，实参为右值
        // printValue(forward<T>(v));forward的模板参数为右值引用，最终得到一个右值，实参为``右值`
        
        // testForward(num);函数的形参为未定引用类型T&&，实参为左值，初始化后被推导为一个左值引用
        // printValue(v);实参为左值
        // printValue(move(v));通过move将左值转换为右值，实参为右值
        // printValue(forward<T>(v));forward的模板参数为左值引用，最终得到一个左值引用，实参为左值
        
        // testForward(forward<int>(num));forward的模板类型为int，最终会得到一个右值，函数的形参为未定引用类型T&&被右值初始化后得到一个右值引用类型
        // printValue(v);已命名的右值v，编译器会视为左值处理，实参为左值
        // printValue(move(v));已命名的右值编译器会视为左值处理，通过move又将其转换为右值，实参为右值
        // printValue(forward<T>(v));forward的模板参数为右值引用，最终得到一个右值，实参为右值
        
        // testForward(forward<int&>(num));forward的模板类型为int&，最终会得到一个左值，函数的形参为未定引用类型T&&被左值初始化后得到一个左值引用类型
        // printValue(v);实参为左值
        // printValue(move(v));通过move将左值转换为右值，实参为右值
        // printValue(forward<T>(v));forward的模板参数为左值引用，最终得到一个左值，实参为左值
        
        // testForward(forward<int&&>(num));forward的模板类型为int&&，最终会得到一个右值，函数的形参为未定引用类型T&&被右值初始化后得到一个右值引用类型
        // printValue(v);已命名的右值v，编译器会视为左值处理，实参为左值
        // printValue(move(v));已命名的右值编译器会视为左值处理，通过move又将其转换为右值，实参为右值
        // printValue(forward<T>(v));forward的模板参数为右值引用，最终得到一个右值，实参为右值
    }
} // namespace forward_ns
int main() {
    forward_ns::test();
}