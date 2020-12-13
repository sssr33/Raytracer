#pragma once
#include "IHitable.h"

class Triangle : public IHitable
{
public:
	Triangle(const vec3<float>& v0, const vec3<float>& v1, const vec3<float>& v2);

	std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;

	vec3<float> v0;
	vec3<float> v1;
	vec3<float> v2;
};
