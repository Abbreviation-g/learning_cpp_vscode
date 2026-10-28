#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// https://subingwen.cn/cpp/string_view/
// 字符串视图（非拥有视图）- string_view

namespace string_view_ns1 {
    //     1. std::string_view 的登场
    // 在 C++17 之前，当我们需要编写一个函数来处理字符串时，通常会面临一个艰难的选择：

    // 使用 const std::string&
    // 优点：安全，避免拷贝。
    // 缺点：如果参数是一个字符串字面量（如 "hello"）或者是一个 C 风格字符串（如
    // char*），编译器必须隐式地创建一个临时的 std::string
    // 对象。这意味着会发生内存分配，这是非常昂贵的操作！ 使用 const char*
    // 优点：速度快，没有内存分配，可以接受字面量。
    // 缺点：不安全，不能获取字符串长度（必须调用 strlen），不能直接使用 STL 算法，一旦涉及到
    // std::string 成员就麻烦了。 std::string_view 是 C++17
    // 引入的一个只读引用包装器。我们可以把它想象成指向字符串的窗户：它只负责观察字符串的数据，而不拥有字符串的数据。

    // std::string_view 类 API 在线查询
    // std::string_view 的构造函数设计得非常宽容，下面这些类型都可以隐式转换为 string_view：

    // 标准字符串：std::string
    // C 风格字符串：const char*
    // 字符串字面量："hello world"
    // 字符数组：char buffer[100]
    // 迭代器范围：任意指向字符的 begin/end 迭代器对（如 std::vector<char> 的子集）。
    // 其他 string_view：从另一个 string_view 构造。
    // 如果编写一个函数需要接收这些不同类型的字符串，我们可以将函数参数声明为
    // std::string_view，这样就可以直接传入上述任何一种类型，而无需修改调用代码。

    // // 以前：你需要三个重载
    // void print(const std::string& s);
    // void print(const char* s);
    // void print(const std::vector<char>& v);

    // // 现在：只需要一个
    // void print(std::string_view sv); // 自动适配所有上述类型
    // 零开销是 C++
    // 的核心哲学，这意味着：不使用某个特性时，不需要为它付出代价；当使用它时，不会付出比手动实现更高的代价。string_view
    // 做到了几乎零开销。所谓的几乎，仅仅是指它本身作为对象存在时占用极小的栈内存（通常是指针和大小共
    // 16 字节），除此之外几乎没有额外代价。具体体现在 以下三个方面：

    // 没有内存分配：这是 string_view 最显著的优势，也是与 std::string 最大的区别

    // 当构造一个 std::string 时（例如从 const
    // char*），通常会涉及到动态内存分配（堆内存申请），以便存储字符数据的副本。如果频繁发生（如在循环中创建临时对象），会严重拖累性能，并造成内存碎片。
    // 当构造一个 string_view
    // 时：它只读不拥有。它只是简单地记录了原始字符串的起始地址和一个长度。底层的数据仍然留在原来的地方（静态区、堆上或栈上），没有任何
    // malloc 或 new 操作。 避免原始数据拷贝

    // std::string
    // 的拷贝：为了拥有数据所有权，拷贝一个字符串需要将每一个字符从源内存复制到目标内存。如果字符串很长，这个开销是巨大的。
    // string_view 的拷贝：拷贝一个 string_view 只需要拷贝两个标量值（指针 ptr 和长度
    // len）。无论它指向的字符串是 10 个字符还是 10GB，拷贝 string_view 的时间复杂度永远是
    // **O(1)**，且速度极快（等价于拷贝两个整数）。 零虚函数开销

    // string_view 是一个轻量级的类模板（类似于
    // struct），它内部没有任何虚函数。这意味着它既没有虚函数表指针带来的内存占用，也没有通过虚函数表进行间接函数调用的运行时开销。现代编译器可以对其进行极其激进的优化，甚至完全优化掉其存在。

