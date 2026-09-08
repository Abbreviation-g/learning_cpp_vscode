#include <cstddef>
#include <ios>
#include <iostream>
#include <limits>
#include <vector>
// https://subingwen.cn/cpp/variable-templates/
// 变量模板，函数模板，类模板
// template<typename T, int value>
// 1.简单变量模板的使用->相当于多了一个定义常量的方式，特点是：更强大，更灵活

namespace variable_template_ns {
    // C++14
    template <typename T>
    constexpr T pi = static_cast<T>(3.141592653589793L);
    template <typename T = double>
    constexpr T pi_2 = T(3.141592653589793L);

    // c++98/c++11模板的写法-》需要写多个特化类
    template <typename T>
    struct PI; // 主模板为空
    // 类特化
    template <>
    struct PI<float> {
        static constexpr float value = 3.141592653589793F;
    };
    template <>
    struct PI<long double> {
        static constexpr long double value = 3.141592653589793L;
    };

    // 通过静态函数获取数值
    template <typename T>
    constexpr T max_value = std::numeric_limits<T>::max();
    template <typename T>
    constexpr T min_value = std::numeric_limits<T>::min();

    void variable_template() {
        constexpr double d = pi<double>;
        std::cout << "pi<double>: " << d << std::endl;
        int i = pi<int>;
        std::cout << "pi<int>: " << i << std::endl;

        double d2 = pi_2<>;
        std::cout << "pi_2<>: " << d << std::endl;

        // 从特化类取值
        float f = PI<float>::value;
    }
    void test_max_value() {
        int max_i = max_value<int>;
        double max_d = max_value<double>;
        long double max_ld = max_value<long double>;
        std::cout << "max_value<int>: " << max_i << std::endl;
        std::cout << "max_value<double>: " << max_d << std::endl;
        std::cout << "max_value<long double>: " << max_ld << std::endl;

        int min_i = min_value<int>;
        double min_d = min_value<double>;
        long double min_ld = min_value<long double>;
        std::cout << "min_value<int>: " << min_i << std::endl;
        std::cout << "min_value<double>: " << min_d << std::endl;
        std::cout << "min_value<long double>: " << min_ld << std::endl;
        // max_value<int>: 2147483647
        // max_value<double>: 1.79769e+308
        // max_value<long double>: 1.18973e+4932
        // min_value<int>: -2147483648
        // min_value<double>: 2.22507e-308
        // min_value<long double>: 3.3621e-4932

        char min_ch = min_value<char>;
        char max_ch = max_value<char>;
        std::cout << "min_value<char>: " << min_ch << ", " << static_cast<int>(min_ch) << std::endl;
        std::cout << "max_value<char>: " << max_ch << ", " << static_cast<int>(max_ch) << std::endl;
    }

} // namespace variable_template_ns

// 变量模板的高级使用-》模板的特化，-》全特化，偏特化
// 全特化
// 语法 template<>
// 实现方式，
//  1.返回固定的类型
//  2.返回类型依赖于模板参数的类型
namespace variable_template_specialization_ns {
    // 1.返回固定的类型
    // 举例, 为不同的类型返回特定的大小值
    //  主模板
    template <typename T>
    constexpr size_t typesize = sizeof(T); // 处理普通的情况
    // 全特化 int
    template <>
    constexpr size_t typesize<int> = 4;
    // 全特化 double
    template <>
    constexpr size_t typesize<double> = 8;
    // 全特化 vector<int>，假定 vector的大小为100
    template <>
    constexpr size_t typesize<std::vector<int>> = 400;

    void test_typesize() {
        std::cout << "typesize<int>: " << typesize<int> << std::endl; // 4
        std::cout << "typesize<double>: " << typesize<double> << std::endl; // 8
        std::cout << "typesize<vector<int>>: " << typesize<std::vector<int>> << std::endl; // 400

        std::cout << "typesize<char>: " << typesize<char> << std::endl; // 1
        std::cout << "typesize<short>: " << typesize<short> << std::endl; // 2
    }

