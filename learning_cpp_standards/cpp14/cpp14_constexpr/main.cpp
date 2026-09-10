#include <cstddef>
#include <iostream>
#include <ostream>
// https://subingwen.cn/cpp/constexpr-ex/
// constexpr 限制放宽
namespace const_constexpr {
    int getInput() {
        std::cout << "Please input a number: ";
        int input = -1;
        std::cin >> input;
        return input;
    }
    void test_const() {
        // 1.初始化实际
        // const 编译或者运行时
        const int num = 123; // 编译时初始化
        // num = 2; // 编译时修改值不被允许
        const int value = getInput(); // 运行时初始化

        const int *ptr = new int(num); // 运行时分配内存
        //  *ptr = 456; // 运行时修改值不被允许
        ptr = new int(value); // 运行时修改指针被允许
        delete ptr;
        int *const ptr1 = new int(num);
        *ptr1 = 456; // 运行时修改值被允许
        // ptr1 = new int(value); // 运行时修改指针不被允许
        delete ptr1;
    }
    void test_constexpr() {
        // 2. constexpr 一定是编译时完成初始化
        constexpr int size = 123; // 编译时初始化
        // constexpr int len = getInput(); // 编译时初始化失败，运行时初始化
    }
    int f1() {
        return 0;
    }
    constexpr int f2() {
        return f1();
    }
    int f3() {
        return f2();
    }
} // namespace const_constexpr
namespace use_constexpr_ns {
    // 定义模板
    // T -> 类型参数, Size -> 非类型参数
    template <typename T, int Size>
    class FixArray {
    public:
        FixArray() {
            for (int i = 0; i < Size; ++i) {
                array[i] = T(); // 默认初始化
            }
        }
        int getLength() const {
            return Size;
        }

    private:
        T array[Size];
    };

    template <typename T, size_t N>
    void printArray(const T *const arr) {
        for (int i = 0; i < N; ++i) {
            std::cout << *(arr + i) << " ";
        }
        std::cout << std::endl;
    }
    template <typename T, size_t N>
    void printArray2(const T (&arr)[N]) {
        for (auto v : arr) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    // 4.位域宽度的设置(指定存储的位数) bit -> 位域
    constexpr int WIDTH = 4;
    struct BitField {
        unsigned int x : WIDTH; // 指定存储的位数为4个bit
        unsigned int y : 2; // 指定存储的位数为2个bit
    };
    // 5.内存对齐说明符 alignas (结构体总大小必须是 8 的整数倍。)(对齐的本质是改变起始地址的位置)
    struct alignas(double) Student {
        // 内存布局大致如下：
        // 偏移	成员	大小
        // 0 ~ 3	id	4
        // 4 ~ 9	name	6
        // 10 ~ 15	填充	6
        // 16 ~ 23	scope	8
        // 24	sex	1
        // 25 ~ 31	尾部填充	7
        // 原因：
        // double 通常要求 8 字节对齐，所以它从偏移 16 开始。
        // alignas(16) 要求整个结构体按 16 字节对齐。
        // 结构体总大小必须是 16 的整数倍。
        int id; // 4byte
        char name[6]; // 6 byte
        double scope; // 8 byte
        char sex; // 1 byte
    }; // 字节对齐 4+6+(补充6byte)+8+1+(补充7) 共32byte

    // (结构体总大小必须是 64 的整数倍。)
    struct alignas(64) Student2 {
        int id; // 4byte
        char name[6]; // 6 byte
        double scope; // 8 byte
        char sex; // 1 byte
        // alignas(64) 要求整个结构体按 64 字节对齐, 结构体总大小必须是 64 的整数倍。
    }; // 字节对齐 4+6+(补充6byte)+8+1+(补充39) 共64byte

    void use_constexpr() {
        // 需要编译时常量的场景

        // 1.定义数组的长度
        constexpr int size1 = 99;
        int array[size1]{ 11 };
        printArray<int, size1>(array);
        printArray2(array);
        // 2.指定模板的非类型参数
        constexpr int SIZE = 12;
        FixArray<int, SIZE> fx;
        std::cout << "fx.getLength(): " << fx.getLength() << std::endl;
        // 3.case标签
        constexpr int START = 0;
        constexpr int STOP = 1;
        int input = const_constexpr::getInput();
        switch (input) {
        case START:
            std::cout << "程序开始执行START" << std::endl;
            break;
        case STOP:
            std::cout << "程序结束执行STOP" << std::endl;
            break;
        default:
            std::cout << "程序执行DEFAULT" << std::endl;
        }
        // 4.位域宽度的设置(指定存储的位数) bit -> 位域
        BitField b1{};
        b1.x = 15; // 1111
        std::cout << "b1.x=" << b1.x << std::endl; // b1.x=15
        b1.x = 16; // 10000 (超宽)
        std::cout << "b1.x=" << b1.x << std::endl; // b1.x=0
        // 5.内存对齐说明符 alignas
        std::cout << "struct alignas(double) Student\t" << "sizeof(Student): " << sizeof(Student) << std::endl; // 32
        std::cout << "struct alignas(16) Student2\t" << "sizeof(Student2): " << sizeof(Student2) << std::endl; // 64
        // 对齐一个buff
        alignas(double) char buffer[12]; // 对齐的本质是改变起始地址的位置，不能改变buffer的大小
        std::cout << "sizeof(buffer)\t" << sizeof(buffer) << std::endl; // 12
    }
} // namespace use_constexpr_ns

// 2. constexpr 在 C++14 中改进
// C++14 对 constexpr 做了重大改进，显著放宽了限制，使其在实际编程中更加实用。以下是详细说明：
namespace constexpr_14_ns {
    // 2.1 放宽函数体内的限制
    // C++11 中只能有一条 return 语句，不能有局部变量、循环、分支等
    // c++11
    constexpr int factorial_11(int n) {
        return n <= 1 ? n : n * factorial_11(n - 1);
    }

