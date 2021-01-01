#include "PerlinNoiseRandom.h"

PerlinNoiseRandom::PerlinNoiseRandom()
    : PerlinNoiseRandom(PerlinNoiseTextureSampler(256))
{}

PerlinNoiseRandom::PerlinNoiseRandom(PerlinNoiseTextureSampler sampler)
    : pos(GetRandomPos())
    , posIncrement(GetRandomIncrement())
    , sampler(std::move(sampler))
{}

PerlinNoiseRandom::PerlinNoiseRandom(const PerlinNoiseRandom& other)
    : pos(other.GetRandomPosOnCopy())
    , posIncrement(other.GetRandomIncrementOnCopy())
    , sampler(other.sampler)
{}

float PerlinNoiseRandom::Next()
{
    float val = this->sampler.Sample(this->pos);

    this->pos.x += this->posIncrement.x;

    if (this->pos.x >= 1.f)
    {
        this->pos.x = 0.f;
        this->pos.y += this->posIncrement.y;

        if (this->pos.y >= 1.f)
        {
            this->pos.y = 0.f;
        }
    }

    return val;
}

std::unique_ptr<IRandom<float>> PerlinNoiseRandom::Clone() const
{
    return std::make_unique<PerlinNoiseRandom>(*this);
}

vec2<float> PerlinNoiseRandom::GetRandomPosOnCopy() const
{
    return vec2<float>();
}

vec2<float> PerlinNoiseRandom::GetRandomIncrementOnCopy() const
{
    return vec2<float>();
}

vec2<float> PerlinNoiseRandom::GetRandomPos()
{
    return vec2<float>();
}

vec2<float> PerlinNoiseRandom::GetRandomIncrement()
{
    return vec2<float>();
}
