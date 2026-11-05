#include <iostream>
// https://subingwen.cn/cpp/enum-init/
// 枚举的直接列表初始化 - 对 enum class 的改进

namespace ns1 {
    // 1. 普通枚举
    enum OldEnum { A = 10, B = 20 };

    // 2. 作用域枚举，指定底层类型为 unsigned char
    // 这种情况在底层硬件开发中很常见
    enum class Status : unsigned char { OK = 0, Error = 1, Unknown = 99 };

    // 3. 另一个枚举，底层类型是 int
    enum class Permission : int { Read = 1, Write = 2, Exec = 4 };

    void test() {
        std::cout << "=== C++17 枚举直接列表初始化测试 ===" << std::endl;

        // --- 场景 A：普通枚举 ---
        std::cout << "\n[普通枚举测试]" << std::endl;
        // 经测试 g++、msvc 都不支持 C++17 的 {} 语法
        // OldEnum e1{ 10 };

        // --- 场景 B：enum class (重点来了) ---
        std::cout << "\n[enum class 测试]" << std::endl;

        // 旧做法 C++17 之前：
        Status s111 = static_cast<Status>(50);

        // 新做法 C++17：
        Status s1{ 51 }; // ✅ 完美！直接用大括号赋予一个未定义的值
        Status s2 = { Status::OK }; // ✅ 当然也可以用已有的枚举值
        Status s3{ 255 }; // ✅ 只要不超过 unsigned char (0-255) 的范围

        std::cout << "Status s1 的原始值: " << static_cast<int>(s1) << std::endl;
        std::cout << "Status s2 的原始值: " << static_cast<int>(s2) << std::endl;
        std::cout << "Status s3 的原始值: " << static_cast<int>(s3) << std::endl;

        // --- 场景 C：错误的示范  ---
        // 1. 忘记用大括号，直接用等号
        // Status bad1 = 100; // ❌ 编译错误：不能将 'int' 转换为 'Status'

        // 2. 用圆括号构造
        // Status bad2(100); // ❌ 编译错误：没有匹配的构造函数

        // 3. 超出底层类型的范围
        // Status bad3 {999}; // ❌ 编译错误：从 'int' 转换到 'unsigned char' 缩窄了范围
        // 解释：999 存不进 unsigned char，C++ 列表初始化为了安全，禁止这种缩窄转换。

        // --- 场景 D：实际应用 - 位掩码组合 ---
        std::cout << "\n[实际应用：组合权限]" << std::endl;
        Permission p{ 1 | 2 };
        std::cout << "组合权限 p 的值: " << static_cast<int>(p) << std::endl;
    }

    //     为了帮大家在面试或使用中记住enum class 这个特性，请记住以下三大要点：

    // 必须用大括号 {}：

    // 只有 Color c { value }; 是对的。Color c (value); 和 Color c = value; 在 enum class
    // 下依然被禁止。

    // 底层类型要匹配：给的值必须能塞进枚举的底层类型里。

    // 如果是 enum class E : char，你不能给它赋值 999，因为 char 装不下。这种“缩窄”是不允许的。

    // 不需要在 enum 列表里：

    // {} 里的数字不一定要出现在枚举定义 { A, B... }
    // 中。这对处理协议里的保留位或未来扩展值非常有用。

} // namespace ns1
int main() {
    ns1::test();
}