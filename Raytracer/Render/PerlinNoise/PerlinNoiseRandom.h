#pragma once
#include "Random/IRandom.h"
#include "PerlinNoiseTextureSampler.h"

class PerlinNoiseRandom : public IRandom<float>
{
public:
    PerlinNoiseRandom();
    explicit PerlinNoiseRandom(PerlinNoiseTextureSampler sampler);
    PerlinNoiseRandom(PerlinNoiseRandom&&) = default;
    PerlinNoiseRandom(const PerlinNoiseRandom& other);

    PerlinNoiseRandom& operator=(PerlinNoiseRandom&&) = default;
    PerlinNoiseRandom& operator=(const PerlinNoiseRandom& other);

    float Next() override;

    std::unique_ptr<IRandom<float>> Clone() const override;

private:
    static vec2<float> GetRandomPos();
    static vec2<float> GetRandomIncrement();

    vec2<float> pos;
    vec2<float> posIncrement;
    PerlinNoiseTextureSampler sampler;
};
