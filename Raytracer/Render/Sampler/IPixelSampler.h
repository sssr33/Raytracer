#pragma once

#include "Math/vec.h"

// intended to support arbitary image size
template<class PixelT>
class IPixelSampler
{
public:
	virtual ~IPixelSampler() = default;

	virtual PixelT Sample(const vec2<float>& pixCoords) const = 0;
};
