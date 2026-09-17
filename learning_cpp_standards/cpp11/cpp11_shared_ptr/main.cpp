#include <cstddef>
#include <iostream>
#include <string>
#include <memory>
// https://subingwen.cn/cpp/shared_ptr/
// 共享智能指针
// 智能指针是存储指向动态分配（堆）对象指针的类，用于生存期的控制，能够确保在离开指针所在作用域时，
// 自动地销毁动态分配的对象，防止内存泄露。
// 智能指针的核心实现技术是引用计数，
// 每使用它一次，内部引用计数加1，每析构一次内部的引用计数减1，减为0时，删除所指向的堆内存。
// C++11中提供了三种智能指针，使用这些智能指针时需要引用头文件<memory>：

// std::shared_ptr：共享的智能指针
// std::unique_ptr：独占的智能指针
// std::weak_ptr：弱引用的智能指针，它不共享指针，不能操作资源，是用来监视shared_ptr的。

namespace shared_ptr_ns {
    using namespace std;
    //     1.1 通过构造函数初始化
    // // shared_ptr<T> 类模板中，提供了多种实用的构造函数, 语法格式如下:
    // std::shared_ptr<T> 智能指针名字(创建堆内存);
    auto test1() -> void {
        // 使用智能指针管理一块 int 型的堆内存
        std::shared_ptr<int> ptr1{ new int{ 520 } };
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 1
        // 使用智能指针管理一块字符数组对应的堆内存
        shared_ptr<char> ptr2(new char[12]{ 'c', 'a', 'r', 'r', 'y' });
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 1
        // 创建智能指针对象, 不管理任何内存
        shared_ptr<int> ptr3;
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 0
        // 创建智能指针对象, 初始化为空
        shared_ptr<int> ptr4(nullptr);
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 0

        int *p = new int;
        shared_ptr<int> p1(p);
        // shared_ptr<int> p2(p); // 它通常不会在构造时立刻报错，但代码本身已经产生了未定义行为。
        std::cout << "-------------\n";
    }
    // 1.2 通过拷贝和移动构造函数初始化
    // 当一个智能指针被初始化之后，就可以通过这个智能指针初始化其他新对象。
    // 在创建新对象的时候，对应的拷贝构造函数或者移动构造函数就被自动调用了。
    // 如果使用拷贝的方式初始化共享智能指针对象，这两个对象会同时管理同一块堆内存，堆内存对应的引用计数也会增加；
    // 如果使用移动的方式初始智能指针对象，只是转让了内存的所有权，管理内存的对象并不会增加，因此内存的引用计数不会变化。
    auto test2() -> void {
        // 使用智能指针管理一块 int 型的堆内存, 内部引用计数为 1
        shared_ptr<int> ptr1(new int(520));
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 1
        // 调用拷贝构造函数
        shared_ptr<int> ptr2(ptr1);
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 2
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 2
        shared_ptr<int> ptr3 = ptr1;
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 3
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 3
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 3
        // 调用移动构造函数
        shared_ptr<int> ptr4(std::move(ptr1));
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 0
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 3
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 3
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 3
        std::shared_ptr<int> ptr5 = std::move(ptr2);
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 0
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 0
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 3
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 3
        cout << "ptr5管理的内存引用计数: " << ptr5.use_count() << endl; // 3
        std::shared_ptr<int> ptr6(ptr5);
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 0
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 0
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 4
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 4
        cout << "ptr5管理的内存引用计数: " << ptr5.use_count() << endl; // 4
        cout << "ptr6管理的内存引用计数: " << ptr6.use_count() << endl; // 4
        std::cout << "-------------\n";
    }

    // 1.3 通过std::make_shared初始化
    // 通过C++提供的std::make_shared() 就可以完成内存对象的创建并将其初始化给智能指针，函数原型如下：

