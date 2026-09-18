#include <iostream>
#include <memory>
#include <string>
// https://subingwen.cn/cpp/weak_ptr/
// 弱引用智能指针

// 1. 基本使用方法
// 弱引用智能指针std::weak_ptr可以看做是shared_ptr的助手，它不管理shared_ptr内部的指针。
// std::weak_ptr没有重载操作符*和->，因为它不共享指针，不能操作资源，
// 所以它的构造不会增加引用计数，析构也不会减少引用计数，
// 它的主要作用就是作为一个旁观者监视shared_ptr中管理的资源是否存在。
namespace weak_ptr_ns {
    // // 默认构造函数
    // constexpr weak_ptr() noexcept;
    // // 拷贝构造
    // weak_ptr (const weak_ptr& x) noexcept;
    // template <class U> weak_ptr (const weak_ptr<U>& x) noexcept;
    // // 通过shared_ptr对象构造
    // template <class U> weak_ptr (const shared_ptr<U>& x) noexcept;
    struct Test {
    public:
        std::shared_ptr<Test> get_shared_ptr() {
            return std::shared_ptr<Test>(this);
        }
    };
    void test1() {
        std::shared_ptr<Test> ptr1(new Test{});
        std::cout << "ptr1: use_count: " << ptr1.use_count() << "\n"; // 1
        // 存在未定义行为：
        std::shared_ptr<Test> ptr2 = ptr1->get_shared_ptr();
        std::cout << "ptr1: use_count: " << ptr1.use_count() << "\n"; //  1
        std::cout << "ptr2: use_count: " << ptr2.use_count() << "\n"; // 1
        // 结果是两个独立的 shared_ptr 管理同一个裸指针：
        // 函数结束时：
        // ptr2 释放 Test
        // ptr1 再次释放同一个 Test
        // 产生 double free，程序异常终止，所以还没执行到 test3()。
    }
    void test2() {
        std::shared_ptr<int> sp(new int{ 11 });
        std::weak_ptr<int> wp1;
        std::weak_ptr<int> wp2{ wp1 };
        std::weak_ptr<int> wp3{ sp };
        std::weak_ptr<int> wp4;
        wp4 = sp;
        std::weak_ptr<int> wp5;
        wp5 = wp3;
    }
    // 1.2.1 use_count()
    // 通过调用std::weak_ptr类提供的use_count()方法可以获得当前所观测资源的引用计数，函数原型如下
    // 虽然弱引用智能指针wp3、wp4、wp5监测的资源是同一个，但是它的引用计数并没有发生任何的变化，也进一步证明了weak_ptr只是监测资源，并不管理资源。
    void test3() {
        using namespace std;
        shared_ptr<int> sp(new int);

        weak_ptr<int> wp1;
        weak_ptr<int> wp2(wp1);
        weak_ptr<int> wp3(sp);
        weak_ptr<int> wp4;
        wp4 = sp;
        weak_ptr<int> wp5;
        wp5 = wp3;

        cout << "use_count: " << endl;
        cout << "wp1: " << wp1.use_count() << endl; // 0
        cout << "wp2: " << wp2.use_count() << endl; // 0
        cout << "wp3: " << wp3.use_count() << endl; // 1
        cout << "wp4: " << wp4.use_count() << endl; // 1
        cout << "wp5: " << wp5.use_count() << endl; // 1
    }
    // 1.2.2 expired()
    // 通过调用std::weak_ptr类提供的expired()方法来判断观测的资源是否已经被释放，函数原型如下：
    // weak_ptr监测的就是shared_ptr管理的资源，当共享智能指针调用shared.reset();之后管理的资源被释放，因此weak.expired()函数的结果返回true，表示监测的资源已经不存在了。
    void test4() {
        using namespace std;
        shared_ptr<int> shared(new int(10));
        weak_ptr<int> weak(shared);
        cout << "1. weak " << (weak.expired() ? "is" : "is not") << " expired" << endl; // 1. weak is not expired

        shared.reset();
        cout << "2. weak " << (weak.expired() ? "is" : "is not") << " expired" << endl; // 2. weak is expired
    }
    // 1.2.3 lock()
    // 通过调用std::weak_ptr类提供的lock()方法来获取管理所监测资源的shared_ptr对象，函数原型如下：
    // shared_ptr<element_type> lock() const noexcept;
    void test5() {
        using namespace std;
        shared_ptr<int> sp1, sp2;
        weak_ptr<int> wp;

        sp1 = std::make_shared<int>(520);
        wp = sp1;
        sp2 = wp.lock();
        cout << "use_count: " << wp.use_count() << endl; // 2

        sp1.reset();
        cout << "use_count: " << wp.use_count() << endl; // 1

        sp1 = wp.lock();
        cout << "use_count: " << wp.use_count() << endl; // 2

        cout << "*sp1: " << *sp1 << endl;
        cout << "*sp2: " << *sp2 << endl;
        // sp2 = wp.lock();通过调用lock()方法得到一个用于管理weak_ptr对象所监测的资源的共享智能指针对象，使用这个对象初始化sp2，此时所监测资源的引用计数为2
        // sp1.reset();共享智能指针sp1被重置，weak_ptr对象所监测的资源的引用计数减1
        // sp1 = wp.lock();sp1重新被初始化，并且管理的还是weak_ptr对象所监测的资源，因此引用计数加1
        // 共享智能指针对象sp1和sp2管理的是同一块内存，因此最终打印的内存中的结果是相同的，都是520
    }
    // 1.2.4 reset()
    // 通过调用std::weak_ptr类提供的reset()方法来清空对象，使其不监测任何资源，函数原型如下：
    // void reset() noexcept;
    void test6() {
        using namespace std;
        shared_ptr<int> sp(new int(10));
        weak_ptr<int> wp(sp);
        cout << "1. wp " << (wp.expired() ? "is" : "is not") << " expired" << endl;

        wp.reset();
        cout << "2. wp " << (wp.expired() ? "is" : "is not") << " expired" << endl;
        // weak_ptr对象sp被重置之后wp.reset();变成了空对象，不再监测任何资源，因此wp.expired()返回true
    }
} // namespace weak_ptr_ns
// shared_ptr使用的注意事项
// 1. 不能使用一个原始地址初始化多个共享智能指针
// 2. 函数不能返回管理了this的共享智能指针对象
// 3. 共享智能指针不能循环引用
namespace weak_ptr_ns2 {
    using namespace std;
    struct Test {
    public:
        shared_ptr<Test> get_shared_ptr() {
            return shared_ptr<Test>(this);
        }
        ~Test() {
            std::cout << "class Test destructor" << "\n";
        }
    };
    void test1() {
        Test *t = new Test{};
        // shared_ptr<Test> ptr1(t);
        // shared_ptr<Test> ptr2(t);
        // class Test destructor
        // class Test destructor
        // 错误 一个对象被析构了两次

        shared_ptr<Test> ptr1(t);
        shared_ptr<Test> ptr2(ptr1);
        // class Test destructor
        // 正确 析构了一次
    }
    void test2() {
        shared_ptr<Test> sp1(new Test{});
        cout << "use_count: " << sp1.use_count() << endl; // 1
        shared_ptr<Test> sp2 = sp1->get_shared_ptr();
        cout << "use_count: " << sp1.use_count() << endl; // 1
        // class Test destructor
        // class Test destructor
        // 错误 一个对象被析构了两次
    }
} // namespace weak_ptr_ns2

