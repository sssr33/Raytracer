#pragma once
#include "Math/ray.h"

class IRandomInUnitSphere
{
public:
    virtual ~IRandomInUnitSphere() = default;

    virtual vec3<float> RandomInUnitSphere(const ray<float>& r) const = 0;
};