    // template< class T, class... Args >
    // shared_ptr<T> make_shared( Args&&... args );
    // T：模板参数的数据类型
    // Args&&... args ：要初始化的数据，如果是通过make_shared创建对象，需按照构造函数的参数列表指定
    class Test {
    public:
        Test() {
            cout << "construct Test..." << endl;
        }
        Test(int x) {
            cout << "construct Test, x = " << x << endl;
        }
        Test(string str) {
            cout << "construct Test, str = " << str << endl;
        }
        ~Test() {
            cout << "destruct Test ..." << endl;
        }
    };
    auto test3() -> void {
        // 使用智能指针管理一块 int 型的堆内存, 内部引用计数为 1
        shared_ptr<int> ptr1 = make_shared<int>(520);
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 1

        shared_ptr<Test> ptr2 = make_shared<Test>();
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl;
        // construct Test...
        // 1

        shared_ptr<Test> ptr3 = make_shared<Test>(Test{ 520 });
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl;
        // construct Test, x = 520
        //  1

        shared_ptr<Test> ptr4 = make_shared<Test>("我是要成为海贼王的男人!!!");
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl;
        // construct Test, str = 我是要成为海贼王的男人!!!
        // 1

        std::cout << "-------------\n";
        // destruct Test ...
        // destruct Test ...
        // destruct Test ...
    }

    //     1.4 通过 reset方法初始化
    // 对于一个未初始化的共享智能指针，可以通过reset方法来初始化，当智能指针中有值的时候，调用reset会使引用计数减1。
    // 共享智能指针类提供的std::shared_ptr::reset方法函数原型如下：

    // void reset() noexcept;

    // template< class Y >
    // void reset( Y* ptr );

    // template< class Y, class Deleter >
    // void reset( Y* ptr, Deleter d );

    // template< class Y, class Deleter, class Alloc >
    // void reset( Y* ptr, Deleter d, Alloc alloc );
    // ptr：指向要取得所有权的对象的指针
    // d：指向要取得所有权的对象的指针
    // aloc：内部存储所用的分配器
    void test4() {
        // 使用智能指针管理一块 int 型的堆内存, 内部引用计数为 1
        shared_ptr<int> ptr1 = make_shared<int>(int{ 520 });
        shared_ptr<int> ptr2 = ptr1;
        shared_ptr<int> ptr3 = ptr1;
        shared_ptr<int> ptr4 = ptr1;
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 4
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 4
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 4
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 4

        ptr4.reset();
        cout << "ptr1管理的内存引用计数: " << ptr1.use_count() << endl; // 3
        cout << "ptr2管理的内存引用计数: " << ptr2.use_count() << endl; // 3
        cout << "ptr3管理的内存引用计数: " << ptr3.use_count() << endl; // 3
        cout << "ptr4管理的内存引用计数: " << ptr4.use_count() << endl; // 0

        shared_ptr<int> ptr5{ ptr1 };
        ptr5.reset(new int(250), [](int *p) {
            std::cout << "delete int\n";
            delete p;
        });
        cout << "ptr5管理的内存引用计数: " << ptr5.use_count() << endl; // 1

        std::cout << "-------------\n";
    }
    // 1.5 获取原始指针
    // 通过智能指针可以管理一个普通变量或者对象的地址，此时原始地址就不可见了。当我们想要修改变量或者对象中的值的时候，就需要从智能指针对象中先取出数据的原始内存的地址再操作，解决方案是调用共享智能指针类提供的get()方法，其函数原型如下：

    // T* get() const noexcept;
    auto test5() -> void {
        int len = 128;
        shared_ptr<char> ptr(new char[len]{ 'c', 'a', 'r', 'r', 'y' });
        // 得到指针的原始地址
        char *add = ptr.get();
        cout << "string: " << add << endl; // string: carry
        memset(add, 0, len);
        strcpy(add, "我是要成为海贼王的男人!!!");
        cout << "string: " << add << endl; // string: 我是要成为海贼王的男人!!!

        shared_ptr<int> p(new int);
        *p = 100;
        cout << p.get() << "  " << *p << "  " << *(p.get()) << endl; // 0x209a064dfe0  100  100
        std::cout << "-------------\n";
    }
} // namespace shared_ptr_ns

