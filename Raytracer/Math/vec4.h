#pragma once

#include "swizzleOp4.h"

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper;

template <typename T = float>
struct vec4
{
    union {
        struct { T x, y, z, w; };
        T data[4];

        union
        {
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<0, 0>> x;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<1, 0>> y;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<2, 0>> z;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<3, 0>> w;
        } swizzle;
    };

    vec4() = default;
    vec4(const T& v) : x(v), y(v), z(v), w(v) {};
    vec4(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };

    vec4 operator+(const vec4& other) const
    {
        vec4 res = { this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w };
        return res;
    }

    vec4& operator+=(const vec4& other)
    {
        vec4 tmp = *this + other;
        *this = tmp;

        return *this;
    }
};
