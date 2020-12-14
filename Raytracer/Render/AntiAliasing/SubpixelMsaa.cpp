#include "SubpixelMsaa.h"

const float SubpixelMsaa::DefaultGreenCoverage = 1.f / 3.f;
const vec3<float> SubpixelMsaa::RedMask = { 1.f, 0.f, 0.f };
const vec3<float> SubpixelMsaa::GreenMask = { 0.f, 1.f, 0.f };
const vec3<float> SubpixelMsaa::BlueMask = { 0.f, 0.f, 1.f };

SubpixelMsaa::SubpixelMsaa(size_t sampleCount)
	: SubpixelMsaa(sampleCount, SubpixelMsaa::DefaultGreenCoverage)
{}

SubpixelMsaa::SubpixelMsaa(size_t sampleCount, float greenCoverage)
	: SubpixelMsaa(
		sampleCount,
		SubpixelMsaa::GetRGBCoverageFromGreen(greenCoverage))
{}

SubpixelMsaa::SubpixelMsaa(size_t sampleCount, const vec3<float>& rgbCoverage)
	: SubpixelMsaa(vec2<size_t>(sampleCount, sampleCount), rgbCoverage)
{}

SubpixelMsaa::SubpixelMsaa(const vec2<size_t>& sampleCount)
	: SubpixelMsaa(sampleCount, SubpixelMsaa::DefaultGreenCoverage)
{}

SubpixelMsaa::SubpixelMsaa(const vec2<size_t>& sampleCount, float greenCoverage)
	: SubpixelMsaa(
		sampleCount,
		SubpixelMsaa::GetRGBCoverageFromGreen(greenCoverage)
	)
{}

SubpixelMsaa::SubpixelMsaa(const vec2<size_t>& sampleCount, const vec3<float>& rgbCoverage)
	: SubpixelMsaa(SubpixelMsaa::MakeInitData(sampleCount, rgbCoverage))
{}

vec3<float> SubpixelMsaa::Resolve(const vec2<float>& pixCoord, const IPixelSampler& pixSampler) const
{
	vec3<float> color = { 0.f, 0.f, 0.f };

	for (const Offset& offset : this->sampleOffsets)
	{
		const vec2<float> subPixCoord = pixCoord + offset.offset;
		color += pixSampler.Sample(subPixCoord) * offset.colorMask;
	}

	color *= this->normalizeK;

	return color;
}

SubpixelMsaa::SubpixelMsaa(InitData&& initData)
	: sampleOffsets(std::move(initData.sampleOffsets))
	, normalizeK(std::move(initData.normalizeK))
{}

SubpixelMsaa::InitData SubpixelMsaa::MakeInitData(const vec2<size_t>& sampleCount, const vec3<float>& rgbCoverage)
{
	const float totalCoverage = rgbCoverage.r + rgbCoverage.g + rgbCoverage.b;
	const vec3<float> rgbCoverageNorm = rgbCoverage / totalCoverage;

	const size_t redPixCount = static_cast<size_t>(std::ceilf(static_cast<float>(sampleCount.x) * rgbCoverageNorm.r));
	const size_t greenPixCount = static_cast<size_t>(std::ceilf(static_cast<float>(sampleCount.x) * rgbCoverageNorm.g));
	const size_t bluePixCount = static_cast<size_t>(std::ceilf(static_cast<float>(sampleCount.x) * rgbCoverageNorm.b));

	const size_t width = redPixCount + greenPixCount + bluePixCount;
	const size_t height = sampleCount.y;

    std::vector<Offset> offsets;
    offsets.reserve(width * height);

    const float stepX = 1.f / static_cast<float>(width);
    const float stepY = 1.f / static_cast<float>(height);

    for (size_t y = 0; y < height; y++)
    {
        const float v = stepY * static_cast<float>(y);

        AddOffsets(stepX, v, 0, redPixCount, SubpixelMsaa::RedMask, offsets);
        AddOffsets(stepX, v, redPixCount, redPixCount + greenPixCount, SubpixelMsaa::GreenMask, offsets);
        AddOffsets(stepX, v, redPixCount + greenPixCount, width, SubpixelMsaa::BlueMask, offsets);
    }

	const float totalRedPixCount = static_cast<float>(redPixCount * height);
	const float totalGreenPixCount = static_cast<float>(greenPixCount * height);
	const float totalBluePixCount = static_cast<float>(bluePixCount * height);

	InitData initData;

	initData.normalizeK = 1.f / vec3<float>(totalRedPixCount, totalGreenPixCount, totalBluePixCount);
	initData.sampleOffsets = std::move(offsets);

	return initData;
}

void SubpixelMsaa::AddOffsets(float stepX, float v, size_t startX, size_t endX, const vec3<float>& colorMask, std::vector<Offset>& offsets)
{
    for (size_t x = startX; x < endX; x++)
    {
        const float u = stepX * static_cast<float>(x);
		offsets.push_back(Offset{ vec2<float>(u, v), colorMask });
    }
}

vec3<float> SubpixelMsaa::GetRGBCoverageFromGreen(float greenCoverage)
{
	float redBlueCoverage = (1.f - greenCoverage) / 2.f;
	return { redBlueCoverage, greenCoverage, redBlueCoverage };
}
