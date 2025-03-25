#include <iostream>
#include "point.h"
#include "number.h"

int add(int a, int b) { return a + b + 1000; }
namespace Number_Func
{
    int add(int a, int b) { return a + b; }
    // ::表示默认命名空间
    int add_int(int a, int b) { return ::add(a, b) + 1; }
}

using namespace std;
static void test01()
{
    // cout << ((1<=>0)>0) << endl; // c++20新特性
    cout << "hello vscode" << endl;
    cout << "hello vscode2" << endl;
    cout << "hello vscode3" << endl;
    cout << Number_Func::add(1, 2) << endl;
    cout << add(1, 2) << endl;
}
static void test02()
{
    Math_Func::Number n1;
    Math_Func::Number n2(1.3);
    n1.info_number();
    n2.info_number();
    Math_Func::Point p1;
    Math_Func::Point p2(1.3, 1.5);
    p1.info_point();
    p2.info_point();
    Math_Func::add(1, 2);
}
static void test03()
{
    cout << Number_Func::add_int(1, 2) << endl;
}
static void test04()
{
    {
        // 只引入函数
        using Number_Func::add_int;
        cout << add_int(1, 2) << endl;
    }
    {
        // 只引入类
        using Math_Func::Number;
        Number n1(1.3);
        n1.info_number();
        Math_Func::Point p1(1.2, 3.2);
        p1.info_point();
    }
    {
        // 引入命名空间，并解决冲突
        using namespace Math_Func;
        Number n1(1.3);
        n1.info_number();
        Point p1(1.2, 3.2);
        p1.info_point();

        // add(2,3); // 报错，Math_Func里面也有一个add，默认命名空间也有add
        cout << Math_Func::add(1, 2) << endl;
        cout << ::add(1, 2) << endl;
    }
}
namespace N1
{
    int add2(int a, int b) { return a + b; }
    namespace N2
    {
        int add2(int a, int b) { return N1::add2(a, b) + 1; }
    } // namespace N2
}
static void test05()
{
    // 命名空间的嵌套和别名
    {
        using namespace N1::N2;
        cout << add2(1, 2) << endl;
    }
    {
        // 别名
        namespace N2_S = N1::N2;
        using namespace N2_S;
        cout << add2(1, 2) << endl;
    }
}
int main()
{

    cout << "---------------------------------" << endl;
    // test03();
    // test04();
    test05();
    cout << "---------------------------------" << endl;
}
