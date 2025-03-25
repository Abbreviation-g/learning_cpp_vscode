#ifndef NUMBER_H
#define NUMBER_H
#include <iostream>
namespace Math_Func
{
    int add(int a, int b);

    class Number
    {
    public:
        Number() = default;
        Number(double x);
        ~Number();
        void info_number() const { std::cout << "Number: " << x << std::endl; }

    private:
        double x{0.0};
    };

}

#endif