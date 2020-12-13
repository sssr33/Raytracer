#include "Sphere.h"

Sphere::Sphere(const vec3<float>& center, float radius)
	: center(center)
	, radius(radius)
{}

std::optional<HitRecord> Sphere::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    vec3<float> fromSphereToRayOrigin = ray.origin - this->center;

    float a = ray.direction.dot(ray.direction);
    float b = ray.direction.dot(fromSphereToRayOrigin);
    float c = fromSphereToRayOrigin.dot(fromSphereToRayOrigin) - radius * radius;

    float discriminant = b * b - a * c;

    if (discriminant >= 0.f)
    {
        const float discriminantSqRoot = std::sqrt(discriminant);

        float tClosest = (-b - discriminantSqRoot) / a;
        if (tClosest > tMin && tClosest < tMax)
        {
            return this->MakeHitRecord(ray, tClosest);
        }

        float tFarthest = (-b + discriminantSqRoot) / a;
        if (tFarthest > tMin && tFarthest < tMax)
        {
            return this->MakeHitRecord(ray, tFarthest);
        }
    }

    return std::nullopt;
}

HitRecord Sphere::MakeHitRecord(const ray<float>& ray, float t) const
{
    HitRecord hitRec;

    hitRec.rayT = t;
    hitRec.point = ray.pointAtParameter(hitRec.rayT);
    hitRec.normal = (hitRec.point - this->center) / this->radius;

    return hitRec;
}
