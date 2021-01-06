#pragma once

#include "swizzleOp3.h"

#include <cmath>

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper;

template <typename T = float>
struct vec3
{
    union {
        struct { T x, y, z; };
        struct { T r, g, b; };
        T data[3];

        union
        {
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<0, 0>> x;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<1, 0>> y;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<2, 0>> z;

            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<0, 0>> r;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<1, 0>> g;
            swizzleOp3<swizzleOpVecHelper, vec3, T, 3, IndexPair<2, 0>> b;
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

    friend vec3& operator+=(vec3& a, const vec3& b)
    {
        a = a + b;
        return a;
    }

    friend vec3 operator-(const vec3& a, const vec3& b)
    {
        vec3 res = { a.x - b.x, a.y - b.y, a.z - b.z };
        return res;
    }

    friend vec3& operator-=(vec3& a, const vec3& b)
    {
        a = a - b;
        return a;
    }

    friend vec3 operator*(const vec3& a, const vec3& b)
    {
        vec3 res = { a.x * b.x, a.y * b.y, a.z * b.z };
        return res;
    }

    friend vec3& operator*=(vec3& a, const vec3& b)
    {
        a = a * b;
        return a;
    }

    friend vec3 operator/(const vec3& a, const vec3& b)
    {
        vec3 res = { a.x / b.x, a.y / b.y, a.z / b.z };
        return res;
    }

    friend vec3& operator/=(vec3& a, const vec3& b)
    {
        a = a / b;
        return a;
    }

    friend vec3 operator-(vec3 a)
    {
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        return a;
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

    vec3 normalized() const
    {
        T lengthInv = T(1) / this->length();
        vec3 res = *this * lengthInv;
        return res;
    }

    void normalize()
    {
        *this = normalized();
    }

    T dot(const vec3& b) const
    {
        const vec3& a = *this;
        float dotProduct =
            a.x * b.x +
            a.y * b.y +
            a.z * b.z;

        return dotProduct;
    }

    vec3 cross(const vec3& b) const
    {
        const vec3& a = *this;
        vec3 res =
        {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };

        return res;
    }

    vec3 lerp(const vec3& b, T t) const
    {
        const vec3& a = *this;
        vec3 res = a + t * (b - a);

        return res;
    }
};
