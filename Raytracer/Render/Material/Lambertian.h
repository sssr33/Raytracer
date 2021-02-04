#pragma once
#include "IMaterial.h"

#include <memory>

class Lambertian : public IMaterial
{
public:
    Lambertian(const vec3<float>& albedo);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> albedo;
};
