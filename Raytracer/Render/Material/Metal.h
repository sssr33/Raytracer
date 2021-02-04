#pragma once
#include "IMaterial.h"

#include <memory>

class Metal : public IMaterial
{
public:
    Metal(const vec3<float>& albedo, float fuzziness);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> albedo;
    float fuzziness;
};
