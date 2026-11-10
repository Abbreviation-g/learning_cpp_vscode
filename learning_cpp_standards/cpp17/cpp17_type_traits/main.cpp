#include <cstddef>
#include <ios>
#include <iostream>
#include <string_view>
#include <type_traits>

// https://subingwen.cn/cpp/type_traits/
//  类型萃取

// 类型萃取允许在编译期确定类型的某种属性。例如，判断一个类型是否是指针、是否是 const
// 修饰的、两个类型是否相同，或者获取类型的成员变量类型等。

// 在编写模板代码时，我们经常需要对不同的类型进行不同的处理。例如，一个排序函数模板，针对内置类型（如
// int）可以使用快速拷贝（如 memcpy），而针对自定义类类型（如
// Student）则必须调用拷贝构造函数。类型萃取帮助编译器做出这种选择。

// 大多数 <type_traits>
// 中的类都有一个共同的结构模式。它们通常是一个类模板，接受一个或多个类型参数，并包含以下成员：

// value: 一个 static constexpr 成员变量。
// 如果是 判断类 traits，它是 bool 类型（如 true 或 false）。
// 如果是 变换类 traits，它可能是整数或其他值（如 std::integral_constant）。
// type: 一个公有的 typedef 或 using 定义的类型别名。
// 如果是 判断类 traits，它通常是输入类型本身（主要用于继承）。
// 如果是 变换类 traits，它是变换后的新类型。

namespace ns1 {
    // C++11/14 时期，我们写代码比较啰嗦
    void test() {
        std::cout << std::boolalpha;
        // C++11/C++14 风格, 判断 int 是否为整型
        std::cout << std::is_integral<int>::value << "\n";

        using NoConstT1 = std::remove_const<const int>::type;
        // 检查一下是否正确
        std::cout << std::is_same<NoConstT1, int>::value << "\n";
    }
    // C++17 引入了 _v 和 _t 后缀（其实 _t 是 C++14 引入的，但在 17 中已成为标配习惯）：
    void test2() {
        std::cout << std::boolalpha;
        // C++17 风格, 判断 int 是否为整型
        std::cout << std::is_integral_v<int> << "\n";

        using NoConstT1 = std::remove_const_t<const int>;
        // 检查一下是否正确
        std::cout << std::is_same_v<NoConstT1, int> << "\n";
    }
} // namespace ns1

namespace ns2 {
    // 2. 详细特性分类表 (C++17 版)
    // 以下列出的所有 xxxx_v 变量模板均定义为 inline constexpr bool xxxx_v = xxxx::value;。
    // 下面表格并未列出所有类，如有需要可自行查询类型萃取相关类 API 在线文档
    // https://subingwen.cn/cpp/type_traits/#1-1-%E6%A0%B8%E5%BF%83%E6%A6%82%E5%BF%B5

    // clang-format off
    // 2.1 基础类型判断
    // 用于检查类型 T 是否具有某种基础性质。
    // 特性类	C++17 变量模板	描述
    // is_void<T>	is_void_v<T>	是否是 void
    // is_null_pointer<T>	is_null_pointer_v<T>	是否是 std::nullptr_t
    // is_integral<T>	is_integral_v<T>	是否为整型 (int, char, bool 等)
    // is_floating_point<T>	is_floating_point_v<T>	是否为浮点型 (float, double)
    // is_array<T>	is_array_v<T>	是否为数组
    // is_pointer<T>	is_pointer_v<T>	是否为指针
    // is_lvalue_reference<T>	is_lvalue_reference_v<T>	是否为左值引用
    // is_enum<T>	is_enum_v<T>	是否为枚举类型
    // is_union<T>	is_union_v<T>	是否为联合体
    // is_class<T>	is_class_v<T>	是否为类或结构体
    // is_function<T>	is_function_v<T>	是否为函数类型
    // is_member_pointer<T>	is_member_pointer_v<T>	是否为成员函数指针或成员变量指针
    // is_arithmetic<T>	is_arithmetic_v<T>	是否为算术类型: 整数或浮点数
    // is_fundamental<T>	is_fundamental_v<T>	是否为基本类型: void、整数、浮点、nullptr
    // is_object<T>	is_object_v<T>	是否为对象类型
    // is_compound<T>	is_compound_v<T>	是否为复合类型
    // is_reference<T>	is_reference_v<T>	是否为引用：左值或右值引用

