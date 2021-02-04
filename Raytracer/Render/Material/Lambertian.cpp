#include "Lambertian.h"
#include "Render/Random/RandomFunctions.h"

Lambertian::Lambertian(const vec3<float>& albedo, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere)
    : albedo(albedo)
    , randomInUnitSphere(std::move(randomInUnitSphere))
{}

std::optional<ScatterRecord> Lambertian::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
#if USE_IRandomInUnitSphere
    vec3<float> target = hitRecord.point + hitRecord.normal + this->randomInUnitSphere->RandomInUnitSphere(r);
#else
#if USE_RND_UNIT_SPHERE
    vec3<float> target = hitRecord.point + hitRecord.normal + RandomInUnitSphere();
#else
    vec3<float> target = hitRecord.point + hitRecord.normal + RandomInSphere();
#endif
#endif

    ScatterRecord rec;

    rec.scattered = ray<float>(hitRecord.point, target - hitRecord.point);
    rec.attenuation = this->albedo;

    return rec;
}
