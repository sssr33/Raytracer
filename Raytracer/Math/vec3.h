#pragma once
#include "GetSetVec.h"
#include "swizzle.h"

struct vec3
{
    vec3() = default;
    explicit vec3(float v);
    vec3(float x, float y, float z);

    template<class VecT, class... T>
    vec3(const VecRef<VecT, T...>& other)
        : x(other.getX())
        , y(other.getY())
        , z(other.getZ())
    {}

    float getX() const;
    float getY() const;
    float getZ() const;

    void setX(float v);
    void setY(float v);
    void setZ(float v);

    float x;
    float y;
    float z;
};
