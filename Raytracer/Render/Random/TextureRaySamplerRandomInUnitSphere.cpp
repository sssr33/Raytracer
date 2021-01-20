#include "TextureRaySamplerRandomInUnitSphere.h"

TextureRaySamplerRandomInUnitSphere::TextureRaySamplerRandomInUnitSphere(std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler)
    : rayNoiseSampler(std::move(rayNoiseSampler))
{}

vec3<float> TextureRaySamplerRandomInUnitSphere::RandomInUnitSphere(const ray<float>& r) const
{
    vec3<float> rndVec;

    do
    {
        float x = std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);
        float y = std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);
        float z = std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);

        rndVec = 2.f * vec3<float>(x, y, z) - vec3<float>(1.f, 1.f, 1.f);
    } while (rndVec.lengthSquared() >= 1.f);

    return rndVec;

    /*vec3<float> rndVec =
    {
        this->rayNoiseSampler->Sample(r),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.1f, 0.1f, 0.f), r.direction)),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.0f, 0.1f, 0.1f), r.direction))
    };

    rndVec = 2.f * rndVec - 1.f;

    return rndVec.normalized();*/
}
