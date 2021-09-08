#include "Sphere.h"

Sphere::Sphere(const vec3<float>& center, float radius, std::unique_ptr<IMaterial> material)
	: center(center)
	, radius(radius)
    , material(std::move(material))
{}

std::optional<HitRecord> Sphere::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    /*
    * Equation simplification
    * float b = 2.f * ray.direction.dot(fromSphereToRayOrigin);
    * x = ray.direction.dot(fromSphereToRayOrigin)
    * 
    * float b = 2.f * x;
    * float discriminant = b * b - 4.f * a * c;
    * float discriminant = 2.f * x * 2.f * x - 4.f * a * c;
    * 
    * float tClosest = (-b - sqrt(discriminant)) / (2.f * a);
    * float tClosest = (-2.f * x - sqrt(2.f * x * 2.f * x - 4.f * a * c)) / (2.f * a);
    * float tClosest = (-2.f * x - sqrt(4.f * x * x - 4.f * a * c)) / (2.f * a);
    * float tClosest = (-2.f * x - sqrt(4.f * (x * x - a * c))) / (2.f * a);
    * float tClosest = (-2.f * x - sqrt(4) * sqrt(x * x - a * c)) / (2.f * a);
    * float tClosest = (-2.f * x - 2 * sqrt(x * x - a * c)) / (2.f * a);
    * divide equation by 2 to simplify
    * float tClosest = (-x - sqrt(x * x - a * c)) / a;
    * float tClosest = -(x + sqrt(x * x - a * c)) / a;
    */

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

std::optional<aabb> Sphere::GetBoundingBox(float time0, float time1) const
{
    aabb box(center + radius, center - radius);
    return box;
}

HitRecord Sphere::MakeHitRecord(const ray<float>& ray, float t) const
{
    HitRecord hitRec;

    hitRec.rayT = t;
    hitRec.point = ray.pointAtParameter(hitRec.rayT);
    hitRec.normal = (hitRec.point - this->center) / this->radius;
    hitRec.material = this->material.get();

    return hitRec;
}
