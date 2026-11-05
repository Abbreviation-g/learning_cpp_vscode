#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
// https://subingwen.cn/cpp/map_set/
// std::map/set/unordered_map/unordered_set 的改进

// 1. 节点句柄
// 在 C++17 之前，当我们尝试从一个 std::map 或 std::unordered_map
// 中提取元素并进行修改时，往往不得不面对代码冗余、效率低下甚至查找两次的尴尬局面。C++17
// 针对关联容器（包括 map, set, unordered_map,
// unordered_set）引入了一组至关重要且细节满满的改进。这些改进的核心宗旨是：拒绝冗余查找，拒绝不必要的拷贝，让接口更加统一和易用。

// C++17
// 引入了节点句柄的概念，它是一种轻量级对象，允许在容器间安全转移单个元素的所有权，而无需复制或移动元素本身。这类似于直接操作底层数据结构的节点。

// map 的节点句柄类型是 node_type。
// set 的节点句柄类型是 node_type。
// 对于 map，句柄拥有键和值的完整所有权；对于 set，它拥有键的所有权。
// 节点句柄的关键特性有以下几点：

// 独占所有权：当一个节点被 extract 出来后，它在原容器中就不复存在了。
// 内存保留：句柄持有该节点的内存分配器。这意味着当你把句柄插入到另一个容器时，不需要重新分配内存，只是修改指针指向。这是零分配的操作。
// 修改键的特权：这是唯一能修改 const
// 键的方法。因为节点被提取出来后，它不属于任何容器，此时你可以通过句柄的 key() 方法修改键（仅限
// map，set 的键永远是 const），然后再插入回去。
namespace ns1 {
    // 1.1 提取节点
    // 在 C++17
    // 中std::map、std::set、std::multimap、std::multiset、std::unordered_set、std::unordered_multiset、std::unordered_map、std::unordered_multimap容器中提取节点有两种重载方式，API
    // 如下：

    // // 节点提取成功之后，该节点就从容器中移除了，但节点并未释放
    // node_type extract( const_iterator position );    // (1)
    // node_type extract( const Key& k );               // (2)
    // 参数：

    // position：指向此容器的有效迭代器

    // k：用于标识要提取节点的键

    // 返回值：一个拥有被提取元素的节点句柄，如果在 (2) 中未找到元素，则为空节点句柄。

    // 对于 std::map<K, V> 和 std::unordered_map<K, V> 的节点句柄，这些容器存储 key-value
    // 对，所以它们的节点句柄提供：

    // node.key()      // 返回键（key）的引用，可修改（对于 map，提取后可以安全修改键）
    // node.mapped()   // 返回值（value）的引用
    // 对于 std::set<T> 和 std::unordered_set<T> 的节点句柄，这些容器只存储
    // key（值本身），所以它们的节点句柄只提供：

    // node.value()    // 返回值的引用，可修改
    // // 没有 node.key() 和 node.mapped()！
    // 下面是伪代码，为大家展示了节点句柄的设计：

    // template<typename Key, typename Value>
    // class map_node_handle {
    //     Key& key();          // 对于 map，键可修改（提取后）
    //     Value& mapped();     // 值可修改
    //     // ...
    // };

    // template<typename Key>
    // class set_node_handle {
    //     Key& value();        // 对于 set，值可修改
    //     // ...
    // };
    // 在 C++17 之前，map 的 key 是 const 的，无法修改。如果想修改 key，必须先 erase 旧数据，再
    // insert 新数据。这不仅麻烦，而且如果 new 插入失败（比如已经有相同的
    // key），就把原来的数据弄丢了。C++17 允许我们安全的修改 Key
    // 的值，如果插入会失败，提取的节点中依然持有数据，因此数据是安全的。

    void test() {
        std::map<int, std::string> m{ { 1, "Apple" }, { 2, "Banana" } };
        std::map<int, std::string>::node_type map_node = m.extract(1);
        if (!map_node.empty()) {
            std::cout << "Extracted from map: ";
            std::cout << "key: " << map_node.key();
            std::cout << ", value= " << map_node.mapped();
            std::cout << std::endl;

            map_node.key() = 10; // 修改键
            map_node.mapped() = "Apricot"; // 修改值

            m.insert(std::move(map_node));
        }

        // 2. set 示例 - 只有 value()
        std::set s{ "Red", "Green", "Blue" };
        std::set<const char *>::node_type set_node = s.extract("Red");
        if (!set_node.empty()) {
            std::cout << "Extracted from set: value=";
            std::cout << set_node.value() << "\n";
            set_node.value() = "Crimson"; // 修改值
            s.insert(std::move(set_node));
        }

        // 3. unordered_map 示例 - 也有 key() 和 mapped()
        std::unordered_map<int, double> um{ { 1, 3.14 }, { 2, 2.17 } };
        std::unordered_map<int, double>::node_type um_node = um.extract(1);
        if (!um_node.empty()) {
            std::cout << "Extracted from unordered_map: ";
            std::cout << "key: " << um_node.key();
            std::cout << ", value:" << um_node.mapped() << std::endl;
        }

        // 传统方式（需要删除 + 插入）：
        // 两次内存操作（释放 + 分配）
        // 值被复制（可能开销大）
        // 键的 const 限制无法绕过
        // 1. 查找并获取值
        auto it = m.find(1);
        if (it != m.end()) {
            std::string value = it->second; // 复制值
            m.erase(it); // 删除节点（内存释放）
            m[10] = value; // 插入新节点（内存分配 + 复制）
        }
    }

