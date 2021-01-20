#pragma once

#include "swizzleOp2.h"

#include <cmath>

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

    T dot(const vec2& b) const
    {
        const vec2& a = *this;
        float dotProduct =
            a.x * b.x +
            a.y * b.y;

        return dotProduct;
    }

    vec2 lerp(const vec2& b, T t) const
    {
        const vec2& a = *this;
        vec2 res = a + t * (b - a);

        return res;
    }

    T lengthSquared() const
    {
        T squaredlLength = this->dot(*this);
        return squaredlLength;
    }

    T length() const
    {
        T length = std::sqrt(this->lengthSquared());
        return length;
    }

    vec2 normalized() const
    {
        T lengthInv = T(1) / this->length();
        vec2 res = *this * lengthInv;
        return res;
    }

    void normalize()
    {
        *this = normalized();
    }
};
