#include "Lambertian.h"

Lambertian::Lambertian(const vec3<float>& albedo, std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler)
    : albedo(albedo)
    , rayNoiseSampler(std::move(rayNoiseSampler))
{}

std::optional<ScatterRecord> Lambertian::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
    vec3<float> target = hitRecord.point + hitRecord.normal + this->RandomInUnitSphere(r);

    ScatterRecord rec;

    rec.scattered = ray<float>(hitRecord.point, target - hitRecord.point);
    rec.attenuation = this->albedo;

    return rec;
}

vec3<float> Lambertian::RandomInUnitSphere(const ray<float>& r) const
{
    vec3<float> rndVec =
    {
        this->rayNoiseSampler->Sample(r),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.1f, 0.1f, 0.f), r.direction)),
        this->rayNoiseSampler->Sample(ray<float>(r.origin + vec3<float>(0.0f, 0.1f, 0.1f), r.direction))
    };

    return rndVec.normalized();
}
