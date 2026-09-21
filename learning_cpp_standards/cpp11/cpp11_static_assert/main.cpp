// 静态断言 static_assert
// https://subingwen.cn/cpp/static_assert/

#include <cassert>
#include <cstring>
#include <iostream>

// assert是一个运行时断言，也就是说它只有在程序运行时才能起作用
namespace assert_ns {
    void create_char_array(int len, char **&p) {
        assert(len > 0);
        char *array = new char[len];
        *p = array;
    }
    void test() {
        char **buff;
        create_char_array(20, buff);
        strcpy(*buff, "hello world");
        std::cout << "buff:" << *buff << std::endl;
        delete[] *buff;
    }
} // namespace assert_ns
//     静态断言static_assert，所谓静态就是在编译时就能够进行检查的断言，使用时不需要引用头文件。静态断言的另一个好处是，可以自定义违反断言时的错误提示信息。静态断言使用起来非常简单，它接收两个参数：
// 参数1：断言表达式，这个表达式通常需要返回一个 bool值
// 参数2：警告信息，它通常就是一段字符串，在违反断言（表达式为false）时提示该信息
namespace static_assert_ns {
    void test() {
        using namespace std;
        cout << "64bit Linux 指针大小: " << sizeof(char *) << endl; // 8
        cout << "64bit Linux long 大小: " << sizeof(long) << endl; // 4

        static_assert(sizeof(long) == 4, "long != 4 bytes");        
    }
} // namespace static_assert_ns
int main() {
    assert_ns::test();
    static_assert_ns::test();
}