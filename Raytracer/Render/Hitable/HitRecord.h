#pragma once
#include "Math/ray.h"

struct HitRecord
{
	float rayT;
	vec3<float> point;
	vec3<float> normal;
};
