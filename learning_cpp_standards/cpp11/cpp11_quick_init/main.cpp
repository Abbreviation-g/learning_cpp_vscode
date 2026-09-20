// https://subingwen.cn/cpp/quick_init/
// 类成员的快速初始化

#include <iostream>

namespace init_ns1 {
    class Base {
    public:
        Base() : a(250) {}
        Base(int num) : a(num) {}
        int a;
        int b = 1;
        static int c;
        // static int c = 0; // error // 类的静态成员，必须在类的外部进行初始化
        static const double d;
        // static const double d = 3.14; // error // 类的静态常量成员，但不是整形或者枚举，无法通过编译
        static const char *const e;
        // static const char *const e = "i am fury"; //error// 类的静态常量成员，但不是整形或者枚举，无法通过编译
        const static int f = 0; // 正确// 静态常量成员可以类内初始化
        //  static const 和 const static 是等价的。
    };
    int Base::c = 0;
    const double Base::d = 3.14;
    const char *const Base::e = "i am fury";
} // namespace init_ns1

// C++11 标准的类成员初始化
namespace init_ns2 {
    using namespace std;
    //     2.1 初始化类的非静态成员
    // 在进行类成员变量初始化的时候，C++11标准对于C++98做了补充，允许在定义类的时候在类内部直接对非静态成员变量进行初始化，在初始化的时候可以使用等号 =
    // 也可以使用花括号 {} 。
    class Test {
    private:
        int a = 9;
        int b = { 5 };
        int c{ 12 };
        double array[4] = { 3.14, 3.15, 3.16, 3.17 };
        double array1[4]{ 3.14, 3.15, 3.16, 3.17 };
        // string s1("hello"); // error // 错误，不能使用小括号() 初始化对象，应该使用花括号{}
        string s2{ "hello, world" };
    };
    // 2.2 类内部赋值和初始化列表
    class Init {
    public:
        Init(int x, int y, int z) : a(x), b(y), c(z) {}

        int a = 1;
        int b = 2;
        int c = 3;
    };
    void test() {
        Init tmp(10, 20, 30);
        cout << "a: " << tmp.a << ", b: " << tmp.b << ", c: " << tmp.c << endl;
    }
} // namespace init_ns2
int main() {
    init_ns2::test();
}
