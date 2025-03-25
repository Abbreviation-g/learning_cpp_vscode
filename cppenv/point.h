#ifndef POINT_H
#define POINT_H
#include <iostream>
namespace Math_Func
{
    class Point
    {
    public:
        Point() = default;
        Point(double x, double y);
        void info_point() const { std::cout << "Point\t x: " << x << ", y: " << y << std::endl; }
        ~Point();
    private:
        double x{0.0};
        double y{0.0};
    };

}

#endif