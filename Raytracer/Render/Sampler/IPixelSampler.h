#pragma once

#include "Math/vec.h"

class IPixelSampler
{
public:
	virtual ~IPixelSampler() = default;

	virtual vec3<float> Sample(const vec2<float>& pixCoords) const = 0;
};
