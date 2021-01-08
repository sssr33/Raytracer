#include "Lambertian.h"

Lambertian::Lambertian(const vec3<float>& albedo, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere)
    : albedo(albedo)
    , randomInUnitSphere(std::move(randomInUnitSphere))
{}

std::optional<ScatterRecord> Lambertian::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
    vec3<float> target = hitRecord.point + hitRecord.normal + this->randomInUnitSphere->RandomInUnitSphere(r);

    ScatterRecord rec;

    rec.scattered = ray<float>(hitRecord.point, target - hitRecord.point);
    rec.attenuation = this->albedo;

    return rec;
}
