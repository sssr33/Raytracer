#pragma once

#include "swizzleOp3.h"

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper;

template <typename T = float>
struct vec3
{
    union {
        struct { T x, y, z; };
        T data[3];

        union
        {
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<0, 0>> x;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<1, 0>> y;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<2, 0>> z;
        } swizzle;
    };

    vec3() = default;
    vec3(const T& v) : x(v), y(v), z(v) {};
    vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };

    friend vec3 operator+(const vec3& a, const vec3& b)
    {
        vec3 res = { a.x + b.x, a.y + b.y, a.z + b.z };
        return res;
    }

    template<class RightT>
    friend RightT operator+=(RightT& a, const vec3& b)
    {
        a = a + b;
        return a;
    }
};
