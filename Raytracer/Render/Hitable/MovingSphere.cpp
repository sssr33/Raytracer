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

std::optional<aabb> MovingSphere::GetBoundingBox(float time0, float time1) const
{
    Sphere sphere0(this->GetCenter(time0), this->radius, std::unique_ptr<IMaterial>(this->material.get()));
    Sphere sphere1(this->GetCenter(time1), this->radius, std::unique_ptr<IMaterial>(this->material.get()));

    std::optional<aabb> box0 = sphere0.GetBoundingBox(time0, time1);
    std::optional<aabb> box1 = sphere1.GetBoundingBox(time0, time1);

    sphere0.material.release();
    sphere1.material.release();

    if (!box0 || !box1)
    {
        return std::nullopt;
    }

    aabb res = box0->United(*box1);
    return res;
}

vec3<float> MovingSphere::GetCenter(float time) const
{
    float t = (time - this->timeStart) / (this->timeEnd - this->timeStart);
    vec3<float> center = this->centerStart.lerp(this->centerEnd, t);
    return center;
}
