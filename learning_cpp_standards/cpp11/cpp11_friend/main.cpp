// https://subingwen.cn/cpp/friend/
// 扩展的 friend 语法

#include <iostream>
// 1. 语法改进
// 在 C++11 标准中对 friend关键字进行了一些改进，以保证其更加好用：

// 声明一个类为另外一个类的友元时，不再需要使用class关键字，并且还可以使用类的别名（使用 typedef 或者 using 定义）。
namespace friend_ns1 {
    using namespace std;
    // 类声明
    class Tom;
    // 定义别名
    using Honey = Tom;
    // 定义两个测试类
    class Jack {
    private:
        // friend class Tom; // c++98标准语法
        friend Tom; // c++11标准语法
        string name = "jack";
        void print() {
            cout << "my name is " << name << "\n";
        }
    };
    class Lucy {
        // friend class Tom; // c++98标准语法
        friend Honey; // c++11标准语法
        string name = "lucy";
        void print() {
            cout << "my name is " << name << "\n";
        }
    };

    class Tom {
    public:
        void print() {
            // 通过类成员对象访问其私有成员
            cout << "invoke Jack private member: " << jObj.name << endl;
            cout << "invoke Jack private function: " << endl;
            jObj.print();

            cout << "invoke Lucy private member: " << lObj.name << endl;
            cout << "invoke Lucy private function: " << endl;
            lObj.print();

            // invoke Jack private member: jack
            // invoke Jack private function:
            // my name is jack
            // invoke Lucy private member: lucy
            // invoke Lucy private function:
            // my name is lucy
        }

    private:
        string name = "Tom";
        Jack jObj;
        Lucy lObj;
    };
    void test() {
        Tom t{};
        t.print();
    }
} // namespace friend_ns1

// 2. 为类模板声明友元
namespace friend_ns2 {
    using namespace std;
    class Tom {};
    template <typename T>
    class Person {
        friend T;
    };
    void test1() {
        Person<Tom> p; // Tom类是Person类的友元
        Person<int> p2; // 对于int类型的模板参数，友元声明被忽略（第6行）
    }

    // 假设有一个矩形类，一个圆形类，我们在对其进行了一系列的操作之后，需要验证一下矩形的宽度和高度、圆形的半径是否满足要求，并且要求这个校验操作要在另一个类中完成。
    template <typename T>
    class Rectangle {
    public:
        friend T;
        Rectangle(int w, int h) : width(w), height(h) {
        }

    private:
        int width;
        int height;
    };
    template <typename T>
    class Circle {
    public:
        friend T;
        Circle(int r) : radius(r) {
        }

    private:
        int radius;
    };

    // 校验类
    class Verify {
    public:
        void verify_rectangle(int w, int h, Rectangle<Verify> &r) {
            if (r.width >= w && r.height >= h) {
                cout << "矩形的宽度和高度满足条件!" << endl;
            } else {
                cout << "矩形的宽度和高度不满足条件!" << endl;
            }
        }
        void verify_circle(int r, Circle<Verify> &c) {
            if (c.radius >= r) {
                cout << "圆形的半径满足条件!" << endl;
            } else {
                cout << "圆形的半径不满足条件!" << endl;
            }
        }
    };
    void test2() {
        Verify v{};
        Circle<Verify> circle(30);
        Rectangle<Verify> rect(90, 100);
        v.verify_circle(60, circle);
        v.verify_rectangle(100, 100, rect);
    }
} // namespace friend_ns2
int main() {
    friend_ns1::test();
    friend_ns2::test1();
}