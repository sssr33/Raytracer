#include "aabb.h"

#include <algorithm>

aabb::aabb(const vec3<float>& min, const vec3<float>& max)
	: min(min)
	, max(max)
{}

bool aabb::Hit(const ray<float>& ray, float tMin, float tMax) const
{
	for (size_t i = 0; i < 3; i++)
	{
		float tmpMin = GetT(i, min, ray);
		float tmpMax = GetT(i, max, ray);

		float t0 = std::min(tmpMin, tmpMax);
		float t1 = std::max(tmpMin, tmpMax);

		tMin = std::max(tMin, t0);
		tMax = std::min(tMax, t1);

		if (tMax <= tMin)
		{
			return false;
		}
	}

	return true;
}

aabb aabb::United(const aabb& other) const
{
	aabb result = {};

	for (int i = 0; i < 3; i++)
	{
		result.min[i] = std::min(this->min[i], other.min[i]);
		result.max[i] = std::max(this->max[i], other.max[i]);
	}

	return result;
}

float aabb::GetT(size_t i, const vec3<float>& pt, const ray<float>& r)
{
	float t = (pt[i] - r.origin[i]) / r.direction[i];
	return t;
}
