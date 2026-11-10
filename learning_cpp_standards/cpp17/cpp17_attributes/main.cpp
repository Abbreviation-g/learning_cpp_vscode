#include <cctype>
#include <iostream>
// https://subingwen.cn/cpp/attributes/
// 属性增强

namespace ns1 {
    // 1. 什么是属性
    //  在 C++ 中，属性
    //  是一种给编译器下指令的机制。它允许程序员告诉编译器某些额外的信息，比如“不要警告我”、“这段代码可能会失败”或者“把这个函数放到某个特定的段”。

    // 属性的标准写法是用 两个方括号 [[ ... ]] 括起来：

    // [[标签]] 代码
    // 在 C++17 之前，大家最常使用的属性是
    // [[deprecated]]，用来标记某个函数或变量已经被弃用，如果别人还在用，编译器就会发出警告。

    // 假设你写了一个旧版本的函数，后来发现更好的实现方法，但为了兼容性又不能立刻删除旧函数。

    // 告诉编译器：这个函数过时了, 不带文字描述
    [[deprecated]] void old_func() {}

    // 旧的实现方式（被标记为过时）
    [[deprecated("请使用新的 fast_calculate() 函数")]]
    int calculate_old_way() {
        return 100;
    }

    // 新的实现方式
    int fast_calculate() {
        return 100; // 更快的算法
    }
} // namespace ns1

namespace ns2 {
    // 2. C++17 新增属性
    // C++17 主要引入了三个新特性：

    // [[nodiscard]]：防止忽略返回值（非常重要！）。
    // [[maybe_unused]]：消除未使用变量的警告。
    // [[fallthrough]]：虽然属于 switch
    // 语句优化，但也算属性相关。但这部分通常归类于语句属性，我们这里重点讲前两个最常用的，外加对所有属性的通用性增强。
    // 2.1 [[nodiscard]]
    // 想象你写了一个检查密码是否正确的函数，它返回 true 或 false。但是，调用这个函数的人可能忘了写
    // if 判断，直接调用了就不管了。这就意味着程序不管密码对错都继续往下跑，这会产生严重的安全漏洞！

    // 解决方案就是在函数返回类型前面加上
    // [[nodiscard]]。如果程序员调用了这个函数却没有处理返回值（比如没有赋值给变量，也没有放在 if
    // 里），编译器会直接报错或发出严重警告。

    // 2.1.1 标记普通函数

    // 定义一个检查文件是否存在的函数
    // [[nodiscard]] 告诉调用者：你必须检查返回值！
    [[nodiscard]] bool check_file_exists(const std::string &filename) {
        std::cout << "正在检查文件: " << filename << "..." << std::endl;
        // 假装做了一些检查逻辑
        return false;
    }

    // 也可以用于枚举类
    enum class [[nodiscard]] ErrorCode : int { OK, NetworkError, FileSystemError };

    ErrorCode do_something() {
        return ErrorCode::NetworkError;
    }

    void test() {
        std::cout << "=== 测试 [[nodiscard]] ===" << std::endl;

        // 场景 1：正确用法 - 处理了返回值
        if (check_file_exists("data.txt")) {
            std::cout << "文件找到了！" << std::endl;
        } else {
            std::cout << "文件不存在。" << std::endl;
        }

        std::cout << "\n尝试不处理返回值..." << std::endl;

        // 场景 2：错误用法 - 忽略了返回值
        // 如果你的编译器支持 C++17 且开启了相应警告级别，下面这行代码会报错或警告
        check_file_exists("ignore_me.txt");

        // 场景 3：忽略枚举示例
        do_something(); // 同样会警告
    }

    // 编译器可能会提示：warning: ignoring return value of 'int transferMoney()',
    // declared with attribute 'nodiscard': '转账操作必须检查结果，防止资金丢失'
    [[nodiscard("转账操作必须检查结果，防止资金丢失")]]
    int transferMoney() {
        // ... 执行转账
        return 0; // 0 成功, -1 失败
    }
    void test2() {
        transferMoney();
    }

    //     2.1.2 标记类/结构体
    // 类加 [[nodiscard]]表示所有按值返回该类对象的函数都会触发检查
    class [[nodiscard("类加 [[nodiscard]]表示所有按值返回该类对象的函数都会触发检查")]]
    ErrorResult {
    public:
        int code;
        std::string msg;
    };
    ErrorResult readFile() {
        return ErrorResult{};
    }
    void test3() {
        readFile();

        auto res = readFile();
    }
} // namespace ns2

