#pragma once
#include "IMaterial.h"
#include "Render/Sampler/TextureRaySampler.h"

class Lambertian : public IMaterial
{
public:
    Lambertian(const vec3<float>& albedo, std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler);

    std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
    vec3<float> RandomInUnitSphere(const ray<float>& r) const;

    vec3<float> albedo;
    std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler;
};