    // C++14 放宽了限制，允许循环、允许使用局部变量并修改、允许使用 if-else等
    // 1.不允许使用try catch。但允许使用throw(c++20)
    // 2.不可使用vector string，因为这两个类底层依赖动态内存分类，但是c++23之后允许
    // 3.自定义对象，成功创建的要求是提供constexpr修饰的构造函数
    // 4.不可使用局部静态变量，但是c++23之后允许
    //  c++14
    constexpr int factorial_14(int n) {
        int result = 1;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }
        return result;
    }
    /**
     * c++11
     1.被constexpr修饰的函数返回值不允许为void
     2.被constexpr修饰的构造函数必须为空，且只能通过初始化列表初始化所有成员(初始化徐放到初始化列表)
     3.被constexpr修饰的类成员函数会隐式成为const函数
     * c++14
     1.被constexpr修饰的函数返回值允许为void
     2.被constexpr修饰的构造函数可以有函数体，可以包含控制语句
     3.被constexpr修饰的类成员函数体内部可以修改类成员的值
     */
    class Point {
    public:
        // C++14 中 constexpr 构造函数可以有函数体和控制语句, 但成员变量
        // 仍然必须在初始化列表 (mem-initializer) 中初始化, 不能在函数体内
        // 赋值, 否则会报
        // "member 'Point::x' must be initialized by mem-initializer in
        //  'constexpr' constructor"。
        constexpr Point(int v1, int v2) : x(v1 > 100 ? 100 : v1), y(v2 > 100 ? 100 : v2) {
            // 构造函数体内可以包含语句, 但不能在这里为成员赋值(常量求值限制)
        }
        constexpr int getX() const {
            return x;
        }
        constexpr int getY() const {
            return y;
        }

    private:
        int x;
        int y;
    };
    void test_point() {
        // C++14 中 constexpr 构造函数体内允许控制语句, 但成员必须在初始化
        // 列表中完成初始化; 因此这里只演示常量的构造与拷贝。
        constexpr Point p(100, 200);
        Point p2(14, 5);
        constexpr Point p3 = p;
        // constexpr Point p3 = p2;//错误: p2 不是常量表达式
        std::cout << "p.getX(): " << p.getX() << ", p.getY(): " << p.getY() << std::endl;
        std::cout << "p2.getX(): " << p2.getX() << ", p2.getY(): " << p2.getY() << std::endl;
        std::cout << "p3.getX(): " << p3.getX() << ", p3.getY(): " << p3.getY() << std::endl;
    }

    // 2.2 constexpr 成员函数不再隐式为 const

    //     根据上文对讲解，我们可以对 constexpr 在 C++11 与 C++14 中的特性做如下总结：

    // 特性	C++11	C++14
    // 构造函数体	必须为空	可以有任意语句
    // 局部变量	❌ 不允许	✅ 允许
    // 控制流语句	❌ 不允许	✅ 允许
    // 函数调用	❌ 不允许	✅ 允许（调用 constexpr 函数）
    // 循环	❌ 不允许	✅ 允许
    // 修改类成员	只能在初始化列表	只能在初始化列表

} // namespace constexpr_14_ns

int main() {
    const_constexpr::test_const();
    const_constexpr::test_constexpr();

    use_constexpr_ns::use_constexpr();

    constexpr_14_ns::test_point();
    return 0;
}