// 2. 指定删除器
// 当智能指针管理的内存对应的引用计数变为0的时候，这块内存就会被智能指针析构掉了。
// 另外，我们在初始化智能指针的时候也可以自己指定删除动作，这个删除操作对应的函数被称之为删除器，
// 这个删除器函数本质是一个回调函数，我们只需要进行实现，其调用是由智能指针完成的。
namespace shared_ptr_ns2 {
    using namespace std;
    template <typename T>
    auto delete_ptr(T *ptr) -> void {
        delete ptr;
        cout << "auto delete_ptr(T *ptr) -> void" << "\n";
    }
    template <typename T>
    auto delete_ptr2(T *ptr) -> void {
        delete[] ptr;
        cout << "auto delete_ptr2(T ptr[]) -> void" << "\n";
    }
    void deleteIntPtr(int *p) {
        delete p;
        cout << "int 型内存被释放了..." << "\n";
    }
    void deleteIntArrPtr(int *p) {
        delete[] p;
        cout << "int[] 型内存被释放了..." << "\n";
    }
    auto test1() -> void {
        int *i = new int{ 10086 };
        int len = 4;
        int *arr = new int[len]{ 10, 11, 50, 0 };
        std::shared_ptr<int> ptr1(i, deleteIntPtr);
        std::shared_ptr<int> ptr2{ arr, deleteIntArrPtr };
        std::cout << "-------------\n";
        // int[] 型内存被释放了...
        // int 型内存被释放了...
    }
    auto test2() -> void {
        int *i = new int{ 10086 };
        int len = 4;
        int *arr = new int[len]{ 10, 11, 50, 0 };
        std::shared_ptr<int> ptr1(i, delete_ptr<int>); //
        std::shared_ptr<int> ptr2{ arr, delete_ptr2<int> };
        std::cout << "-------------\n";

        // auto delete_ptr2(T ptr[]) -> void
        // auto delete_ptr(T *ptr) -> void
    }
    auto test3() -> void {
        shared_ptr<int> ptr(new int(250), deleteIntPtr);

        shared_ptr<int> ptr2(new int(250), [](int *p) {
            delete p;
        });
        shared_ptr<int> ptr3(new int[10], [](int *p) {
            delete[] p;
        });
        // 在删除数组内存时，除了自己编写删除器，也可以使用C++提供的std::default_delete<T>()函数作为删除器，
        // 这个函数内部的删除功能也是通过调用delete来实现的，要释放什么类型的内存就将模板类型T指定为什么类型即可。
        shared_ptr<int> ptr4(new int[10], default_delete<int[]>{});
    }
    // 另外，我们还可以自己封装一个make_shared_array方法来让shared_ptr支持数组，代码如下:
    template <typename T>
    auto make_shared_array(size_t size) -> shared_ptr<T[]> {
        return shared_ptr<T[]>(new T[size], default_delete<T[]>{});
    }
    auto test4() -> void {
        const int len = 12;
        shared_ptr<int[]> ptr1 = make_shared_array<int>(len);
        int *arr = ptr1.get();
        for (int i = 0; i < len; i++) {
            arr[i] = i;
            std::cout << arr[i] << ",";
        }
        std::cout << "\n";
        //  0,1,2,3,4,5,6,7,8,9,10,11,

        shared_ptr<int[]> ptr3(new int[10]);
    }
} // namespace shared_ptr_ns2
auto main(int argc, const char **argv) -> int {
    shared_ptr_ns::test1();
    shared_ptr_ns::test2();
    shared_ptr_ns::test3();
    shared_ptr_ns::test4();
    shared_ptr_ns::test5();
    shared_ptr_ns2::test1();
    shared_ptr_ns2::test2();
    shared_ptr_ns2::test3();
    shared_ptr_ns2::test4();
}