// 通过weak_ptr来解决，通过wek_ptr返回管理this资源的共享智能指针对象shared_ptr。
// C++11中为我们提供了一个模板类叫做std::enable_shared_from_this<T>，
// 这个类中有一个方法叫做shared_from_this()，通过这个方法可以返回一个共享智能指针，
// 在函数的内部就是使用weak_ptr来监测this对象，并通过调用weak_ptr的lock()方法返回一个
// 在调用enable_shared_from_this类的shared_from_this()方法之前，
// 必须要先初始化函数内部weak_ptr对象，否则该函数无法返回一个有效的shared_ptr对象
// （具体处理方法可以参考上面的示例代码）。
namespace weak_ptr_ns3 {
    using namespace std;
    struct Test : public enable_shared_from_this<Test> {
    public:
        shared_ptr<Test> get_shared_ptr() {
            return shared_from_this();
        }
        ~Test() {
            std::cout << "class Test destructor" << "\n";
        }
    };
    void test1() {
        shared_ptr<Test> sp1(new Test);
        cout << "use_count: " << sp1.use_count() << endl; // 1
        shared_ptr<Test> sp2 = sp1->get_shared_ptr();
        cout << "use_count: " << sp1.use_count() << endl; // 2
        // class Test destructor
        // 正确 析构了一次
    }
} // namespace weak_ptr_ns3