    // 下面的示例代码演示了std::string_view 对象的创建与初始化：
    void test1() {
        // 1. 从 C 风格字符串构造
        const char *cstr = "hello world c str";
        std::string_view sv1{ cstr };

        // 2. 从 std::string 构造
        std::string str = "hello std:string";
        std::string_view sv2{ str };

        // 3. 从字面量直接构造（最常用）
        std::string_view sv3{ "hello literal" }; // 没有临时 std::string 创建！

        // 4. 从字符数组构造
        char arr[]{ "hello array" };
        std::string_view sv4{ arr };

        // 5. 从另一个 string_view 构造
        std::string_view sv5 = sv3;

        // 6. 带长度的构造（用于处理非空终止的字符串）
        const char data[]{ 'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'd' };
        std::string_view sv6{ data, 5 };

        std::cout << "sv1: " << sv1 << std::endl;
        std::cout << "sv2: " << sv2 << std::endl;
        std::cout << "sv3: " << sv3 << std::endl;
        std::cout << "sv4: " << sv4 << std::endl;
        std::cout << "sv5: " << sv5 << std::endl;
        std::cout << "sv6: " << sv6 << std::endl;

        // sv1: hello world c str
        // sv2: hello std:string
        // sv3: hello literal
        // sv4: hello array
        // sv5: hello literal
        // sv6: Hello
    }

    //     2.1 只读视图
    // string_view 提供了和 std::string 类似的接口，但都是只读的：
    void test2() {
        std::string_view sv{ "hello world!" };

        // 访问元素
        std::cout << "first char: " << sv[0] << "\n";
        std::cout << "front char: " << sv.front() << "\n";
        std::cout << "back char: " << sv.back() << "\n";

        // 大小信息
        std::cout << "size: " << sv.size() << "\n";
        std::cout << "length: " << sv.length() << "\n";
        std::cout << "empty: " << sv.empty() << "\n";

        // 数据访问（注意：可能不是空终止的！）
        std::cout << "data pointer: " << static_cast<const void *>(sv.data()) << "\n";

        // 迭代器（可以用于 range-based for 循环）
        for (char c : sv) {
            std::cout << c;
        }
        std::cout << std::endl;

        // 比较操作（与 string_view、std::string、字面量比较）
        std::string_view sv2{ "hello" };
        std::cout << "Compare: " << (sv < sv2) << std::endl; // false

        // string_view 只是一个观察者，它并不保证你看到的数据末尾有一个
        // \0（空字符）。接下来我们对比一下 std::string 和 string_view 的区别：
        // C++ 字符串

        // 在 C 和 C++ 中，标准的 C 风格字符串（const char*）和 std::string 总是以一个空字符 \0
        // 结尾。 因为末尾有 \0，像 strlen、printf 或 std::cout << char*
        // 这样的函数只要拿到一个指针，就可以一直向后读，直到遇到 \0 停下。
        // std::string_view：string_view是由一个指针（起始位置）和一个长度组成的，它不像 std::string
        // 那样强制要求字符串必须有 \0。它只关心从指针开始，往后数 N 个字节都是我的数据。

        // 这就导致了两种危险情况：

        // 子字符串切片：从一个字符串中取子串时，string_view 仅仅是指向了中间的某一段。

        // std::string s = "Hello, World!";
        // std::string_view sv(s.data(), 5); // 指向 "Hello" 这一部分
        // // sv 内部只记录了 长度=5
        // sv 的末尾是逗号 ,再往后紧跟着空格，根本没有 \0。

        // 非空终止的缓冲区：可以直接从字符数组构造 string_view，那个数组可能压根就没放 \0。

        // char buffer[] = {'H', 'i', '!'}; // 注意：没有 \0
        // std::string_view sv(buffer, 3);
        // 再看示例中的这行代码：

        // std::cout << "Data pointer: " << static_cast<const void*>(sv.data()) << std::endl;
        // sv.data()：返回指向字符串起始位置的 const char*。
        // static_cast<const
        // void*>(...)：这是一个关键的类型转换。它强行把这个指针当成无类型指针输出。 为什么要转成
        // void*？

        // 如果不转，直接写 std::cout << sv.data()，std::cout 会以为我们传给它一个 C
        // 风格字符串（const char*），它就会尝试从那个地址开始打印字符，直到遇到 \0 才停止。

        // 结合上面描述的两种危险情况，如果我们打印的是 sv("Hello", 5)：

        // sv.data() 返回指向 'H' 的指针。
        // 如果不转 void*，cout 会打印：Hello, World!（它不知道 sv
        // 认为字符串在第5个字符结束了，它会一路读下去，直到原来的 string 结尾）。 如果转成
        // void*，cout 就只会打印这个内存地址（例如 0x7ffee4），而不会试图去读取里面的内容。
    }

    // 2.2 零开销子串操作
    // 子串操作这是 std::string_view 最强大的功能之一。std::string 的 substr 操作会返回一个新的
    // std::string
    // 对象，这意味着新的内存分配和数据拷贝。如果你只需要读取字符串的一部分，这纯粹是浪费。而
    // std::string_view 的 substr 操作只调整指针和长度，完全不拷贝数据。
    void test3() {
        std::string_view sv = "The quick brown fox jumps over the lazy dog";
        // 获取子串：不复制，只是调整指针和长度！
        std::string_view sub1 = sv.substr(4, 5); // quick
        std::string_view sub2 = sv.substr(16, 3); // fox
        std::cout << "Original: " << sv << std::endl;
        std::cout << "Substring 1: " << sub1 << std::endl;
        std::cout << "Substring 2: " << sub2 << std::endl;

        // 获取到结尾的子串
        std::string_view suffix = sv.substr(31); // the lazy dog
        std::cout << "Suffix: " << suffix << std::endl;

        // 检查内存地址：子串和原串共享内存！
        std::cout << "sv.data() address: " << static_cast<const void *>(sv.data()) << std::endl;
        std::cout << "sub1.data() address: " << static_cast<const void *>(sub1.data()) << std::endl;
        // 注意：sub1.data() 的地址 = sv.data() + 4

        //         std::string_view 内部通常只包含两个成员（类似于 struct { const char* ptr; size_t
        //         len; }）：

        // 指针：指向字符串数据的起始位置。
        // 长度：字符串的长度。
        // 当我们调用 std::string_view sub1 = sv.substr(4, 5); 时：

        // 没有内存分配：完全不涉及 new 或 malloc。
        // 没有内存复制：CPU 不需要搬运任何字符数据。
        // 仅修改指针和长度：编译器只需要生成几条极其简单的指令：
        // 计算新指针：新指针 = 旧指针 + 4
        // 设定新长度：新长度 = 5
    }

    //     2.3 移除前缀和后缀
    // std::string_view 非常适合解析和分割操作。它提供了两个极其强大的成员函数来修改视图范围：

    // remove_prefix(size_t n)：从视图的起始位置移除 n 个字符。视图向后滑动。
    // remove_suffix(size_t n)：从视图的末尾移除 n 个字符。视图向前收缩。
    // 这两个方法都是 O(1) 操作，它们只修改内部存储的 指针 和 长度 变量。
    void test4() {
        std::string_view sv = "Hello, World!";

        // 移除前缀
        std::string_view prefix_removed = sv;
        prefix_removed.remove_prefix(7); // 移除前 7 个字符
        std::cout << "After remove_prefix(7): " << prefix_removed << std::endl; // "World!"

        // 移除后缀
        std::string_view suffix_removed = sv;
        suffix_removed.remove_suffix(7); // 移除后 7 个字符
        std::cout << "After remove_suffix(7): " << suffix_removed << std::endl; // "Hello"

        // 组合使用：解析类似 "key=value" 的字符串
        std::string_view kv = "name=John";
        auto eq_pos = kv.find('=');
        if (eq_pos != std::string_view::npos) {
            std::string_view key = kv.substr(0, eq_pos);
            std::string_view value = kv.substr(eq_pos + 1);
            std::cout << "Key: " << key << ", Value: " << value << std::endl;
        }

        //         std::string_view 的原地修改能力，使其成为了无拷贝解析器的理想构建块。

        // 它把 字符串处理 变成了 指针游走。
        // 它把 数据拷贝 变成了 整数加减。
        // 在写高性能代码时，只要发现自己在不断地 substr 并扔掉旧字符串，请立即考虑使用
        // remove_prefix/remove_suffix 来重构你的逻辑！
    }

    //     2.4 与 std::string 的互转
    // std::string_view
    // 是只读的，核心原因是它不拥有数据。因此不能通过它修改字符串的内容。如果需要修改，可以把它转换回
    // std::string。

    // std::string_view 提供的接口全是 const 的。例如：

    // front() 返回的是 const char&
    // operator[] 返回的是 const char&
    // 没有 data() 的 非 const 重载版本（这与 C++17 的 std::string::data() 不同）。
    // C++17 后，std::string 增加了接受 string_view 版本的构造函数和函数：
    void test5() {
        std::string_view sv{ "hello" };
        std::string s{ sv };
        s += " world"; // 现在 s 可以被修改了
        std::cout << s << std::endl;

        std::string str;
        str += sv; // 追加
        str.append(sv); // 追加
        str.assign(sv); // 赋值
        str.insert(0, sv); // 插入
        str.replace(0, 2, sv); // 替换

        bool equal = (str == sv);
        int result = str.compare(sv);
        std::cout << "str = " << str << ", sv = " << sv << std::endl;
        std::cout << "equal = " << equal << ", result = " << result << std::endl;
    }

    //     3. 实战案例
    // 在很多高级编程中，我们需要拆分字符串。传统做法会生成很多小 string
    // 对象，导致大量内存分配。使用 string_view 可以实现零拷贝拆分。

    std::vector<std::string_view> split(std::string_view str, char delim) {
        // 返回 string_view 的 vector，不包含任何字符串数据的拷贝
        std::vector<std::string_view> result;

        while (!str.empty()) {
            // 1. 查找分隔符
            size_t pos = str.find(delim);
            if (pos == std::string_view::npos) {
                // 没找到分隔符，把剩下的部分加入结果
                result.push_back(str);
                break;
            }
            // 2. 截取子串 (零拷贝)
            result.push_back(str.substr(0, pos));
            // 3. 调整视图，移除已经处理的部分和分隔符
            str.remove_prefix(pos + 1);
        }

        return result;
    }
    void test6() {
        std::string data = "apple,orange,banana,grape";
        auto parts = split(data, ',');
        for (const auto &part : parts) {
            std::cout << part << "\n";
        }

        //         std::string_view::npos 是 C++
        //         标准库中定义的一个非常关键的静态常量，用于处理字符串查找操作中的“未找到”或“最大长度”的情况。简单来说，它是
        //         std::string_view 内部类型 size_t 的最大值。含义有两种：

        // 表示 无效位置 或 未找到。
        // 表示 直到字符串末尾。
        // 所有其核心用途也是有两种：

        // 查找操作的返回值

        // std::string_view::npos 最常见的用途是检查查找函数（如 find, rfind, find_first_of
        // 等）是否成功。

        // 截取剩余所有字符

        // 利用 npos 作为计数参数，可以方便地表示从某处开始，一直到字符串结束。这在使用 substr
        // 时非常常见。通常 substr 的用法是：substr(pos = 0, len = npos)。

        // substr函数内部的逻辑大致是：

        // 实际长度 = min(请求长度, 剩余长度)
        // 请求长度 是 npos（巨大无比），所以 min 操作会自动选取 剩余长度。这样就实现了取到末尾。
    }
} // namespace string_view_ns1
int main() {
    string_view_ns1::test1();
    string_view_ns1::test2();
    string_view_ns1::test3();
    string_view_ns1::test4();
    string_view_ns1::test5();
    string_view_ns1::test6();
}