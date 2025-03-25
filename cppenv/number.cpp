#include "number.h"

namespace Math_Func
{
    int add(int a, int b) { return a + b; }

    Number::Number(double x) : x(x) {}
    Number::~Number() {}
}