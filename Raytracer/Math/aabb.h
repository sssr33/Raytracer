#pragma once
#include "ray.h"

class aabb
{
public:
	aabb() = default;
	aabb(const vec3<float>& min, const vec3<float>& max);

	bool Hit(const ray<float>& r, float tMin, float tMax) const;

	aabb United(const aabb& other) const;

	vec3<float> min;
	vec3<float> max;

private:
	static float GetT(size_t i, const vec3<float>& pt, const ray<float>& r);
};
