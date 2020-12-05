#pragma once

#include "swizzleOp2.h"

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper;

template <typename T = float>
struct vec2
{
    union {
        struct { T x, y; };
        T data[2];

        union
        {
            swizzleOp2<swizzleOpVecHelper, vec2, T, 3, IndexPair<0, 0>> x;
            swizzleOp2<swizzleOpVecHelper, vec2, T, 3, IndexPair<1, 0>> y;
        } swizzle;
    };

    vec2() = default;
    vec2(const T& v) : x(v), y(v) {};
    vec2(const T& x, const T& y) : x(x), y(y) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };

    friend vec2 operator+(const vec2& a, const vec2& b)
    {
        vec2 res = { a.x + b.x, a.y + b.y };
        return res;
    }

    friend vec2& operator+=(vec2& a, const vec2& b)
    {
        a = a + b;
        return a;
    }

    friend vec2 operator-(const vec2& a, const vec2& b)
    {
        vec2 res = { a.x - b.x, a.y - b.y };
        return res;
    }

    friend vec2& operator-=(vec2& a, const vec2& b)
    {
        a = a - b;
        return a;
    }

    friend vec2 operator*(const vec2& a, const vec2& b)
    {
        vec2 res = { a.x * b.x, a.y * b.y };
        return res;
    }

    friend vec2& operator*=(vec2& a, const vec2& b)
    {
        a = a * b;
        return a;
    }

    friend vec2 operator/(const vec2& a, const vec2& b)
    {
        vec2 res = { a.x / b.x, a.y / b.y };
        return res;
    }

    friend vec2& operator/=(vec2& a, const vec2& b)
    {
        a = a / b;
        return a;
    }

    friend vec2 operator-(vec2 a)
    {
        a.x = -a.x;
        a.y = -a.y;
        return a;
    }
};
