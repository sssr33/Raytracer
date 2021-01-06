#pragma once
#include "IPixelAntiAliasing.h"

#include <vector>

class PixelMsaa : public IPixelAntiAliasing
{
public:
	explicit PixelMsaa(size_t sampleCount);
	explicit PixelMsaa(const vec2<size_t>& sampleCount);

	vec3<float> Resolve(const vec2<float>& pixCoord, const IPixelSampler<vec3<float>>& pixSampler) const override;

private:
	static std::vector<vec2<float>> MakeOffsets(const vec2<size_t>& sampleCount);

	std::vector<vec2<float>> sampleOffsets;
	float normalizeK;
};
