#pragma once
#include "Math/vec.h"
#include "Render/Sampler/IPixelSampler.h"

class IPixelAntiAliasing
{
public:
	virtual ~IPixelAntiAliasing() = default;

	// Resolves mulisampled pixel to a color at <pixCoords> using <pixSampler>.
	// The resolve term is taken from here https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#copies-resolve
	virtual vec3<float> Resolve(const vec2<float>& pixCoord, const IPixelSampler<vec3<float>>& pixSampler) const = 0;
};
