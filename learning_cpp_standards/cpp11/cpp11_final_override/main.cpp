#include <iostream>
// https://subingwen.cn/cpp/final/
// final和override
namespace final_ns {
    // 1.1 修饰函数
    // 如果使用final修饰函数，只能修饰虚函数，这样就能阻止子类重写父类的这个函数了：
    class Base {
    public:
        virtual void test() = 0;
    };
    class Child : public Base {
    public:
        void test() final {
            std::cout << "class Child test();" << std::endl;
        }
    };
    class GrandChild : public Child {
    public:
        // Declaration of 'test' overrides a 'final' functionclang(final_function_overridden)
        // void test() final {
        //     std::cout << "class GrandChild test();" << std::endl;
        // }
    };
    void test() {
        Base *b = new Child{};
        b->test();

        // Base b2 = Child{}; // Variable type 'Base' is an abstract classclang(abstract_type_in_decl)
        // main.cpp(8, 22): Unimplemented pure virtual method 'test' in 'Base'
    }

    // 1.2 修饰类
    // 使用final关键字修饰过的类是不允许被继承的，也就是说这个类不能有派生类。
    class Child2 final : public Base {
        void test() final {
            std::cout << "class Child2 test();" << std::endl;
        }
    };
    // class GrandChild2 : public Child2 {    }; // Base 'Child2' is marked 'final'clang(class_marked_final_used_as_base)

} // namespace final_ns

// 2. override
namespace override_ns {
    // override关键字确保在派生类中声明的重写函数与基类的虚函数有相同的签名，
    // 同时也明确表明将会重写基类的虚函数，这样就可以保证重写的虚函数的正确性，
    // 也提高了代码的可读性，和final一样这个关键字要写到方法的后面。使用方法如下：
    class Base {
    public:
        virtual void test() = 0;
    };
    class Child : public Base {
    public:
        void test() override {
            std::cout << "class Child test();" << std::endl;
        }
    };
    void test() {
        std::cout << "override_ns-------" << std::endl;

        Base *b = new Child{};
        b->test(); // class Child test();
        Child c;
        c.test(); // class Child test();
    }
} // namespace override_ns
namespace non_virtual_ns {
    // override关键字确保在派生类中声明的重写函数与基类的虚函数有相同的签名，
    // 同时也明确表明将会重写基类的虚函数，这样就可以保证重写的虚函数的正确性，
    // 也提高了代码的可读性，和final一样这个关键字要写到方法的后面。使用方法如下：
    class Base {
    public:
        void test() {
            std::cout << "class Base test();" << std::endl;
        };
    };
    class Child : public Base {
    public:
        void test() {
            std::cout << "class Child test();" << std::endl;
        }
    };
    void test() {
        // test() ‌不是虚函数‌（没有 virtual 关键字），所以是‌静态绑定（编译期决定）‌。
        std::cout << "non_virtual_ns-------" << std::endl;
        Base *b = new Child{}; // class Base test();
        b->test();
        Child c;
        c.test(); // class Child test();
    }
} // namespace non_virtual_ns
namespace virtual_ns {
    // override关键字确保在派生类中声明的重写函数与基类的虚函数有相同的签名，
    // 同时也明确表明将会重写基类的虚函数，这样就可以保证重写的虚函数的正确性，
    // 也提高了代码的可读性，和final一样这个关键字要写到方法的后面。使用方法如下：
    class Base {
    public:
        virtual void test() {
            std::cout << "class Base test();" << std::endl;
        };
    };
    class Child : public Base {
    public:
        void test() {
            std::cout << "class Child test();" << std::endl;
        }
    };
    void test() {
        // 加了 virtual 多态
        std::cout << "virtual_ns-------" << std::endl;
        Base *b = new Child{}; // class Child test();
        b->test();
        Child c;
        c.test(); // class Child test();
    }
} // namespace non_virtual_ns
//Base::test() 是虚函数，但‌只有声明，没有定义‌。
// 虚函数类型	                是否需要实现
// virtual void test();	        ✅ ‌需要‌提供定义
// virtual void test() = 0;	    ❌ 不需要定义（纯虚）
// virtual void test() {}	    ✅ 已提供定义
int main() {
    final_ns::test();
    override_ns::test();
    non_virtual_ns::test();
    virtual_ns::test();
}