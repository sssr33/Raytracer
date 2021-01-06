#pragma once
#include "Math/ray.h"

#include <optional>

struct HitRecord
{
	float rayT;
	vec3<float> point;
	vec3<float> normal;
	std::optional<vec3<float>> color;
};
