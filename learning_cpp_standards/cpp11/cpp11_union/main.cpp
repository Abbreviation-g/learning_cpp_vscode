// https://subingwen.cn/cpp/union/
// 非受限联合体
// 1. 什么是非受限联合体
// 联合体又叫共用体，我将其称之为union，它的使用方式和结构体类似，程序猿可以在联合体内部定义多种不同类型的数据成员，但是这些数据会共享同一块内存空间（也就是如果对多个数据成员同时赋值会发生数据的覆盖）。在某些特定的场景下，通过这种特殊的数据结构我们就可以实现内存的复用，从而达到节省内存空间的目的。

// 在C++11之前我们使用的联合体是有局限性的，主要有以下三点：

// 不允许联合体拥有非POD类型的成员
// 不允许联合体拥有静态成员
// 不允许联合体拥有引用类型的成员
// 在新的C++11标准中，取消了关于联合体对于数据成员类型的限定，规定任何非引用类型都可以成为联合体的数据成员，这样的联合体称之为非受限联合体（Unrestricted Union）
#include <iostream>
namespace union_ns1 {
    union U1 {
        int a;
        double b;
        float c;
        char d;
        void print() {
            std::cout << "a: " << a << ", b: " << b << ", c: " << c << ", d: " << d << "\n";
        }
    };

    void test1() {
        U1 u1{ .a = 10086 };
        u1.print();
        u1.d = 'd';
        u1.print();
    }
} // namespace union_ns1

// 2.1 静态类型的成员
// 对于非受限联合体来说，静态成员有两种分别是静态成员变量和静态成员函数，我们来看一下下面的代码：
namespace union_ns2 {
    union Test {
        int age;
        long id;
        // int& tmp = age; // error非受限联合体中不允许出现引用类型
        static char c;
        static int print() {
            std::cout << "c value: " << c << "\n";
            return 0;
        }
    };
    char Test::c = 'a';

    void test() {
        using namespace std;
        Test t;
        Test t1;
        t.c = 'b';
        t1.c = 'c';
        t1.age = 666;

        cout << "t.c: " << t.c << endl;
        cout << "t1.c: " << t1.c << endl;
        cout << "t1.age: " << t1.age << endl;
        cout << "t1.id: " << t1.id << endl;
        t.print();
        Test::print();
        // t.c: c
        // t1.c: c
        // t1.age: 666
        // t1.id: 666
        // c value: c
        // c value: c
        // 在非受限联合体中静态成员变量和非静态成员变量使用的不是同一块内存。
    }
} // namespace union_ns2
// 2.2 非POD类型成员
// 在 C++11标准中会默认删除一些非受限联合体的默认函数。
// 比如，非受限联合体有一个非 POD 的成员，而该非 POD成员类型拥有 非平凡的构造函数，
// 那么非受限联合体的默认构造函数将被编译器删除。
// 其他的特殊成员函数，例如默认拷贝构造函数、拷贝赋值操作符以及析构函数等，也将遵从此规则。下面来举例说明：
namespace union_ns3 {
    using namespace std;
    union Student {
        int id;
        string name;
    };
    void test() {
        // Student s{.id=1}; // error C2280: “Student::Student(void)”: 尝试引用已删除的函数
        // 上面代码中的非受限联合体Student中拥有一个非PDO类型的成员string name，string
        // 类中有非平凡构造函数，因此Student的构造函数被删除（通过警告信息可以得知它的析构函数也被删除了）导致对象无法被成功创建出来。解决这个问题的办法就是由程序猿自己为非受限联合体定义构造函数，在定义构造函数的时候我们需要用到定位放置
        // new操作。
    }

    // placement new
    // 一般情况下，使用new申请空间时，是从系统的堆（heap）中分配空间，
    // 申请所得的空间的位置是根据当时的内存的实际使用情况决定的。
    // 但是，在某些特殊情况下，可能需要在已分配的特定内存创建对象，这种操作就叫做placement
    // new即定位放置 new。 定位放置new操作的语法形式不同于普通的new操作：
    // 使用new申请内存空间：Base* ptr = new Base; 使用定位放置new申请内存空间：
    // ClassName* ptr = new (定位的内存地址)ClassName;

