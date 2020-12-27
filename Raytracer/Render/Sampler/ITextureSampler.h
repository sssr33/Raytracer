#pragma once

#include "Math/vec.h"

// intended to support coordinates in range from 0;0 to 1;1
// beyond that range implementation specific logic may be applied
template<class PixelT>
class ITextureSampler
{
public:
	virtual ~ITextureSampler() = default;

	virtual PixelT Sample(const vec2<float> & texCoords) const = 0;
};
