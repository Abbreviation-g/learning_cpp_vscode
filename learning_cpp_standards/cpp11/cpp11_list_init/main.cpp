#include <initializer_list>
#include <iostream>
#include <vector>
// https://subingwen.cn/cpp/list-init/
// 列表初始化
// 关于C++中的变量，数组，对象等都有不同的初始化方法，在这些繁琐的初始化方法中没有任何一种方式适用于所有的情况。为了统一初始化方式，并且让初始化行为具有确定的效果，在C++11中提出了列表初始化的概念。

// 1. 统一的初始化
namespace init_ns {
    // 在C++98/03中，对应普通数组和可以直接进行内存拷贝（memcpy()）的对象是可以使用列表初始化来初始化数据的
    void test() {
        // 数组的初始化
        int array[] = { 1, 3, 5, 7, 9 };
        double array1[3] = { 1.2, 1.3, 1.4 };

        // 对象的初始化
        struct Person {
            int id;
            double salary;
        } zhang3{ 1, 3000 };
    }
    class Test {
    public:
        Test(int) {
        }

    private:
        Test(const Test &);
    };

    // 在C++11中，列表初始化变得更加灵活了，来看一下下面这段初始化类对象的代码：
    void test2() {
        Test t1(520); // t1：最中规中矩的初始化方式，通过提供的带参构造进行对象的初始化
        // Test t2 = 520; //
        // t2：语法错误，因为提供的拷贝构造函数是私有的。如果拷贝构造函数是公共的，520会通过隐式类型转换被Test(int)构造成一个匿名对象，然后再通过对这个匿名对象进行拷贝构造得到t2（这个错误在VS中不会出现，在Linux中使用g++编译会提示描述的这个错误，截图如下。）

        // t3和t4：使用了C++11的初始化方式来初始化对象，效果和t1的方式是相同的。
        Test t3 = { 520 };
        Test t4{ 520 };

        int a1 = { 1314 };
        int a2{ 1314 };
        int arr1[] = { 1, 2, 3 };
        int arr2[]{ 1, 2, 3 };
        // t1、arr1和t2、arr2：这两个是基础数据类型的列表初始化方式，可以看到，和对象的初始化方式是统一的。
        // t4、a2、arr2的写法，是C++11中新添加的语法格式，使用这种方式可以直接在变量名后边跟上初始化列表，来进行变量或者对象的初始化。

        int *p = new int{ 520 };
        double b = double{ 52.134 };
        int *array = new int[3]{ 1, 2, 3 };
    }

    class Person {
    public:
        Person(int num) : Person(num, "") {
        }
        Person(int num, std::string name) : m_num(num), m_name(name) {
        }

    private:
        int m_num;
        std::string m_name;
    };
    Person getPerson() {
        return { 1, "name" };
    }
    struct T {
    public:
        int a;
        int arr[3];
        int c;
    };
    void test3() {
        T t{ 3, { 3, 5, 7 }, 4 };
        std::cout << t.a << t.arr[0] << t.c << std::endl;

        Person t1(520);
        Person t2 = 520;
        Person t3 = { 520 };
        Person t4{ 520 };
        Person t5{ 1, "xxx" };
        Person t6 = getPerson();

        int a1 = { 1 };
        int a2{ 22 };
        int arr1[] = { 1, 4 };
        int arr2[4]{ 233 };

        int *p = new int{ 111 };
        int *p2 = new int(111);
        double b = double(222.0);
        double b2 = double{ 222.0 };
        int *arr = new int[2]{ 111, 222 };
        int *arr_2 = new int[]{ 111, 222 };
        int *arr_3 = new int[4]{};
        int *arr_4 = new int[4];
    }
} // namespace init_ns

// 因为如果使用列表初始化对对象初始化时，还需要判断这个对象对应的类型是不是一个聚合体，如果是初始化列表中的数据就会拷贝到对象中。
// 普通数组本身可以看做是一个聚合类型
// 满足以下条件的类（class、struct、union）可以被看做是一个聚合类型：
//  无用户自定义的构造函数。
//  无私有或保护的非静态数据成员。
//  无基类。
//  无虚函数。
//  类中不能有使用{}和=直接初始化的非静态数据成员（从c++14开始就支持了）。
namespace init_ns2 {
    using namespace std;
    struct T1 {
        int x;
        int y;
    } a = { 123, 321 };

    struct T2 {
        int x;
        int y;
        T2(int, int) : x(10), y(20) {
        }
    } b = { 123, 321 };