    // 全特化-》2.返回类型依赖于模板参数的类型
    // 举例，实现米和英尺的单位换算
    // 主模板
    template <typename T, T value>
    constexpr T meter_to_feet = T(3.280839895013123 * value);
    template <typename T, T value>
    constexpr T feet_to_meter = T(0.3048 * value);
    // 全特化， c++20之前，非类型参数不能是double，float等浮点型
    template <>
    constexpr int meter_to_feet<int, 1> = int(3.280839895013123 * 1);
    template <>
    constexpr int meter_to_feet<int, 5> = int(3.280839895013123 * 5);
    template <>
    constexpr int meter_to_feet<int, 10> = int(3.280839895013123 * 10);
    template <>
    constexpr int meter_to_feet<int, 100> = int(3.280839895013123 * 100);

    void test_meter_to_feet() {
        std::cout << "1 meter = " << meter_to_feet<int, 1> << " feet." << std::endl;
        std::cout << "5 meter = " << meter_to_feet<int, 5> << " feet." << std::endl;
        std::cout << "10 meter = " << meter_to_feet<int, 10> << " feet." << std::endl;
        std::cout << "100 meter = " << meter_to_feet<int, 100> << " feet." << std::endl;
        std::cout << "2 meter = " << meter_to_feet<int, 2> << " feet." << std::endl;
        // 1 meter = 3 feet.
        // 5 meter = 16 feet.
        // 10 meter = 32 feet.
        // 100 meter = 328 feet.
        // 2 meter = 6 feet.

        std::cout << "1.0 meter = " << meter_to_feet<double, 1.0> << " feet." << std::endl;
        std::cout << "5.0 meter = " << meter_to_feet<double, 5.0> << " feet." << std::endl;
        std::cout << "10.0 meter = " << meter_to_feet<double, 10.0> << " feet." << std::endl;
        std::cout << "100.0 meter = " << meter_to_feet<double, 100.0> << " feet." << std::endl;
        std::cout << "2.0 meter = " << meter_to_feet<double, 2.0> << " feet." << std::endl;
        // 1.0 meter = 3.28084 feet.
        // 5.0 meter = 16.4042 feet.
        // 10.0 meter = 32.8084 feet.
        // 100.0 meter = 328.084 feet.
        // 2.0 meter = 6.56168 feet.
    }

    // 偏特化: 为某一个类别的模板参数提供一个定制版本
    // 1.模板参数:
    //      全特化: 所有的模板参数都被具体类型替换了
    //      偏特化: 仍有部分模板参数未被指定
    // 2.语法
    //      全特化:template<>
    //      偏特化:template<typename T>
    // 3.使用的类型
    //      全特化: int double
    //      偏特化: int* int& int[]

    // 示例1:判断是不是数组类型
    // 主模板: 都不是数组
    template <typename T>
    constexpr bool is_array = false;
    // 偏特化处理, 带固定大小的数组
    template <typename T, size_t N>
    constexpr bool is_array<T[N]> = true;
    // 偏特化处理, 无固定大小的数组
    template <typename T>
    constexpr bool is_array<T[]> = true;
    // 偏特化处理, 带固定大小的二维数组
    template <typename T, size_t N, size_t M>
    constexpr bool is_array<T[N][M]> = true;

