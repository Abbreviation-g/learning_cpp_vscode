#include <iostream>
// https://subingwen.cn/cpp/enum/
// 强类型枚举

namespace enum_ns1 {
    //     1.2 枚举的缺陷
    // C/C++的enum有个很”奇怪” 的设定，就是具名（有名字）的enum类型的名字，以及 enum 的成员的名字都是全局可见的。这与 C++中具名的 namespace、class/struct 及
    // union 必须通过名字::成员名的方式访问相比是格格不入的，编码过程中一不小心程序员就容易遇到问题。比如∶

    // enum China {Shanghai, Dongjing, Beijing, Nanjing};
    // enum Japan {Dongjing, Daban, Hengbin, Fudao};
    // 上面定义的两个枚举在编译的时候，编译器会报错，具体信息如下：
    // error C2365: “Dongjing”: 重定义；以前的定义是“枚举数”

    // 2. 强类型枚举
    // 2.1 优势
    // 针对枚举的缺陷，C++11标准引入了一种新的枚举类型，即枚举类，又称强类型枚举（strong-typed enum）。声明强类型枚举非常简单，只需要在 enum 后加上关键字
    // class。比如∶

    // // 定义强类型枚举
    // enum class Colors{Red, Green, Blue};
    // 强类型枚举具有以下几点优势∶

    // -强作用域，强类型枚举成员的名称不会被输出到其父作用域空间。
    //      -  强类型枚举只能是有名枚举，如果是匿名枚举会导致枚举值无法使用（因为没有作用域名称）。
    // - 转换限制，强类型枚举成员的值不可以与整型隐式地相互转换。
    // - 可以指定底层类型。强类型枚举默认的底层类型为 int，但也可以显式地指定底层类型，具体方法为在枚举名称后面加上∶type，其中 type 可以是除 wchar_t
    // 以外的任何整型。比如: enum class Colors :char { Red, Green, Blue };
    using namespace std;
    enum class China {
        Shanghai,
        Dongjing,
        Beijing,
        Nanjing,
    };
    enum class Japan : char { Dongjing, Daban, Hengbin, Fudao };
    void test1() {
        // int m = Shanghai; // error
        // int n = China::Shanghai; // error
        if ((int)China::Beijing >= 2) {
            cout << "ok!" << endl;
        }
        cout << "size1: " << sizeof(China::Dongjing) << endl;
        cout << "size2: " << sizeof(Japan::Dongjing) << endl;

        // int m = Shanghai：该行的代码有两处错误
        // 强类型枚举属于强作用于类型，不能直接使用，枚举值前必须加枚举类型
        // 强类型枚举不会进行隐式类型转换，因此枚举值不能直接给int行变量赋值（虽然强类型枚举的枚举值默认就是整形，但其不能作为整形使用）。
        // int n = China::Shanghai：语法错误，将强类型枚举值作为整形使用，此处不会进行隐式类型转换
        // if ((int)China::Beijing >= 2)：语法正确，强类型枚举值在和整数比较之前做了强制类型转换。
        // sizeof(China::Dongjing)：打印的结果为4，强类型枚举底层类型值默认为int，因此占用的内存是4个字节
        // sizeof(Japan::Dongjing)：打印的结果为1，显示指定了强类型枚举值的类型为char，因此占用的内存大小为1个字节，这样我们就可以节省更多的内存空间了。
    }

    //     2.2 对原有枚举的扩展
    // 相比于原来的枚举，强类型枚举更像是一个属于C++的枚举。但为了配合新的枚举类型，C++11还对原有枚举类型进行了扩展：

    // 原有枚举类型的底层类型在默认情况下，仍然由编译器来具体指定实现。但也可以跟强类型枚举类一样，显式地由程序员来指定。其指定的方式跟强类型枚举一样，都是枚举名称后面加上∶type，其中type
    // 可以是除 wchar_t 以外的任何整型。比如∶

    // enum Colors : char { Red, Green, Blue };
    // 关于作用域，在C++11中，枚举成员的名字除了会自动输出到父作用域，也可以在枚举类型定义的作用域内有效。比如：

    enum Colors : char { Red, Green, Blue };
    void test2() {
        Colors c1 = Green; // C++11以前的用法
        Colors c2 = Colors::Green; // C++11的扩展语法
    }
    // C++11中对原有枚举类型的这两个扩展都保留了向后兼容性，也方便了程序员在代码中同时操作两种枚举类型。此外，我们在声明强类型枚举的时候，也可以使用关键字enum
    // struct。实际上 enum struct 和enum class在语法上没有任何区别（enum class 的成员没有公有私有之分，也不会使用模板来支持泛化的声明 ）。

} // namespace enum_ns1
int main() {
    enum_ns1::test1();
    enum_ns1::test2();
}