#pragma once

class Math
{
public:

    template<class T, class T2>
    static T lerp(const T& a, const T& b, const T2& t)
    {
        T res = a + t * (b - a);
        return res;
    }
};