    void test_is_array() {
        std::cout << std::boolalpha;
        std::cout << "is_array<int>: " << is_array<int> << std::endl;
        std::cout << "is_array<int[6]>: " << is_array<int[6]> << std::endl;
        std::cout << "is_array<int[]>: " << is_array<int[]> << std::endl;
        std::cout << "is_array<int[6][5]>: " << is_array<int[6][5]> << std::endl;
        // is_array<int>: false
        //  is_array<int[6]>: true
        //  is_array<int[]>: true
        //  is_array<int[6][5]>: true
        int value = 5;
        int arr1[5] = { 1, 2, 3, 4, 5 };
        int arr2[] = { 1, 2, 3, 4, 5 };
        int arr3[2][2] = { 1, 2, 3 };
        int *ptr = arr1;
        std::cout << "is_array<decltype(value)>: " << is_array<decltype(value)> << std::endl;
        std::cout << "is_array<decltype(arr1)>: " << is_array<decltype(arr1)> << std::endl;
        std::cout << "is_array<decltype(arr2)>: " << is_array<decltype(arr2)> << std::endl;
        std::cout << "is_array<decltype(arr3)>: " << is_array<decltype(arr3)> << std::endl;
        std::cout << "is_array<decltype(ptr)>: " << is_array<decltype(ptr)> << std::endl;
        // is_array<decltype(value)>: false
        // is_array<decltype(arr1)>: true
        // is_array<decltype(arr2)>: true
        // is_array<decltype(arr3)>: true
        // is_array<decltype(ptr)>: false
        std::cout << std::noboolalpha;
    }
    // 示例2，判断两个类型是否相同
    // 主模板
    template <typename T, typename U>
    constexpr bool is_same_type = false;
    // 偏特化: 类型相同
    template <typename T>
    constexpr bool is_same_type<T, T> = true;
    // 偏特化， 处理const限定
    template <typename T, typename U>
    const bool is_same_type<const T, U> = is_same_type<T, U>; // 递归引用自身模板，会触发重载决议
    template <typename T, typename U>
    const bool is_same_type<T, const U> = is_same_type<T, U>; // 递归引用自身模板，会触发重载决议

    void test_is_same_type() {
        int value = 5;
        std::cout << std::boolalpha;

        std::cout << "is_same_type<int, decltype(value)> " << is_same_type<int, decltype(value)> << std::endl;
        std::cout << "is_same_type<int, const int> " << is_same_type<int, const int> << std::endl;
        std::cout << "is_same_type<const int, int> " << is_same_type<const int, int> << std::endl;
        std::cout << "is_same_type<int, double> " << is_same_type<int, double> << std::endl;
        // std::cout << is_same_type<const int, const int> << std::endl; // Ambiguous partial specializations of 'is_same_type<const int, const int>'
        std::cout << std::noboolalpha;

        // is_same_type<int, decltype(value)> true
        // is_same_type<int, const int> true
        // is_same_type<const int, int> true
        // is_same_type<int, double> false
    }

} // namespace variable_template_specialization_ns
// 偏特化的本质-->>变形，相当于添加了额外的约束条件，模板参数并没有被完全替换
// 偏特化声明中的模板参数列表必须包含所有未被特化的主模板参数
// 被特化的参数则直接替换为具体类型，值，或者模式

// 特化的优先级 全特化优先级 > 偏特化 > 主模板
namespace variable_template_priority_ns {
    // 主模板(优先级最低)
    template <typename T>
    constexpr int level = 0;
    // 偏特化，指针和二级指针
    template <typename T>
    constexpr int level<T *> = 1;
    template <typename T>
    constexpr int level<T **> = 2;
    // 全特化(优先级最高)
    template <>
    constexpr int level<int *> = 3;

    void test_priority() {
        std::cout << std::boolalpha;
        std::cout << "level<int*> " << level<int *> << std::endl;
        std::cout << "level<int**> " << level<int **> << std::endl;
        // level<int*> 3 // 说明全特化优先级高于偏特化
        // level<int**> 2 // 说明偏特化优先级高于主模板
        std::cout << std::noboolalpha;
    }
} // namespace variable_template_priority_ns

int main() {
    variable_template_ns::variable_template();
    variable_template_ns::test_max_value();
    variable_template_specialization_ns::test_typesize();
    variable_template_specialization_ns::test_meter_to_feet();
    variable_template_specialization_ns::test_is_array();
    variable_template_specialization_ns::test_is_same_type();
    variable_template_priority_ns::test_priority();
}