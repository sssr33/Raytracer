#include "MovingSphere.h"
#include "Math/Math.h"
#include "Sphere.h"

MovingSphere::MovingSphere(
    const vec3<float>& centerStart, const vec3<float>& centerEnd,
    float timeStart, float timeEnd,
    float radius,
    std::unique_ptr<IMaterial> material)
    : centerStart(centerStart)
    , centerEnd(centerEnd)
    , timeStart(timeStart)
    , timeEnd(timeEnd)
    , radius(radius)
    , material(std::move(std::move(material)))
{}

std::optional<HitRecord> MovingSphere::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    Sphere sphere(this->GetCenter(ray.time), this->radius, std::unique_ptr<IMaterial>(this->material.get()));

    auto hitRecord = sphere.Hit(ray, tMin, tMax);
    sphere.material.release();

    return hitRecord;
}

vec3<float> MovingSphere::GetCenter(float time) const
{
    float t = (time - this->timeStart) / (this->timeEnd - this->timeStart);
    vec3<float> center = this->centerStart.lerp(this->centerEnd, t);
    return center;
}
