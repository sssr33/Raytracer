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

    vec2 operator+(const vec2& other) const
    {
        vec2 res = { this->x + other.x, this->y + other.y };
        return res;
    }

    vec2& operator+=(const vec2& other)
    {
        vec2 tmp = *this + other;
        *this = tmp;

        return *this;
    }
};
