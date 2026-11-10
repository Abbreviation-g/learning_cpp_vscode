#include <iostream>

//https://subingwen.cn/cpp/namespace/
// 嵌套命名空间

// ==========================================
// 旧风格 (C++17 之前)
// ==========================================
namespace OldCompany 
{
    namespace Department 
    {
        namespace DevTeam 
        {

            void greet() 
            {
                std::cout << "Hello from OldCompany::Department::DevTeam (老式写法)" << std::endl;
            }

        }
    }
}

// ==========================================
// 新风格 (C++17 简化写法)
// ==========================================
namespace NewCompany::Department::DevTeam 
{

    // 注意：这里不需要一层层括号缩进！
    void greet() 
    {
        std::cout << "Hello from NewCompany::Department::DevTeam (新式写法)" << std::endl;
    }
}

// 演示给已有命名空间添加成员
namespace NewCompany::Department 
{

    void meeting() 
    {
        std::cout << "Meeting in NewCompany::Department (扩展写法)" << std::endl;
    }

}

int main() 
{
    // 1. 调用旧风格的函数
    OldCompany::Department::DevTeam::greet();

    // 2. 调用新风格的函数
    NewCompany::Department::DevTeam::greet();

    // 3. 调用扩展空间的函数
    NewCompany::Department::meeting();

    // 4. 更方便的使用方式：using 指令
    // 你会发现新写法在 using 时也更清晰
    using namespace NewCompany::Department::DevTeam;
    greet(); // 现在可以直接调用了

    return 0;
}