    void test() {
        int x[] = { 1, 2, 3, 4, 5, 6 };
        double y[3][3] = {
            { 1.23, 2.34, 3.45 },
            { 4.56, 5.67, 6.78 },
            { 7.89, 8.91, 9.99 },
        };
        char carry[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
        std::string sarry[] = { "hello", "world", "nihao", "shijie" };
    }
    // struct T1_1 {
    //     int x;
    //     long y;

    // protected:
    //     int z;
    // } t{ 1, 100, 2 }; // error, 类中有私有成员, 无法使用初始化列表初始化
    struct T2_2 {
        int x;
        long y;

    protected:
        static int z;
    } t{ 1, 100 };
    int T2_2::z = 0;

    void test2() {
        T2_2 t;
        std::cout << t.x << ", " << t.y << std::endl;
    }
} // namespace init_ns2

// 2.2 非聚合体
// 对于聚合类型的类可以直接使用列表初始化进行对象的初始化，
// 如果不满足聚合条件还想使用列表初始化其实也是可以的，
// 需要在类的内部自定义一个构造函数, 在构造函数中使用初始化列表对类成员变量进行初始化:
namespace init_ns3 {
    class Person {
    public:
        Person(int num, std::string name) : m_num(num), m_name(name) {
        }

    private:
        int m_num;
        std::string m_name;
    };
    Person getPerson() {
        return { 1, "name" };
    }

    // 聚合类型的定义并非递归的，也就是说当一个类的非静态成员是非聚合类型时，
    // 这个类也可能是聚合类型，比如下面的这个例子：
    struct T1 {
        int x;
        double y;

    private:
        int z;
    };

    struct T2 {
        T1 t1;
        long x1;
        double y1;
    };
    // 可以看到，T1并非一个聚合类型，因为它有一个Private的非静态成员。但是尽管T2有一个非聚合类型的非静态成员t1，T2依然是一个聚合类型，可以直接使用列表初始化的方式进行初始化。
    // 最后强调一下t2对象的初始化过程，对于非聚合类型的成员t1做初始化的时候，可以直接写一对空的大括号{}，这相当于调用是T1的无参构造函数。
    void test() {
        T2 t2{ {}, 520, 13.14 };
    }
} // namespace init_ns3

// 3. std::initializer_list
//  它是一个轻量级的容器类型，内部定义了迭代器iterator等容器必须的概念，遍历时得到的迭代器是只读的。
//  对于std::initializer_list<T>而言，它可以接收任意长度的初始化列表，但是要求元素必须是同种类型T
//  在std::initializer_list内部有三个成员接口：size(), begin(), end()。
//  std::initializer_list对象只能被整体初始化或者赋值。
namespace initializer_list_ns {
    // 3.1 作为普通函数参数
    // 如果想要自定义一个函数并且接收任意个数的参数（变参函数），只需要将函数参数指定为std::initializer_list，使用初始化列表{ }作为实参进行数据传递即可。
    void traversal(std::initializer_list<int> a) {
        for (auto it = a.begin(); it != a.end(); it++) {
            std::cout << *it << ", ";
        }
        std::cout << "\n";
    }
    void test1() {
        std::initializer_list<int> list{ 1, 2, 3, 4 };
        traversal(list);
        traversal(std::initializer_list<int>{ 23, 45, 56, 78, 9 });
        traversal({ 23, 45, 56, 78, 9 });
    }

    // 3.2 作为构造函数参数
    // 自定义的类如果在构造对象的时候想要接收任意个数的实参，可以给构造函数指定为std::initializer_list类型，在自定义类的内部还是使用容器来存储接收的多个实参。
    class Test {
    public:
        Test(std::initializer_list<std::string> list) {
            for (const auto *it = list.begin(); it != list.end(); it++) {
                std::cout << *it << ", ";
                m_names.push_back(*it);
            }
            std::cout << "\n";
        }
        void print_vector() {
            for (const auto &value : m_names) {
                std::cout << value << ", ";
            }
            std::cout << "\n";
        }

    private:
        std::vector<std::string> m_names;
    };
    void test2() {
        Test t({ "jack", "lucy", "tom" });
        t.print_vector();
        Test t1({ "hello", "world", "nihao", "shijie" });
        t1.print_vector();
    }
} // namespace initializer_list_ns
int main() {
    initializer_list_ns::test1();
    initializer_list_ns::test2();
}