#pragma once
#include "Math/ray.h"

#include <optional>

class IMaterial;

struct HitRecord
{
	float rayT = 0.f;
	vec3<float> point = { 0.f, 0.f, 0.f };
	vec3<float> normal = { 0.f, 0.f, 0.f };
	IMaterial* material = nullptr;
};
