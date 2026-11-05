#include <minwindef.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <array>
#include <utility>
// https://subingwen.cn/cpp/ctad/
// 类模板参数推导
// C++17 允许编译器根据我们在构造函数中传入的参数，自动推断出模板参数 T
// 到底是什么类型，这就叫做类模板参数推导（Class Template Argument Deduction，简称
// CTAD）。简单说就是：编译器变得更聪明了，能猜出我们需要什么类型！

namespace ns1 {
    template <typename T>
    class MyBox {
    private:
        T value;

    public:
        MyBox(T val) : value(val) {}
    };

    void test() {
        MyBox box1(42); // 编译器：参数是 42 → int → 所以 T = int
        MyBox box2(3.14); // 编译器：参数是 3.14 → double → 所以 T = double
        MyBox box3("hello"); // 编译器：参数是 "hello" → const char* → 所以 T = const char*
    }

    void test2() {
        // 以前
        std::pair<std::string, int> p1("Alice", 25);
        // C++17
        std::pair p2("Bob", 30); // 推导为 <const char*, int>

        auto p3 = std::make_pair("Charlie", 40); // 以前为了省类型这么写
        std::pair p4{ "Charlie", 40 }; // 现在直接这样写，更直观
    }
} // namespace ns1
// 推导指引的语法格式如下：

// template <template-parameters>
// ClassName(ConstructorParameters) -> DeducedClassName<ActualTypeArgs>;
// template <template-parameters>：这是推导指引自己所需的模板参数。

// 作用：定义指引中用到的类型变量。

// 是否可选：是的。如果推导规则不涉及泛型映射，就不需要写这个。

// ClassName(ConstructorParameters)：这部分看起来非常像一个构造函数的声明（没有函数体）。

// 作用：编译器在代码中看到对象初始化时，会拿初始化的参数来匹配这里的 ConstructorParameters。

// 注意：这里只是描述参数的样子，并不一定要求类中真的有一个一模一样签名的构造函数（尽管通常都有）。

// -> DeducedClassName<ActualTypeArgs>：箭头 -> 分隔了左边和右边。

// 作用：告诉编译器最终生成的类类型是什么。
// DeducedClassName：通常就是类模板本身的名字。
// <ActualTypeArgs>：这里才是真正决定类模板参数 T
// 是什么的地方。你可以在这里对类型进行修改（例如去掉引用、强制转为 int 等）。
// 这个语法的作用就像是一座翻译桥，它告诉编译器：“当你看到右边（构造函数）这种形式的参数时，请把左边的类模板实例化为右边（目标类型）指定的样子”。

// 推导指引必须定义在类模板的定义之后，且位于同一个命名空间作用域中（通常就在类定义下面）。
namespace ns2 {
    //     2.1 构造函数是独立模板
    // 下面的例子中用到了两套独立的模板：外层是类模板参数为T，内层是函数模板模板参数为U:

    // template<typename T> → 类模板参数：实例化 WrapperGood<XXX>/WrapperBad<XXX>
    // 的时候就确定死了，它的对象的成员 val 的类型就是 T。一旦创建这个类，T 就固定不变。
    // template<typename U> → 构造函数本身是一个函数模板，U 是构造时推导的类型。

    // 错误示范：没有推导指引
    template <typename T>
    struct WrapperBad {
        template <typename U>
        WrapperBad(U &&u) : val(std::forward<U>(u)) {
            std::cout << "WrapperBad Constructed" << std::endl;
        }
        T val;
    };
    // 正确示范：添加推导指引
    template <typename T>
    struct WrapperGood {
        template <typename U>
        WrapperGood(U &&u) : val(std::forward<U>(u)) {
            std::cout << "WrapperGood Constructed" << std::endl;
        }
        T val;
    };
    // === 关键点：推导指引 ===
    // std::decay_t 会去掉引用和 cv 限定符，返回纯净的类型
    template <typename U>
    WrapperGood(U &&u) -> WrapperGood<std::decay_t<U>>;

