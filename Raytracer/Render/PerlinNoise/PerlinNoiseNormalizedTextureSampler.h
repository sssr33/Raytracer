#pragma once
#include "PerlinNoiseTextureSampler.h"

class PerlinNoiseNormalizedTextureSampler : public PerlinNoiseTextureSampler
{
public:
    using PerlinNoiseTextureSampler::PerlinNoiseTextureSampler;

    float Sample(const vec2<float>& texCoords) const override;
};
