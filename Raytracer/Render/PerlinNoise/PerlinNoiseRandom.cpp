#include "PerlinNoiseRandom.h"

#include <random>

PerlinNoiseRandom::PerlinNoiseRandom()
    : PerlinNoiseRandom(PerlinNoiseTextureSampler(256))
{}

PerlinNoiseRandom::PerlinNoiseRandom(PerlinNoiseTextureSampler sampler)
    : pos(GetRandomPos())
    , posIncrement(GetRandomIncrement())
    , sampler(std::move(sampler))
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

    // [0...1) range
    val += 1.0;
    val /= 2.0;
    val = fmodf(val, 1.f);

    return val;
}

std::unique_ptr<IRandom<float>> PerlinNoiseRandom::Clone() const
{
    return std::make_unique<PerlinNoiseRandom>(*this);
}

vec2<float> PerlinNoiseRandom::GetRandomPos()
{
    std::random_device rd;
    std::mt19937 g(rd());

    float x = static_cast<float>(g() % 100000u) / 100000.f;
    float y = static_cast<float>(g() % 100000u) / 100000.f;

    return vec2<float>(x, y);
}

vec2<float> PerlinNoiseRandom::GetRandomIncrement()
{
    std::random_device rd;
    std::mt19937 g(rd());

    float x = static_cast<float>(g() % 100000u) / 100000.f;
    float y = static_cast<float>(g() % 100000u) / 100000.f;

    x *= 0.1f;
    y *= 0.1f;

    return vec2<float>(x, y);
}