// 3. 解决循环引用问题
// 智能指针如果循环引用会导致内存泄露
namespace weak_ptr_ns4 {
    using namespace std;

    struct TA;
    struct TB;
    struct TA {
        shared_ptr<TB> bptr;
        ~TA() {
            cout << "class TA is disstruct ..." << endl;
        }
    };

    struct TB {
        shared_ptr<TA> aptr;
        ~TB() {
            cout << "class TB is disstruct ..." << endl;
        }
    };
    // 在测试程序中，共享智能指针ap、bp对TA、TB实例对象的引用计数变为2，
    // 在共享智能指针离开作用域之后引用计数只能减为1，
    // 这种情况下不会去删除智能指针管理的内存，导致类TA、TB的实例对象不能被析构，最终造成内存泄露。
    void test1() {
        shared_ptr<TA> ap(new TA{});
        shared_ptr<TB> bp(new TB{});
        cout << "TA object use_count: " << ap.use_count() << endl; // 1
        cout << "TB object use_count: " << bp.use_count() << endl; // 1

        ap->bptr = bp;
        cout << "TA object use_count: " << ap.use_count() << endl; // 1
        cout << "TB object use_count: " << bp.use_count() << endl; // 2
        bp->aptr = ap;
        cout << "TA object use_count: " << ap.use_count() << endl; // 2
        cout << "TB object use_count: " << bp.use_count() << endl; // 2
    }
} // namespace weak_ptr_ns4

namespace weak_ptr_ns5 {
    // 通过使用weak_ptr可以解决这个问题，只要将类TA或者TB的任意一个成员改为weak_ptr
    using namespace std;

    struct TA;
    struct TB;

    struct TA {
        weak_ptr<TB> bptr;
        ~TA() {
            cout << "class TA is disstruct ..." << endl;
        }
    };

    struct TB {
        shared_ptr<TA> aptr;
        ~TB() {
            cout << "class TB is disstruct ..." << endl;
        }
    };
    // 上面程序中，在对类TA成员赋值时ap->bptr = bp;
    // 由于bptr是weak_ptr类型，这个赋值操作并不会增加引用计数，所以bp的引用计数仍然为1，
    // 在离开作用域之后bp的引用计数减为0，类TB的实例对象被析构。

    // 在类TB的实例对象被析构的时候，内部的aptr也被析构，
    // 其对TA对象的管理解除，内存的引用计数减为1，当共享智能指针ap离开作用域之后，
    // 对TA对象的管理也解除了，内存的引用计数减为0，类TA的实例对象被析构。
    void test1() {
        shared_ptr<TA> ap(new TA);
        shared_ptr<TB> bp(new TB);
        cout << "TA object use_count: " << ap.use_count() << endl; // 1
        cout << "TB object use_count: " << bp.use_count() << endl; // 1

        ap->bptr = bp;
        cout << "TA object use_count: " << ap.use_count() << endl; // 1
        cout << "TB object use_count: " << bp.use_count() << endl; // 1
        bp->aptr = ap;
        cout << "TA object use_count: " << ap.use_count() << endl; // 2
        cout << "TB object use_count: " << bp.use_count() << endl; // 1
    }
} // namespace weak_ptr_ns5
int main() {
    // weak_ptr_ns::test1();
    // weak_ptr_ns::test3();
    // weak_ptr_ns::test4();
    // weak_ptr_ns::test5();
    // weak_ptr_ns::test6();

    // weak_ptr_ns2::test1();
    // weak_ptr_ns2::test2();

    // weak_ptr_ns3::test1();
    // weak_ptr_ns4::test1();

    weak_ptr_ns5::test1();
}