    void test() {
        int x = 10;
        // WrapperBad wb{x}; // error
        WrapperGood wg{ x };
        std::cout << "Type of wg.val is int? "
                  << std::is_same_v<decltype(wg.val), int> << "\n"; // 输出 1 (true)

        WrapperGood wg2{ 22.0 };
        std::cout << std::is_same_v<decltype(wg2.val), decltype(22.0)> << "\n";

        //         具体来说，当写下 WrapperBad wb(x); 时：

        // CTAD 机制启动：编译器尝试根据构造函数的参数来推导 WrapperBad 的模板参数 T。
        // 构造函数匹配：它找到了构造函数 template <typename U> WrapperBad(U&& u)。
        // 推导失败：
        // 构造函数本身是一个函数模板，它有自己的模板参数 U。
        // 根据参数 x（左值 int），编译器可以推导出 U 为 int&。
        // 编译器无法自动推导出类模板参数 T 和 U 之间的关系。它不知道 T 应该等于 U，还是
        // std::decay_t<U>，或者是其他什么类型。 由于T 无法推导，因此编译器报错。

        //         为什么 WrapperGood wg(x) 成功？因为有类型推导引导。

        // template <typename U>
        // WrapperGood(U&&) -> WrapperGood<std::decay_t<U>>;
        // 推导过程：编译器看到 WrapperGood wg(x)。它查找是否存在推导指引。
        // 应用指引：它发现了 WrapperGood(U&&) -> WrapperGood<std::decay_t<U>>。
        // 强制关联：指引明确告诉编译器：“如果你看到构造函数参数是 U&&，那么类模板参数 T 必须是
        // std::decay_t<U>”。 这里 x 是 int，推导出 U 为 int。 指引强制 T 为 std::decay_t<int>，即
        // int。 结果：T 被成功推导为 int，编译通过。
    }
} // namespace ns2
namespace ns3 {
    // 2.2 聚合类的初始化
    // 聚合类没有构造函数，直接使用成员变量。虽然 C++20 开始支持部分隐式推导，但在 C++17
    // 中或者为了保证跨版本的稳定性，通常需要指引。特别是当使用 {} 初始化时。
    template <typename T>
    struct Point {
        T x;
        T y;
    };
    // === 推导指引 ===
    template <typename T>
    Point(T, T) -> Point<std::decay_t<T>>;

    void test() {
        Point p1{ 1.2, 1.0 }; // 推导为 Point<double>
        Point p2{ 1, 2 }; // 推导为 Point<int>
    }
} // namespace ns3

// 2.3 类型转换与别名推导
// 有时候构造函数接受的参数类型并不是 T，而是我们需要将类实例化为 T。
namespace ns4 {
    template <typename T>
    class MyString {
    public:
        // 1. 接受 std::string 的构造函数
        explicit MyString(std::string s) : data(s) {}

        template <std::size_t N>
        explicit MyString(const std::array<char, N> &chars) : data(chars.data(), N) {}

        template <typename U, typename = typename std::enable_if<std::is_arithmetic_v<U>>::type>
        explicit MyString(U u) : data(std::to_string(u)) {}

        void print() const {
            std::cout << data << std::endl;
        }

    private:
        std::string data;
    };
    // === 推导指引 ===
    MyString(std::string) -> MyString<std::string>;
    // 数值类型的推导指引
    template <typename Integer>
    MyString(Integer) -> MyString<std::string>;
    // const char*
    MyString(const char *) -> MyString<std::string>;
    // std::array<char, N>
    template <std::size_t N>
    MyString(const std::array<char, N> &) -> MyString<std::string>;

    void test() {
        std::string s{ "hello" };

        MyString str1{ s };
        str1.print();

        const char *s2{ "world" };
        MyString str2{ s2 };
        str2.print();

        MyString str3{ 101 }; // 匹配 MyString(U u)，U=int，调用 to_string(100)
        str3.print();

        MyString str4{ 100.01 };
        str4.print();

        std::array arr{ 'x', 'x' };
        MyString str5{ arr };
        str5.print();
    }
} // namespace ns4

// 3.1 隐式推导指引
// 编译器会自动从类模板的普通构造函数、模板构造函数中，自动生成一条实参类型原样映射类模板参数的指引，全程无需开发者手写代码。
namespace ns5 {
    // 类模板
    template <typename T>
    struct SimpleBox {
        // 普通构造函数
        SimpleBox(T val) {}
    };

    // 编译器自动生成隐式推导指引：
    // SimpleBox(T) -> SimpleBox<T>;

    // CTAD 生效，自动推导 T = int
    SimpleBox b(10);

    template <typename T>
    struct Wrapper {
        // 独立模板 U，和类模板 T 无关联
        template <typename U>
        Wrapper(U &&u) : val(std::forward<U>(u)) {}
        T val;
    };