    // 1. 使用定位放置new操作，既可以在栈(stack)上生成对象，也可以在堆（heap）上生成对象，这取决于定位时指定的内存地址是在堆还是在栈上。
    // 2. 从表面上看，定位放置new操作是申请空间，其本质是利用已经申请好的空间，真正的申请空间的工作是在此之前完成的。
    // 3. 使用定位放置new 创建对象时会自动调用对应类的构造函数，但是由于对象的空间不会自动释放，如果需要释放堆内存必须显示调用类的析构函数。
    // 4. 使用定位放置new操作，我们可以反复动态申请到同一块堆内存，这样可以避免内存的重复创建销毁，从而提高程序的执行效率（比如网络通信中数据的接收和发送）。

    class Base {
    public:
        Base() {
            cout << "Base()" << "\n";
        }
        ~Base() {
            cout << "~Base()" << "\n";
        }
        void print() {
            cout << "number value: " << number << endl;
        }

    private:
        int number;
    };
    void test2() {
        int n = 100;
        Base *b = new (&n) Base;
        b->print();
        // Base()
        // number value: 100
    }
    // 自定义非受限联合体构造函数
    // 掌握了placement new的使用，我们通过一段程序来演示一下如果在非受限联合体中自定义构造函数：
    class Base2 {
    public:
        void setText(string str) {
            notes = str;
        }
        void print() {
            cout << "Base notes: " << notes << endl;
        }

    private:
        string notes;
    };
    union Student2 {
        Student2() {
            new (&name) string;
        }
        ~Student2() {}

        int id;
        Base2 tmp;
        string name;
    };
    void test3() {
        Student2 s;
        s.name = "蒙奇·D·路飞";
        s.tmp.setText("我是要成为海贼王的男人!");
        s.tmp.print();
        cout << "Student name: " << s.name << endl;
        // Base notes: 我是要成为海贼王的男人!
        // Student name: 我是要成为海贼王的男人!
    }
} // namespace union_ns3
// 匿名的非受限联合体
// 一般情况下我们使用的非受限联合体都是具名的（有名字），但是我们也可以定义匿名的非受限联合体，一个比较实用的场景就是配合着类的定义使用。
namespace union_ns4 {
    using namespace std;
    struct Foreigner {
        Foreigner(string s, string p) : addr(s), phone(p) {}
        string addr;
        string phone;
    };
    class Person {
    public:
        enum class Category : char { Student, Local, Foreigner };
        Person(int num) : number(num), type(Category::Student) {}
        Person(string id) : idNum(id), type(Category::Local) {}
        Person(string addr, string phone) : foreigner(addr, phone), type(Category::Foreigner) {}
        ~Person() {}
        void print() {
            cout << "Person category: " << (int)type << endl;
            switch (type) {
            case Category::Student:
                cout << "Student school number: " << number << endl;
                break;
            case Category::Local:
                cout << "Local people ID number: " << idNum << endl;
                break;
            case Category::Foreigner:
                cout << "Foreigner address: " << foreigner.addr << ", phone: " << foreigner.phone << endl;
                break;
            default:
                break;
            }
        }

    private:
        Category type;
        union {
            int number;
            string idNum;
            Foreigner foreigner;
        };
    };
    void test1() {
        Person p1(9527);
        Person p2("1101122022X");
        Person p3("砂隐村村北", "1301810001");
        p1.print();
        p2.print();
        p3.print();
    }
    // 在Person类中添加了一个匿名的非受限联合体用来存储人口信息，仔细分析之后就会发现这种处理方式的优势非常明显：尽可能地节省了内存空间。
    // Person类可以直接访问匿名非受限联合体内部的数据成员。
    // 不使用匿名非受限联合体申请的内存空间等于 number、 idNum 、 foreign 三者内存之和。
    // 使用匿名非受限联合体之后number、 idNum 、 foreign 三者共用同一块内存。

} // namespace union_ns4
int main() {
    std::cout << "--------" << "union_ns1::test1();" << "--------" << "\n";
    union_ns1::test1();
    std::cout << "--------" << "union_ns2::test();" << "--------" << "\n";
    union_ns2::test();
    std::cout << "--------" << "union_ns3::test2();" << "--------" << "\n";
    union_ns3::test2();
    std::cout << "--------" << "union_ns3::test3();" << "--------" << "\n";
    union_ns3::test3();
    std::cout << "--------" << "union_ns3::test3();" << "--------" << "\n";
    union_ns4::test1();
}