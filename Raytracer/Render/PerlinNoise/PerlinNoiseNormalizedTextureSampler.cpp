#include "PerlinNoiseNormalizedTextureSampler.h"

float PerlinNoiseNormalizedTextureSampler::Sample(const vec2<float>& texCoords) const
{
    float sample = PerlinNoiseTextureSampler::Sample(texCoords);

    // from -1 ... 1 to 0 ... 1
    sample += 1.0;
    sample /= 2.0;

    return sample;
}