    int x = 10;
    // Wrapper wg(x); // error
    Wrapper<int> wg(x);
} // namespace ns5

namespace ns6 {
    // 3.2 显式推导指引
    // 当隐式指引无法满足需求（需要类型转换、类型包装、关联脱节模板参数）时，开发者手动书写的推导规则，优先级高于隐式指引。

    // 3.2.1 非模板型显式推导指引（固定映射）
    // 指引本身无自定义模板参数，可以用于固定的类型转换场景：传入 A 类型，强制推导为 B 类型。

    // 示例：传入 int，强制推导 long 类型
    template <typename T>
    struct Box {
        Box(T v) {}
    };

    // 固定规则：实参为 int -> 推导 Box<long>
    Box(int) -> Box<long>;
    void test() {
        Box b(10); // 推导结果：Box<long>
        std::cout << std::is_same_v<decltype(b), Box<long>> << std::endl; // 1
        std::cout << std::is_same_v<decltype(b), Box<int>> << std::endl; // 0
    }

} // namespace ns6

namespace ns7 {
    //     3.2.2 模板型显式推导指引（通用映射，核心常用）
    // 指引自带独立模板参数，可捕获任意实参类型，实现通用推导，是工程中最常用的形式，完美解决万能引用、数组退化、类型清洗问题。

    // 示例1：修复万能引用构造的 CTAD 失效问题

    // 很多时候类模板想存值类型，但构造函数是万能引用 U&&，隐式 CTAD 会推导出引用
    // /const，不符合预期。

    template <typename T>
    struct Wrapper {
        T data;
        template <typename U>
        Wrapper(U &&u) : data(std::forward<U>(u)) {}
    };

    // ========== 模板型显式推导指引 ==========
    template <typename U> // U是指引自己的模板参数，捕获实参类型
    Wrapper(U &&) -> Wrapper<std::remove_cv_t<std::remove_reference_t<U>>>;
    // 含义：不管传进来什么U（带const/&），去掉引用、const、volatile，作为Wrapper的T

    void test() {
        int a = 10;
        const int b = 20;

        Wrapper w1{ a }; // U=int& → remove_cvref → int → Wrapper<int>
        Wrapper w2{ b }; // U=const int& → remove_cvref → int → Wrapper<int>
        Wrapper w3{ 30.0 }; // U=int → remove_cvref → int → Wrapper<int>

        static_assert(std::is_same_v<decltype(w1), Wrapper<int>>);
        static_assert(std::is_same_v<decltype(w2), Wrapper<int>>);
        static_assert(std::is_same_v<decltype(w3), Wrapper<double>>);
    }
} // namespace ns7

namespace ns8 {
    // 示例2：一对多转换：传入 pair，提取元素构造类模板
    template <typename T1, typename T2>
    struct PairBox {
        T1 first;
        T2 second;
        // 接收两个参数的构造
        PairBox(T1 a, T2 b) : first(a), second(b) {}
        // 接收std::pair的构造函数，单参数版本
        template <typename A, typename B>
        explicit PairBox(const std::pair<A, B> &p) : first(p.first), second(p.second) {}
    };
    // CTAD推导指引：当传入std::pair<A,B>时，推导成PairBox<A,B>
    template <typename A, typename B>
    PairBox(std::pair<A, B>) -> PairBox<A, B>;
    void test() {
        std::pair p{ 3.14, 100 };
        PairBox box{ p };

        static_assert(
            std::is_same_v<decltype(box), PairBox<decltype(p.first), decltype(p.second)>>);
    }
} // namespace ns8

namespace ns9 {
    //     3.3 特殊修饰：explicit 推导指引
    // 就像构造函数可以加 explicit 一样，显式推导指引也可以添加 explicit
    // 修饰，用于限制拷贝初始化，仅允许直接初始化。
    template <typename T>
    struct MyDemo {
        MyDemo(T) {}
    };
    template <typename T>
    explicit MyDemo(T) -> MyDemo<T>;

    void test() {
        MyDemo w1{ 10 };
        MyDemo s2 = 10;
    }

    // 编译上面的程序会发现MyDemo w2 = 10;
    // 这行代码并不会报错，这又是为什么呢？因为CTAD候选集合里面有两条候选：

