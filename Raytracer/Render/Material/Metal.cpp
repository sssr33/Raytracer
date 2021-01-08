#include "Metal.h"

#include <algorithm>

Metal::Metal(const vec3<float>& albedo, float fuzziness, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere)
    : albedo(albedo)
    , fuzziness(std::clamp(fuzziness, 0.f, 1.f))
    , randomInUnitSphere(std::move(randomInUnitSphere))
{}

std::optional<ScatterRecord> Metal::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
    vec3<float> reflected = r.direction.normalized().reflect(hitRecord.normal);

    ScatterRecord rec;

    rec.scattered = ray<float>(hitRecord.point, reflected + this->fuzziness * this->randomInUnitSphere->RandomInUnitSphere(r));
    rec.attenuation = this->albedo;

    if (rec.scattered.direction.dot(hitRecord.normal) >= 0)
    {
        return rec;
    }

    // reflected inside surface
    return std::nullopt;
}