    // 2.2 类型属性判断
    // 检查类型的更深层属性。
    // 特性类	C++17 变量模板	描述
    // is_const<T>	is_const_v<T>	是否有 const 修饰
    // is_volatile<T>	is_volatile_v<T>	是否有 volatile 修饰
    // is_trivial<T>	is_trivial_v<T>	是否为平凡类型 (可安全 memcpy)
    // is_standard_layout<T>	is_standard_layout_v<T>	是否为标准布局 (兼容 C 语言)
    // is_pod<T> (C++20起移除)	is_pod_v<T>	是否为 POD 类型 (既是 trivial 又是 standard_layout)
    // is_empty<T>	is_empty_v<T>	是否为空类 (无数据成员，虚函数通常不为空)
    // is_polymorphic<T>	is_polymorphic_v<T>	是否为多态类 (含虚函数)
    // is_abstract<T>	is_abstract_v<T>	是否为抽象类 (含纯虚函数)

    // 2.3 类型关系判断
    // 检查类型之间的相互关系。
    // 特性类	C++17 变量模板	描述
    // is_same<T, U>	is_same_v<T, U>	T 和 U 是否完全相同
    // is_base_of<Base, Der>	is_base_of_v<Base, Der>	Base 是否是 Der 的基类
    // is_convertible<T, U>	is_convertible_v<T, U>	T 是否可隐式转换为 U
    // is_invocable<...>	is_invocable_v<...>	[C++17] 给定参数是否可调用
    // invoke_result<F, Args>	invoke_result_t<F, Args>	推导调用结果的类型

    // 在 C++17 之前，很难非常精准地判断一个对象是否可以被调用，以及调用的结果类型是什么。C++17 引入了一系列工具：
    // 元特征	作用	语法
    // is_invocable_v<F, Args...>	仅判断能否调用，不限制返回值	无返回类型参数
    // is_invocable_r_v<R, F, Args...>	能调用 且返回值可隐式转为 R	第一个模板参数是目标返回类型
    // is_nothrow_invocable_v<F, Args...>	能调用，且调用标记 noexcept	不抛异常版本
    // is_nothrow_invocable_r_v<R, F, Args...>	能调用、返回可转 R、且 noexcept	全限制版本
    // clang-format on

    void func(int a) {
        std::cout << "Function called with " << a << std::endl;
    }
    struct Functor {
        void operator()(double d) {}
    };
    void test() {
        // ==========================================
        // 1. is_invocable 测试
        // ==========================================
        std::cout << "--- is_invocable checks ---" << std::endl;
        std::cout << std::boolalpha;

        // 检查 func 是否能被 int 参数调用
        std::cout << "func(int): " << std::is_invocable_v<decltype(func), int> << "\n"; // true
        // 检查 func 是否能被 double 参数调用 (隐式转换)
        std::cout << "func(double): "
                  << std::is_invocable_v<decltype(func), double> << "\n"; // true
        // 检查 func 是否能被 string 参数调用 (不允许)
        std::cout << "func(std::string): "
                  << std::is_invocable_v<decltype(func), std::string> << "\n"; // false

        // 检查 lambda, lambda 接受 string，const char* 可以构造 string，且返回 size_t

        auto lambda = [](const std::string &s) { return s.size(); };
        std::cout << "lambda(cont char*): "
                  << std::is_invocable_r_v<size_t, decltype(lambda), const char *> << "\n"; // true
        std::cout << "lambda return can be received by int: "
                  << std::is_invocable_r_v<int, decltype(lambda), const char *> << "\n"; // true
        std::cout
            << "lambda(std::string_view): "
            << std::is_invocable_r_v<size_t, decltype(lambda), std::string_view> << "\n"; // false

        // ==========================================
        // 2. invoke_result 测试
        // ==========================================
        std::cout << "\n--- invoke_result checks ---" << std::endl;
        // 2.1 基础试例：获取普通函数的返回类型
        // 语法：std::invoke_result_t<Callable, Args...>
        using FuncReturn = std::invoke_result_t<decltype(func), int>;
        // 验证返回类型是否为 void
        std::cout << "return type of func(int) is void: "
                  << std::is_same_v<FuncReturn, void> << "\n"; // true
        // 2.2 测试 lambda 的返回值
        // lambda 的逻辑是：接受 string，返回 size_t
        using LambdaReturn = std::invoke_result_t<decltype(lambda), std::string>;
        // 验证返回类型是否为 size_t
        std::cout << "return type of lambda(std::string) is size_t: "
                  << std::is_same_v<LambdaReturn, size_t> << "\n"; // true
        // 2.3 测试函数对象的返回值
        // Functor 的 operator() 返回 void
        using FunctorReturn = std::invoke_result_t<Functor, double>;
        std::cout << "return type of Functor(double) is void: "
                  << std::is_same_v<FunctorReturn, void> << "\n"; // true
    }

