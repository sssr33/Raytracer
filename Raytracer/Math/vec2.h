#pragma once
#include "GetSetVec.h"
#include "swizzle.h"

struct vec2 : public VecRefBase2<vec2, vec2>
{
    vec2() = default;
    explicit vec2(float v);
    vec2(float x, float y);

    template<class VecT, class... T>
    vec2(const VecRef<VecT, T...>& other)
        : x(other.getX())
        , y(other.getY())
    {}

    float getX() const;
    float getY() const;

    void setX(float v);
    void setY(float v);

    float x;
    float y;
};
