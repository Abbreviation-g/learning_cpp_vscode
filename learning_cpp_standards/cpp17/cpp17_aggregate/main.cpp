#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
// https://subingwen.cn/cpp/aggregate17/
// 带有基类的聚合类
namespace ns1 {
    struct Base {
        int x;
    };
    struct Derived : public Base {
        static int y;
    };
    struct NonDerived {
        int a;
        NonDerived() {}
    };
    struct NonDerived1 : virtual public Base {
        int y;
    };
    struct NonDerived2 : public Base {
        // virtual void print(){}
    private:
        int num;
    };
    void test() {
        std::cout << std::boolalpha;
        std::cout << "Base is aggregate: " << std::is_aggregate_v<Base> << std::endl; // true
        std::cout << "Derived is aggregate: "
                  << std::is_aggregate_v<Derived> << std::endl; // true (C++17)
        std::cout << "NonDerived is aggregate: "
                  << std::is_aggregate_v<NonDerived> << std::endl; // false
        std::cout << "NonDerived1 is aggregate: "
                  << std::is_aggregate_v<NonDerived1> << std::endl; // false
        std::cout << "NonDerived2 is aggregate: "
                  << std::is_aggregate_v<NonDerived2> << std::endl; // false
    }
} // namespace ns1

namespace ns2 {
    //     2.1 聚合类初始化
    // 在使用{}初始化聚合类对象的时候，初始化顺序为先初始化基类部分，再初始化派生类成员。
    struct Person {
        std::string name;
        int age;
    };
    struct Employee : public Person {
        std::string department;
        double salary;
        int employee_id;
    };
    void test() {
        std::cout << "=== 基本聚合类示例 ===" << std::endl;

        // 聚合初始化
        // 方式1: 从父类到子类, 按照数据成员顺序依次初始化
        Employee emp{ "张三", 32, "技术支持", 10020.9, 1001 };
        // 方式2: 将父类数据写到 {} 作为一个整体, 它是子类数据的一部分
        Employee emp2{ { "里斯", 33 }, "客服", 12000.7, 2002 };
        // 访问成员
        std::cout << "姓名: " << emp.name << std::endl;
        std::cout << "年龄: " << emp.age << std::endl;
        std::cout << "部门: " << emp.department << std::endl;
        std::cout << "薪资: " << emp.salary << std::endl;
        std::cout << "员工ID: " << emp.employee_id << std::endl;
    }
} // namespace ns2
namespace ns3 {
    // 2.2 结构化绑定
    // 虽然在结构化绑定中直接解包带有基类的聚合比较少见，但聚合初始化为结构化绑定提供了底层支持。下面是C++17
    // 中结构化绑定到数据成员模式的硬性规则：所有非静态 public
    // 数据成员，必须全部来自同一个类：要么全部是派生类直接成员，要么全部来自同一个无歧义的 public
    // 基类；不能一部分在基类（如果是多重继承也只能有一个基类中有数据成员）、一部分在派生类。

    struct RGBColor {
        int red;
        int green;
        int blue;
    };
    struct NamedColor : public RGBColor {
        std::string name;
    };
    struct Alpha {
        static constexpr uint8_t alpha{ 255 };
    };
    struct NameColor : public Alpha {
        std::string name;
        int hex;
    };
    void test() {
        std::cout << "=== 聚合类与结构化绑定 ===" << std::endl;

        // 基本聚合类的结构化绑定
        RGBColor color{ 255, 128, 64 };
        auto [r, g, b] = color;
        std::cout << "RGB颜色: (" << r << ", " << g << ", " << b << ")" << std::endl;

        // 带有基类的聚合类
        NamedColor namedColor{ { 200, 100, 50 }, "橙红色" };
        // 手动解构派生类
        std::cout << "命名颜色: " << namedColor.name << " RGB: (" << namedColor.red << ", "
                  << namedColor.green << ", " << namedColor.blue << ")" << std::endl;

        // 使用结构化绑定解构数据
        NameColor c1{ {}, "Blue", 0x0000ff };
        auto [name, hex] = c1;
        std::cout << name << ", " << hex << "\n";
    }
} // namespace ns3
int main() {
    ns1::test();

    ns2::test();

    ns3::test();
}