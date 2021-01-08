#include "TextureRaySamplerRandomInUnitSphere.h"

TextureRaySamplerRandomInUnitSphere::TextureRaySamplerRandomInUnitSphere(std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler)
    : rayNoiseSampler(std::move(rayNoiseSampler))
{}

vec3<float> TextureRaySamplerRandomInUnitSphere::RandomInUnitSphere(const ray<float>& r) const
{
    vec3<float> rndVec =
    {
        this->rayNoiseSampler->Sample(r),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.1f, 0.1f, 0.f), r.direction)),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.0f, 0.1f, 0.1f), r.direction))
    };

    return rndVec.normalized();
}