namespace ns3 {
    // 2.2 [[maybe_unused]]
    // 在开发过程中，我们经常会遇到变量定义了但没用的情况。

    // 比如你写了一个函数，参数 int flag 为了将来扩展预留的，现在还没用到。
    // 或者你在 Debug 模式下定义了变量，但在 Release 模式下被宏优化掉了。
    // 编译器看到未使用的变量会很唠叨，输出一堆黄色的警告信息。这些警告多了，反而会淹没真正严重的错误。如果不想这样就可以在变量、函数、参数等声明前加上
    // [[maybe_unused]]，编译器会对这个变量睁一只眼闭一只眼，不再报未使用的警告。

    void oldStyleFunc(int a, int b) {
        (void)a; // 告诉编译器：我知道 a 没用，别吵
        std::cout << "b 的值是: " << b << std::endl;
    }
    [[maybe_unused]] void newStyleFunc(int a, int b) {
        [[maybe_unused]] int x = 10;
        std::cout << "b 的值是: " << b << std::endl;
    }
    // 一个更真实的场景：用于预处理宏导致的代码差异
    void processRequest([[maybe_unused]] bool debugMode) {
        std::cout << "处理请求中..." << std::endl;
#ifdef DEBUG
        if (debugMode) {
            std::cout << "Debug 模式开启！" << std::endl;
        }
#endif
        // 如果没有定义 DEBUG，上面那个 debugMode 参数就没用
        // 加上 [[maybe_unused]] 就不会报警告了
    }
    void test() {
        std::cout << "=== 测试 [[maybe_unused]] ===" << std::endl;

        newStyleFunc(100, 200);
        processRequest(true);
    }
} // namespace ns3

namespace ns4 {
    // 2.3 [[fallthrough]]
    // 在 C++（以及 C 语言）的传统语法中，switch 语句有一个著名的特性叫做
    // “直落”。它的意思是：如果在一个 case 分支的代码执行完后，没有写 break 语句，程序会直接穿过这个
    // case 的限制，继续执行下一个 case 里面的代码。也就是说直落是具有两面性的：

    // 优点：有时我们希望不同的条件共用一段逻辑，写起来很方便。
    // 缺点：大多数时候，忘记写 break 是严重的程序 Bug（这叫“逻辑穿透”），而且很难发现。
    // 在 C++17 之前，编译器看到没用 break
    // 就穿透到下一个case的情况，通常会发出警告（Warning）。但程序员如果是故意想穿透的，这个警告就很烦人。为了解决这个问题，C++17
    // 引入了 [[fallthrough]]。它的作用就是告诉编译器：“不用警告，我是故意这么写的！”

    // [[fallthrough]] 必须作为一个单独的语句出现，并且放在非空的 case 代码块的末尾（最后一句）。

    // 假设我们有这样一个需求：检查成绩等级。
    //  如果是 A，打印“优秀”。
    //  如果是 B，打印“良好”。
    //  如果是 C 或 D，都打印“及格”。
    //  否则“不及格”。
    //  对于 C 和 D，我们就可以用穿透。
    void checkGrade(char grade) {
        std::cout << "检查成绩: " << grade << " -> ";
        const char normalized_grade =
            static_cast<char>(std::toupper(static_cast<unsigned char>(grade)));

        // const char normalized_grade = toupper(grade);
        switch (normalized_grade) {
        case 'A':
            std::cout << "优秀" << "\n";
            break;
        case 'B':
            std::cout << "良好" << "\n";
            break;
        case 'C':
            std::cout << "勉强及格 (C)";
            // 我是故意的，我想让它继续往下走，去处理通用的“及格”逻辑
            [[fallthrough]];
        case 'D':
            // 这里会被 C 穿透进来
            std::cout << " 和 (D) -> 最终结论：及格" << std::endl;
            break;
        default:
            std::cout << "不及格" << std::endl;
            break;
        }
    }

    void test() {
        checkGrade('A');
        checkGrade('b');
        checkGrade('C');
        checkGrade('d');
        checkGrade('D');
        checkGrade('F');
    }
} // namespace ns4
int main() {
    ns2::test();
    ns2::test2();
    ns2::test3();

    ns3::test();

    ns4::test();
}