    // clang-format off
    //     2.4 类型修改
    // 注意：这些不是 _v 变量，而是 type 成员。C++14/17 使用 _t 别名模板。

    // 特性类	C++14/17 别名模板	描述
    // remove_const<T>	remove_const_t<T>	移除 const
    // remove_volatile<T>	remove_volatile_t<T>	移除 volatile
    // remove_cv<T>	remove_cv_t<T>	移除 const 和 volatile
    // add_const<T>	add_const_t<T>	添加 const
    // remove_reference<T>	remove_reference_t<T>	移除引用
    // add_lvalue_reference<T>	add_lvalue_reference_t<T>	添加左值引用 &
    // add_rvalue_reference<T>	add_rvalue_reference_t<T>	添加右值引用 &&
    // remove_pointer<T>	remove_pointer_t<T>	移除顶层指针
    // add_pointer<T>	add_pointer_t<T>	添加指针
    // make_signed<T>	make_signed_t<T>	转为有符号类型
    // make_unsigned<T>	make_unsigned_t<T>	转为无符号类型
    // remove_extent<T>	remove_extent_t<T>	移除数组的一维
    // remove_all_extents<T>	remove_all_extents_t<T>	移除数组的所有维度
    // decay<T>	decay_t<T>	类型退化：数组转指针、函数转指针、去 cv、去引用

    // 2.5 特性逻辑组合
    // C++11 时只能写 std::is_integral::value && std::is_floating_point::value。C++17 提供了类型层面的逻辑运算。
    // 特性类	描述
    // conjunction<B1...>	逻辑与。如果所有 Bi 都为 true，则为 true。短路求值（遇到 false 停止）。
    // disjunction<B1...>	逻辑或。如果任意 Bi 为 true，则为 true。短路求值。
    // negation<B>	逻辑非。取反。
    // clang-format on

    // 逻辑：是算术类型 且 不是 bool (bool也是算术类型，但在某些业务逻辑中我们排除它)
    template<typename T>
    using is_numeric =
        std::conjunction<std::is_arithmetic<T>, std::negation<std::is_same<T, bool>>>;

    void test2() {
        std::cout << std::boolalpha;
        // 1. 使用 _t 别名模板
        using CharPtr = std::add_pointer_t<char>;
        std::cout << "CharPtr is pointer: " << std::is_pointer_v<CharPtr> << "\n";

        // 2. 使用 C++17 的 conjunction (逻辑与) 和 negation (逻辑非)
        // 检查 double 是否符合我们自定义的 "numeric" 定义：是算术且不是bool -> true
        std::cout << "is double numeric: " << is_numeric<double>::value << "\n"; // true
        // 检查 bool 是否符合
        std::cout << "is double numeric: " << is_numeric<bool>::value << "\n"; // false
        // 检查 CharPtr 是否符合
        std::cout << "Is char* numeric? " << is_numeric<CharPtr>::value << std::endl; // false
    }

} // namespace ns2
int main() {
    ns1::test();
    ns2::test();

    ns2::test();
    ns2::test2();
}