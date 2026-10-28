#include <iostream>
#include <map>
#include <optional>
#include <string>

// https://subingwen.cn/cpp/optional/
// 薛定谔的盒子 - std::optional

// 2. std::optional 操作
// 2.1 初始化和重置
// 这个类的头文件为：#include <optional>，关于std::optional对象的初始化常用的有以下几种方式：

// 默认初始化：如果不提供任何参数，std::optional 会被初始化为不包含任何值的状态。
// 使用 std::nullopt 显式置空：这是 C++ 标准推荐的表示空值的方式，比使用 nullptr 或 {} 更语义化。
// 就地直接构造：使用std::optional要存储的对象对应的类的构造函数，这是最推荐的方式之一。
// 使用 make_optional 辅助函数：类似于 std::make_pair 或 std::make_shared，它可以自动推导类型。
// 隐式类型转换：如果类型 U 可以转换为类型 T，你可以用 U 来初始化 std::optional<T>。
// 重置 / 赋值会改变 std::optional 的状态，可能涉及析构旧对象、构造新对象等逻辑。

// 赋值为 std::nullopt (重置为空)：这是将一个已包含值的 optional 变为空的正确方式。
// 赋值为具体值：如果赋给 optional 一个新值，编译器会根据当前状态自动处理：
// 情况 A（当前为空）：在 optional 内部直接调用 T 的构造函数（拷贝或移动）。
// 情况 B（当前有值）：调用 T 的赋值运算符（operator=）将新值赋给旧对象。
// 使用 reset() 成员函数：reset() 是显式销毁内部值并将其置空的专用函数。
// 使用 emplace() ：emplace() 是最高效的重置并构造方式。它会销毁当前包含的对象，然后直接在 optional
// 的内存空间中使用给定的参数构造一个新对象。 交换 swap(opt)：交换两个 optional
// 对象的状态。如果两者都有值，则交换其内部包含的值；如果状态不同，则按照逻辑移动值。

namespace ns1 {
    std::optional<int> get_data(bool should_return_data) {
        if (should_return_data) {
            return 10;
        } else {
            return std::nullopt;
        }
    }
    void test() {
        // 1. 默认构造：默认是空的
        std::optional<int> a;
        if (a) {
            std::cout << *a << std::endl;
        } else {
            std::cout << "a 当前不包含值" << std::endl;
        }

        // 2. 显式初始化为空
        std::optional<int> b = std::nullopt;
        // 3. 初始化为有值
        std::optional<int> c{ 20 };

        // 4. std::make_optional (类似于 make_pair)
        auto d = std::make_optional<std::string>("hello");
        // 5. int 隐式转换为 double
        std::optional<double> e = 42;
        // 6. 重置为无值状态
        d.reset(); // 方法1: 使用 reset()
        c = std::nullopt; // 方法2: 赋值为 nullopt

        std::optional<std::string> f = "old data";
        // 销毁 "Old Data"，然后直接调用 string(const char*, size_t) 构造函数
        // 这种方式比 "f = std::string("New", 3)" 更高效，因为它不会产生临时 string 对象
        f.emplace("new", 3);

        // 判断是否为空
        if (!d && !c) {
            std::cout << "c 和 d 现在都是空的" << std::endl;
        }
        a = 100;
        b.swap(a);

        std::cout << "a = " << a.value_or(0) << "\n";
        std::cout << "b = " << b.value_or(0) << "\n";
    }
} // namespace ns1

namespace ns2 {
    // 2.1 检查与访问
    // 在使用 std::optional 时，检查其是否包含值（即是否有效）是至关重要的，因为直接解引用一个空的
    // std::optional 会导致未定义行为，通常是程序崩溃。

    // 2.1.1 使用 has_value() 成员函数
    // 使用 has_value() 成员函数这是最直接、语义最清晰的方法。has_value() 返回一个 bool，如果
    // optional 包含值则返回 true，否则返回 false。

