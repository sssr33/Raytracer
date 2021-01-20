#pragma once
#include "Math/ray.h"

class ICamera
{
public:
	virtual ~ICamera() = default;

	virtual ray<float> GetRay(const vec2<float>& uv) const = 0;
};
