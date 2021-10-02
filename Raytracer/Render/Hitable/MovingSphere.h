#pragma once
#include "IHitable.h"

class MovingSphere : public IHitable
{
public:
    MovingSphere(
        const vec3<float>& centerStart, const vec3<float>& centerEnd,
        float timeStart, float timeEnd,
        float radius,
        std::unique_ptr<IMaterial> material);

    std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;
    std::optional<aabb> GetBoundingBox(float time0, float time1) const override;

    vec3<float> centerStart;
    vec3<float> centerEnd;
    float timeStart;
    float timeEnd;
    float radius;
    std::unique_ptr<IMaterial> material;

private:
    vec3<float> GetCenter(float time) const;
};