    // 适用场景： 逻辑简单，只需要判断是否有值的场合。
    std::optional<int> get_data(bool flag) {
        if (flag) {
            return 42;
        } else {
            return std::nullopt;
        }
    }
    void test() {
        std::optional<int> data = get_data(true);
        if (data.has_value()) {
            std::cout << "有值" << std::endl;
        } else {
            std::cout << "没有值." << std::endl;
        }
    }
} // namespace ns2
namespace ns3 {
    // 2.1.2 显式转换为 bool
    // std::optional 重载了布尔转换运算符。我们可以直接将 optional 对象放在 if 条件判断中。这与
    // has_value() 的效果完全一致，但代码更简洁。

    // 适用场景： 绝大多数日常检查场景，这是现代 C++ 最推荐的写法。
    std::optional<std::string> get_name() {
        return "alice";
    }
    void test() {
        auto name = get_name();
        // 直接判断，代码简洁
        if (name) {
            // 如果 name 包含值，条件为真
            std::cout << "Name: " << *name << std::endl;
            std::cout << "Name: " << name.value() << std::endl;
            std::cout << "Name: " << name.value_or("") << std::endl;
        } else {
            std::cout << "Name is empty." << std::endl;
        }
    }

    //     2.2 值的获取
    // 如果想要取出std::optional中存储的数据常用的方式有三种：

    // 使用解引用操作符 * 或者箭头操作符 ->：
    // 用法就像普通指针一样。
    // 警告：如果 optional 为空时使用这两个操作符，行为是未定义的（通常直接崩溃）。
    // value() 成员函数：
    // 返回值的引用。
    // 安全特性：如果 optional 为空，它会抛出 std::bad_optional_access 异常。
    // value_or() 成员函数：这不仅仅是一个检查方法，更是一个实用的访问模式，它会检查 optional
    // 是否有值： 如果有值，返回该值。 如果为空，返回你指定的默认值。
    void test2() {
        std::optional<std::string> opt1 = "hello world";
        std::optional<std::string> opt2; //

        // --- 方式 1: operator* (不安全，需先检查) ---
        if (opt1) {
            std::cout << "opt1 length: " << (*opt1).length() << "\n";
            std::cout << "opt1 length: " << (*opt1) << "\n";
        }

        // --- 方式 2: operator-> (不安全，需先检查) ---
        if (opt1) {
            std::cout << "opt1 length: " << opt1->length() << "\n";
        }

        // --- 方式 3: value() (抛异常版) ---
        try {
            std::cout << "opt2 " << opt2.value() << std::endl;
        } catch (const std::bad_optional_access &e) {
            std::cout << "cat exception: " << e.what() << "\n";
        }
        // --- 方式 4: value_or() ---
        // 如果 opt2 有值就用 opt2 的值，没值就用 "默认值"
        std::cout << "opt2 或默认值: " << opt2.value_or("Hello Dabing") << std::endl;
    }
} // namespace ns3

namespace ns4 {
    template <typename K, typename V>
    class Config {
    private:
        std::map<K, V> data;

    public:
        void set(const K &key, const V &value) {
            data[key] = value;
        }
        // 返回 optional 表示键可能不存在
        std::optional<V> get_string(const K &key) const {
            auto it = data.find(key);
            if (it != data.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        std::optional<int> get_int(const K &key) const {
            auto str_opt = get_string(key);
            if (!str_opt) {
                return std::nullopt;
            }
            try {
                return std::stoi(str_opt.value());
            } catch (...) {
                return std::nullopt;
            }
        }
        std::optional<double> get_double(const K &key) const {
            auto str_opt = get_string(key);
            if (!str_opt) {
                return std::nullopt;
            }
            try {
                return std::stod(str_opt.value());
            } catch (...) {
                return std::nullopt;
            }
        }
    };
    void test() {
        Config<std::string, std::string> config;
        config.set("server.port", "8080");
        config.set("server.host", "localhost");
        config.set("timeout", "30.5");

        // 安全获取配置
        auto port = config.get_int("server.port");
        if (port) {
            std::cout << "端口: " << *port << std::endl;
        } else {
            std::cout << "端口未配置\n";
        }

        auto timeout = config.get_double("timeout");
        std::cout << "超时: " << timeout.value_or(60.0) << "秒\n";

        // 不存在的配置
        auto missing = config.get_string("missing.key");
        if (!missing) {
            std::cout << "配置项不存在\n";
        }
    }
} // namespace ns4
int main() {
    ns1::test();
    ns2::test();
    ns3::test();
    ns3::test2();
    ns4::test();
}