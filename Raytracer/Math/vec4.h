#pragma once

#include "swizzleOp4.h"

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper;

template <typename T = float>
struct vec4
{
    union {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        T data[4];

        union
        {
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<0, 0>> x;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<1, 0>> y;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<2, 0>> z;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<3, 0>> w;

            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<0, 0>> r;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<1, 0>> g;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<2, 0>> b;
            swizzleOp4<swizzleOpVecHelper, vec4, T, 3, IndexPair<3, 0>> a;
        } swizzle;
    };

    vec4() = default;
    vec4(const T& v) : x(v), y(v), z(v), w(v) {};
    vec4(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };

    friend vec4 operator+(const vec4& a, const vec4& b)
    {
        vec4 res = { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
        return res;
    }

    friend vec4& operator+=(vec4& a, const vec4& b)
    {
        a = a + b;
        return a;
    }

    friend vec4 operator-(const vec4& a, const vec4& b)
    {
        vec4 res = { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
        return res;
    }

    friend vec4& operator-=(vec4& a, const vec4& b)
    {
        a = a - b;
        return a;
    }

    friend vec4 operator*(const vec4& a, const vec4& b)
    {
        vec4 res = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
        return res;
    }

    friend vec4& operator*=(vec4& a, const vec4& b)
    {
        a = a * b;
        return a;
    }

    friend vec4 operator/(const vec4& a, const vec4& b)
    {
        vec4 res = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
        return res;
    }

    friend vec4& operator/=(vec4& a, const vec4& b)
    {
        a = a / b;
        return a;
    }

    friend vec4 operator-(vec4 a)
    {
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        a.w = -a.w;
        return a;
    }
};
