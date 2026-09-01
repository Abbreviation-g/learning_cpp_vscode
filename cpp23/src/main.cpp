#include <cstddef>
#include <iostream>
#include <utility>

class Buffer {
public:
    
    // 默认构造函数：不接收任何参数，创建一个空的 Buffer。
    explicit Buffer() {
        std::cout << "default ctor" << std::endl;
    }

    
    // 显式构造函数：禁止从 int 隐式转换为 Buffer。
    explicit Buffer(int a) {
        std::cout << "int ctor: " << a << std::endl;
    }

    
    // 拷贝构造函数：用另一个 Buffer 创建新对象。
    // 注意：拷贝构造函数如果声明为 explicit，那么只能用直接初始化，不能用 `Buffer b = a;` 这种复制初始化。
    explicit Buffer(const Buffer& other) {
        std::cout << "copy ctor" << std::endl;
    }

    
    // 移动构造函数：从右值对象接管资源，避免额外拷贝。
    // 注意：同理，`Buffer c = std::move(a);` 也会被禁止，必须改成 `Buffer c(std::move(a));`。
    explicit Buffer(Buffer&& other) noexcept {
        std::cout << "move ctor" << std::endl;
    }

    
    // 拷贝赋值运算符：把一个已存在的 Buffer 复制给另一个。
    Buffer& operator=(const Buffer& other) {
        std::cout << "copy assignment" << std::endl;
        return *this;
    }

    
    // 移动赋值运算符：把右值对象资源转移到当前对象。
    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "move assignment" << std::endl;
        return *this;
    }

    
    // 析构函数：释放资源，通常在对象销毁时调用。
    ~Buffer() {
        std::cout << "dtor" << std::endl;
    }
};

static void process(const Buffer& b) {
    std::cout << "process by value" << std::endl;
}

int main() {
    std::cout << "--- demo ---" << std::endl;

    process(Buffer(11));

    Buffer a(20);
    process(a);                     // copy ctor is used here

    Buffer b(a);                   // direct init: copy ctor
    Buffer c(std::move(a));        // direct init: move ctor

    b = a;                         // copy assignment
    c = std::move(b);              // move assignment

    std::cout << "End Main" << std::endl;
    return 0;
}
