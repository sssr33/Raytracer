#include "Metal.h"

Metal::Metal(const vec3<float>& albedo)
    : albedo(albedo)
{}

std::optional<ScatterRecord> Metal::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
    vec3<float> reflected = r.direction.normalized().reflect(hitRecord.normal);

    ScatterRecord rec;

    rec.scattered = ray<float>(hitRecord.point, reflected);
    rec.attenuation = this->albedo;

    if (rec.scattered.direction.dot(hitRecord.normal) >= 0)
    {
        return rec;
    }

    // reflected inside surface
    return std::nullopt;
}
