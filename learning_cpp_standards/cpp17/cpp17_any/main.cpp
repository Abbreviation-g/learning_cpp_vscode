#include <any>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
// https://subingwen.cn/cpp/any/
// 类型安全的任意类型容器 - std::any

namespace ns1 {
    // 1 存入数据：我们可以像赋值一样直接存，或者使用 std::make_any 构造。
    void test1() {
        // 1. 默认构造
        std::any a1;
        std::cout << "a1 has_value: " << a1.has_value() << std::endl; // 0

        // 2. 直接存储各种类型
        std::any a2 = 11;
        std::any a3 = 2.22;
        std::any a4 = std::string("xxx");
        std::any a5 = "c-string"; // const char*

        // 3. 使用std::make_any
        auto a6 = std::make_any<std::vector<int>>(3, 100);
        auto a7 = std::make_any<std::pair<int, std::string>>(1, "ap");
    }
    // 2.检查类型：由于它什么都可以存，取出时必须先确认里面是什么。
    void test2() {
        std::any values[]{ 42, 3.14, std::string("text"), std::vector<int>{ 1, 2, 3 } };

        for (const auto &val : values) {
            if (!val.has_value()) {
                continue;
            }
            // 使用 type() 获取 type_info
            const std::type_info &ti = val.type();
            // 注意：ti.name() 返回的实现定义的名字，可能不易读
            std::cout << "Type: " << ti.name() << std::endl;
            // 根据类型名进行判断（编译器相关，可能不友好）
            if (ti == typeid(int)) {
                std::cout << "  -> It's an int: " << std::any_cast<int>(val) << std::endl;
            } else if (ti == typeid(double)) {
                std::cout << "  -> It's a double: " << std::any_cast<double>(val) << std::endl;
            }
        }
    }

    // 3.    取出数据：取出数据需要使用 std::any_cast，这是最关键的一步。std::any_cast
    // 是从箱子拿东西的唯一工具，但它非常严格，取数据过程中需要遵循以下规则：
    // 类型必须完全匹配：C++ 的类型转换规则在这里不适用。
    // int 和 long 是不同的类型。
    // int 和 int& 是不同的类型（如果存的是 int，必须用 int 取，不能用 int& 直接取）。
    // std::any a = 10;
    // // long b = std::any_cast<long>(a); // 抛出异常！虽然 int 和 long 隐式转换，但 std::any
    // 不允许。 int c = std::any_cast<int>(a);      // 正确 使用指针转换避免异常
    // 如果我们不想用 try-catch，可以使用指针版本的
    // any_cast。当类型不匹配的时候，它不会抛出异常，而是返回 nullptr。
    void test3() {
        // 创建一个 any 对象，存储一个字符串
        std::any data = std::string("Hello, C++17!");

        // 场景1：正确的类型转换 - 指针版本
        // 注意：这里获取的是指针，不是引用！
        std::string *strptr = std::any_cast<std::string>(&data);
        if (nullptr != strptr) {
            std::cout << "转换成功: " << *strptr << std::endl; // 需要解引用
        } else {
            std::cout << "转换失败: nullptr" << std::endl;
        }

        // 场景2：错误的类型转换 - 指针版本
        int *intPtr = std::any_cast<int>(&data);
        if (intPtr != nullptr) {
            std::cout << "转换成功: " << *intPtr << std::endl;
        } else {
            std::cout << "转换失败: 试图将 string 转换为 int" << std::endl;
        }

        // 场景3：修改 any 中的值（通过指针）
        if (strptr != nullptr) {
            *strptr = "已修改";
            std::cout << "修改后的值: " << *std::any_cast<std::string>(&data) << std::endl;
        }

        // 场景4：与引用版本的对比
        try {
            // 引用版本：类型不匹配会抛出异常
            int &intRef = std::any_cast<int &>(data);
            std::cout << "这行不会执行" << std::endl;
        } catch (const std::bad_any_cast &e) {
            std::cout << "引用版本抛异常: " << e.what() << std::endl;
        }

        // 场景5：判断 any 是否存储了特定类型的值
        std::any value = 42;
        // 检查是否存储了 int 类型
        if (auto ptr = std::any_cast<int>(&value)) {
            std::cout << "value 存储的是 int: " << *ptr << std::endl;
        }
        // 检查是否存储了 string 类型
        if (auto ptr = std::any_cast<std::string>(&value)) {
            std::cout << "这不会执行，因为 value 不是 string" << std::endl;
        } else {
            std::cout << "value 不是 string 类型" << std::endl;
        }
    }
} // namespace ns1
namespace ns2 {
    class Config {
    private:
        std::unordered_map<std::string, std::any> settings;

