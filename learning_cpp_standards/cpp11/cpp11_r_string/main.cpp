#include <iostream>
#include <string>

// https://www.bilibili.com/video/BV1bX4y1G7ks
// https://subingwen.cn/cpp/R/
// 原始字面量
// 在C++11中添加了定义原始字符串的字面量，定义方式为：R
// “xxx(原始字符串)xxx”其中（）两边的字符串可以省略。原始字面量R可以直接表示字符串的实际含义，而不需要额外对字符串做转义或连接等操作。 最后强调一个细节：在R
// “xxx(raw string)xxx” 中，原始字符串必须用括号（）括起来，括号的前后可以加其他字符串，所加的字符串会被忽略，并且加的字符串必须在括号两边同时出现。

namespace test1_ns {
    void test() {
        using namespace std;
        string str = "D:\hello\world\test.text";
        cout << str << endl;
        string str1 = "D:\\hello\\world\\test.text";
        cout << str1 << endl;
        string str2 = R"(D:\hello\world\test.text)";
        cout << str2 << endl;
        string str3 = R"(a 
        b
        c
        d)";

        // 在C++11之前如果一个字符串分别写到了不同的行里边，需要加连接符，这种方式不仅繁琐，还破坏了表达式的原始含义，如果使用原始字面量就变得简单很多，很强直观，可读性强。
        string str4 = "<html>\
        <head>\
        <title>\
        海贼王\
        </title>\
        </head>\
        <body>\
        <p>\
        我是要成为海贼王的男人!!!\
        </p>\
        </body>\
        </html>";
        cout << str4 << endl;
        string str5 = R"(<html>
        <head>
        <title>
        海贼王
        </title>
        </head>
        <body>
        <p>
        我是要成为海贼王的男人!!!
        </p>
        </body>
        </html>)";
        cout << str5 << endl;
        // D:helloworld    est.text
        // D:\hello\world\test.text
        // D:\hello\world\test.text
        // a
        //         b
        //         c
        //         d
    }
    // 通过输出的信息可以得到如下结论：使用原始字面量R “xxx(raw string)xxx”，（）两边的字符串在解析的时候是会被忽略的，因此一般不用指定。
    // 如果在（）前后指定了字符串，那么前后的字符串必须相同，否则会出现语法错误。
    void test2() {
        using namespace std;

        string str1 = R"(D:\hello\world\test.text)";
        cout << str1 << endl;
        string str2 = R"luffy(D:\hello\world\test.text)luffy";
        cout << str2 << endl;
        // string str3 = R"luffy(D:\hello\world\test.text)robin";	// 语法错误，编译不通过
        // cout << str3 << endl;
    }
} // namespace test1_ns
int main() {
    test1_ns::test();
    test1_ns::test2();
}