    // 1.2 合并容器
    // merge() 允许我们将一个容器中的所有节点剪切到另一个容器中，具体行为如下：

    // 无拷贝，无移动构造函数调用：由于是转移所有权，不会触发 Key 或 Value
    // 的拷贝/移动构造函数，只转移内存控制权。 冲突处理： 如果源容器和目标容器中有重复的 Key（对于
    // map/set），源节点不会被插入，也不会被销毁，而是留在源容器中。这是 merge 的不修改冲突源原则。
    // 对于 unordered_map，如果发生哈希冲突或 Key 冲突，行为同上。
    void test2() {
        // 合并两个 map
        std::map<int, std::string> map1 = { { 1, "A" }, { 2, "B" } };
        std::map<int, std::string> map2 = { { 2, "C" }, { 3, "D" } };
        // 合并 map2 到 map1
        map1.merge(map2);
        auto print_map = [](const auto &items) {
            for (const auto &[key, value] : items) {
                std::cout << key << ": " << value << ", ";
            }
            std::cout << "\n";
        };

        print_map(map1);
        print_map(map2);

        // 合并两个 set
        std::set<int> set1 = { 1, 2, 3 };
        std::set<int> set2 = { 3, 4, 5 };
        set1.merge(set2);

        auto print_set = [](const auto &items) {
            for (const auto &value : items) {
                std::cout << value << ", ";
            }
            std::cout << "\n";
        };
        print_set(set1);
        print_set(set2);

        // map1: {1:"A", 2:"B", 3:"D"}：键冲突时保留 map1 的值
        // map2: {2:"C"}：键 2 的节点因冲突留在 map2 中
        // set1: {1, 2, 3, 4, 5}：元素值冲突时保留 set1 的值
        // set2: {3}：重复元素留在 set2
    }
} // namespace ns1
namespace ns2 {
    // 2. try_emplace()
    // 在 C++17 之前，我们经常使用 emplace 来原地构造元素。但 emplace 有一个缺陷：即使 key
    // 已经存在，它也会构造 value 对象，然后再丢弃。这在 value 构造成本高昂时是巨大的性能浪费。
    // 解决方案就是使用 C++17 中为容器添加的新函数 try_emplace，它能够保证只有当插入成功（即 key
    // 不存在）时，才会调用 value 的构造函数。函数签名如下：
    void test1() {
        std::map<int, std::string> m{ { 1, "old" } };
        // 尝试插入 key=1，构造参数为 "New String"
        // 结果：key 1 已存在，"New String" 根本不会被构造，直接返回 false
        auto [it, success] = m.try_emplace(1, "new");
        if (!success) {
            std::cout << "插入失败，没有浪费资源构造字符串" << std::endl;
        }
    }

    void test2() {
        // map 的 value 是 tuple
        std::map<int, std::tuple<std::string, int, double>> m1;
        // 使用 make_tuple
        auto [it, inserted1] = m1.try_emplace(1, std::make_tuple("apple", 100, 1.5));
        // 使用 tuple 的构造函数
        auto [it2, inserted2] =
            m1.try_emplace(2, std::tuple<std::string, int, double>{ "banana", 200, 2.0 });

        // map 的 value 是 pair<string, pair<int, double>>
        std::map<int, std::pair<std::string, std::pair<int, double>>> m2;
        auto [it3, inserted3] = m2.try_emplace(3, "cherry", std::make_pair(300, 3.5));
    }
} // namespace ns2

namespace ns3 {
    // 3. insert_or_assign()
    // operator[] 是 map 最常用的操作，但它有两个副作用：

    // 如果 key 不存在，它会默认构造一个 value。
    // 它总是返回一个引用，无法让你知道这次是插入了新元素，还是赋值更新了旧元素。
    // C++17 中给map容器添加了新的 API 函数insert_or_assign，它结合了 insert 和 try_emplace
    // 的优点，专门用于赋值语义。函数签名如下：

    // template <class M>
    // pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj);
    // 如果 Key 不存在：将 obj 转发给 value 的构造函数（类似于
    // try_emplace，不会先默认构造再赋值，效率更高）。 如果 Key 存在：将 obj 赋值给已有的
    // value（调用 operator=）。 返回 pair<iterator, bool>：bool
    // 表示是否发生了插入（true）还是赋值（false）。

    struct RefType {
        RefType() = delete; // 禁用默认构造函数
        RefType(int) {}
    };
    void test() {
        std::map<int, std::string> m;
        // 场景 1: Key 不存在 -> 插入
        auto [it1, inserted1] = m.insert_or_assign(1, "hello");
        std::cout << "inserted1: " << inserted1 << "\n";
        // 场景 2: Key 存在 -> 赋值
        auto [it2, inserted2] = m.insert_or_assign(1, "world");
        std::cout << "inserted2: " << inserted2 << "\n";

        std::map<int, RefType> m1;
        // m1[1] = 10; // error
        m1.insert_or_assign(1, 10);
    }
} // namespace ns3

namespace ns4 {
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
        A a{ 10086 };
        B b{ 100 };
        a = static_cast<A>(b);
        std::cout << "a.a: " << a.get() << std::endl;

        B converted = static_cast<B>(a);
        std::cout << "converted.b: " << converted.get() << std::endl;
    }
} // namespace ns4
int main(int argc, char *argv[]) {
    ns1::test();
    ns1::test2();

    ns2::test1();
    ns2::test2();

    ns3::test();

    ns4::test();
}