    // 编译器自动生成的隐式推导指引：template<typename T> MyDemo(T) -> MyDemo<T>;（不是 explicit）
    // 手写的 explicit 推导指引：拷贝初始化场景直接被丢弃，不参与候选
    // 拷贝初始化 MyDemo w2 = 10; 做 CTAD 重载决议：

    // 所有 explicit 的推导指引，直接从候选列表删掉
    // 本类构造函数自动生成的隐式推导指引仍然保留，正常参与推导
    // 关键点总结：手写 explicit 推导指引，只是新增一条 explicit 候选；它不会删除 /
    // 覆盖编译器自动生成的隐式推导指引！既然是这样，那么什么时候 explicit
    // 手写推导指引才真的生效呢？

    // 只有隐式生成的推导指引不存在的时候，手写的这条才是唯一候选，此时explicit的限制才能体现威力。
    // 典型场景：派生类继承构造函数（没有本类构造函数，不会自动生成隐式指引）。
} // namespace ns9

namespace ns10 {
    // 3.4 继承构造函数与推导指引
    // 在下面这个例子中，我们定义了一个基类 Base 和一个派生类 Derived。Derived 继承了 Base
    // 的构造函数。
    template <typename T>
    struct Base {
        Base(T) {}
    };
    template <typename T>
    struct Derived : Base<T> {
        using Base<T>::Base; // 继承 Base 的构造函数
    };

    //     using Base<T>::Base;：把基类构造函数 “引入”
    //     到派生类作用域，派生类可以直接调用基类构造函数。 但是：继承构造函数 ≠
    //     在派生类生成一个真正的构造函数实体。
    // 隐式推导指引生成规则：编译器扫描派生类自己（直接声明）的构造函数；继承过来的构造函数不算，不会用来自动生成
    // CTAD 指引。
    void test() {
        // 为什么报错？
        // Derived<T> 自己没有手写构造函数；只有继承来的构造函数。
        // 编译器生成隐式推导指引时，不看继承构造函数，所以不会自动生成任何推导指引。
        // Derived d = 10; CTAD 找不到候选推导指引，推导失败。
        // 就算写 Derived d{10}; 同样报错，CTAD 阶段直接失败。解决办法：手动给 Derived 写推导指引。

        // Derived d{ 10 }; // error
    }
} // namespace ns10
namespace ns10 {
    // 手写推导指引，人为增加CTAD候选
    template <typename T>
    explicit Derived(T) -> Derived<T>;
    void test2() {
        Derived d1{ 10 }; // explicit指引可用
        // Derived d2 = 10; // explicit推导指引直接被过滤，CTAD失败
    }
} // namespace ns10

namespace ns11 {
    class A;
    class B;
    class A {
    private:
        int a;

    public:
        int get() const {
            return a;
        }
        explicit A(int aa) : a(aa) {}
        explicit operator B() const;
    };
    class B {
    private:
        int b;

    public:
        explicit B(int bb) : b(bb) {}
        int get() const {
            return b;
        }
        explicit operator A() {
            return A{ b };
        }
    };

    A::operator B() const {
        return B{ a };
    }

    void test() {
        bool a_b = std::is_constructible_v<A, B>;
        bool b_a = std::is_constructible_v<B, A>;

        std::cout << "A->B" << a_b << "\n"; // 1
        std::cout << "B->A" << b_a << "\n"; // 1
    }
} // namespace ns11
int main() {
    ns1::test();
    ns1::test2();

    ns2::test();

    ns3::test();

    ns4::test();

    ns6::test();

    ns7::test();

    ns8::test();

    ns9::test();

    ns10::test();
    ns10::test2();

    ns11::test();
}

// 容易踩坑的结论清单：
//  using Base<T>::Base; 继承构造函数不会产生隐式 CTAD 推导指引。
//  隐式推导指引，只来源于本类直接声明的构造函数，继承构造函数忽略。
//  实例化完成（模板参数已知）之后，继承构造函数正常生效，不受 CTAD 规则影响。
//  手写推导指引不受继承构造函数限制，你可以随便写，用来匹配基类构造函数的参数。
//  explicit 可以写在推导指引或者基类构造函数上，两者生效阶段完全不一样：
//  explicit 推导指引：控制CTAD 阶段，拷贝初始化禁用这条指引
//  explicit 继承构造函数：控制构造函数调用阶段，拷贝初始化不能调用构造函数
