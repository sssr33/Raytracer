#pragma once
#include "IPixelAntiAliasing.h"

#include <vector>

// rgb coverage defines % of how many samples will be used for each component.
// Component(red, green, blue) is sampled from left to right with red on left and blue on right.
// Subpixel geometry is 3 rectangles with equal height and <coverage> width.
class SubpixelMsaa : public IPixelAntiAliasing
{
public:
	static const float DefaultGreenCoverage;

	explicit SubpixelMsaa(size_t sampleCount);
	SubpixelMsaa(size_t sampleCount, float greenCoverage);
	SubpixelMsaa(size_t sampleCount, const vec3<float>& rgbCoverage);
	explicit SubpixelMsaa(const vec2<size_t>& sampleCount);
	SubpixelMsaa(const vec2<size_t>& sampleCount, float greenCoverage);
	SubpixelMsaa(const vec2<size_t>& sampleCount, const vec3<float>& rgbCoverage);

	vec3<float> Resolve(const vec2<float>& pixCoord, const IPixelSampler& pixSampler) const override;

private:
	static const vec3<float> RedMask;
	static const vec3<float> GreenMask;
	static const vec3<float> BlueMask;

	struct Offset
	{
		vec2<float> offset;
		vec3<float> colorMask;
	};

	struct InitData
	{
		std::vector<Offset> sampleOffsets;
		vec3<float> normalizeK;
	};

	explicit SubpixelMsaa(InitData&& initData);

	static InitData MakeInitData(const vec2<size_t>& sampleCount, const vec3<float>& rgbCoverage);
	static void AddOffsets(float stepX, float v, size_t startX, size_t endX, const vec3<float>& colorMask, std::vector<Offset>& offsets);

	// computes equal coverage for red and blue
	static vec3<float> GetRGBCoverageFromGreen(float greenCoverage);

	std::vector<Offset> sampleOffsets;
	vec3<float> normalizeK;
};
