#pragma once
#include "IHitable.h"

class Sphere : public IHitable
{
public:
	Sphere(const vec3<float>& center, float radius, std::unique_ptr<IMaterial> material);

	std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;

	vec3<float> center;
	float radius;

private:
	HitRecord MakeHitRecord(const ray<float>& ray, float t) const;

	std::unique_ptr<IMaterial> material;
};
