#pragma once
#include "IMaterial.h"

class Metal : public IMaterial
{
public:
    Metal(const vec3<float>& albedo);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> albedo;
};
