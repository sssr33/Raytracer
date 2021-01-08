#pragma once
#include "Math/ray.h"

struct ScatterRecord
{
	ray<float> scattered;
	vec3<float> attenuation;
};
