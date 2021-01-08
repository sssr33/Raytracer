#pragma once
#include "IMaterial.h"
#include "Render/Random/IRandomInUnitSphere.h"

#include <memory>

class Metal : public IMaterial
{
public:
    Metal(const vec3<float>& albedo, float fuzziness, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> albedo;
    float fuzziness;
    std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere;
};
