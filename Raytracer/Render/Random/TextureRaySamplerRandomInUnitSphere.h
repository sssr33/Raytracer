#pragma once
#include "IRandomInUnitSphere.h"
#include "Render/Sampler/TextureRaySampler.h"

#include <memory>

class TextureRaySamplerRandomInUnitSphere : public IRandomInUnitSphere
{
public:
    TextureRaySamplerRandomInUnitSphere(std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler);

    vec3<float> RandomInUnitSphere(const ray<float>& r) const override;

private:
    std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler;
};
