#pragma once
#include "IMaterial.h"
#include "Render/Random/IRandomInUnitSphere.h"

#include <memory>

class Lambertian : public IMaterial
{
public:
    Lambertian(const vec3<float>& albedo, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> albedo;
    std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere;
};