    public:
        template <typename T>
        void set(const std::string &key, const T &value) {
            settings[key] = value;
        }
        template <typename T>
        T get(const std::string &key) const {
            auto it = settings.find(key);
            if (it == settings.end()) {
                throw std::runtime_error{ "key not found: " + key };
            }
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast &e) {
                throw std::runtime_error("Type mismatch for key: " + key);
            }
        }

        template <typename T>
        T get_or_default(const std::string &key, const T &default_value) {
            auto it = settings.find(key);
            if (it == settings.end()) {
                return default_value;
            }
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast &e) {
                return default_value;
            }
        }
    };
    void test() {
        Config config;

        // 存储不同类型的配置
        config.set("window_width", 800);
        config.set("window_height", 600);
        config.set("window_title", std::string("My Application"));
        config.set("fullscreen", false);
        config.set("fps", 60.0);
        config.set("resolution", std::pair<int, int>(1920, 1080));

        // 读取配置
        int width = config.get<int>("window_width");
        std::string title = config.get<std::string>("window_title");
        double fps = config.get_or_default("fps", 30.0);

        std::cout << "Window: " << title << " " << width << "x" << config.get<int>("window_height")
                  << std::endl;
        std::cout << "FPS: " << fps << std::endl;
    }
} // namespace ns2

namespace ns3 {
    // 2.2 类型安全的异构容器
    class HeterogeneousContainer {
    private:
        std::vector<std::any> items;

    public:
        template <typename T>
        void add(T &&value) {
            items.emplace_back(std::forward<T>(value));
        }
        template <typename T>
        std::vector<T *> get_all_of_type() {
            std::vector<T *> result;
            for (auto &item : items) {
                if (auto *ptr = std::any_cast<T>(&item)) {
                    result.push_back(ptr);
                }
            }
            return result;
        }
        void process_all() {
            for (size_t i = 0; i < items.size(); ++i) {
                const auto &item = items[i];
                const std::type_info &ti = item.type();

                std::cout << "Item " << i << ": type = " << ti.name() << ", value = ";
                // 根据类型处理（需要预先知道可能的类型）
                if (ti == typeid(int)) {
                    std::cout << std::any_cast<int>(item);
                } else if (ti == typeid(double)) {
                    std::cout << std::any_cast<double>(item);
                } else if (ti == typeid(std::string)) {
                    std::cout << std::any_cast<std::string>(item);
                } else {
                    std::cout << "[unknown type]";
                }

                std::cout << std::endl;
            }
        }
    };

    void test() {
        HeterogeneousContainer container;

        container.add(10);
        container.add(3.14);
        container.add(std::string("Hello"));
        container.add(20);
        container.add(std::string("World"));
        container.add(2.718);

        // 获取所有整数
        auto ints = container.get_all_of_type<int>();
        std::cout << "ints: ";
        for (int *ptr : ints) {
            std::cout << *ptr << ", ";
        }
        std::cout << "\n";

        // 处理所有元素
        container.process_all();
    }

} // namespace ns3
int main() {
    ns1::test1();
    ns1::test2();
    ns1::test3();

    ns2::test();

    ns3::test();
}