#pragma once
#include "vec.h"

template<class T>
struct ray
{
    ray() = default;
    ray(const vec3<T>& origin, const vec3<T>& direction, T time)
        : origin(origin), direction(direction), time(time)
    {}

    vec3<T> pointAtParameter(float t) const
    {
        vec3<T> pt = this->origin + t * this->direction;
        return pt;
    }

    vec3<T> origin;
    vec3<T> direction;
    T time;
};
