#include "PixelMsaa.h"

PixelMsaa::PixelMsaa(size_t sampleCount)
	: PixelMsaa(vec2<size_t>(sampleCount, sampleCount))
{}

PixelMsaa::PixelMsaa(const vec2<size_t>& sampleCount)
	: sampleOffsets(PixelMsaa::MakeOffsets(sampleCount))
	, normalizeK(1.f / static_cast<float>(sampleCount.x * sampleCount.y))
{}

vec3<float> PixelMsaa::Resolve(const vec2<float>& pixCoord, const IPixelSampler<vec3<float>>& pixSampler) const
{
    vec3<float> color = { 0.f, 0.f, 0.f };

    for (const vec2<float>& offset : this->sampleOffsets)
    {
        const vec2<float> subPixCoord = pixCoord + offset;
        color += pixSampler.Sample(subPixCoord);
    }

    color *= this->normalizeK;

    return color;
}

std::vector<vec2<float>> PixelMsaa::MakeOffsets(const vec2<size_t>& sampleCount)
{
    std::vector<vec2<float>> offsets;
    offsets.reserve(sampleCount.x * sampleCount.y);

    const float stepX = 1.f / static_cast<float>(sampleCount.x);
    const float stepY = 1.f / static_cast<float>(sampleCount.y);

    for (size_t y = 0; y < sampleCount.y; y++)
    {
        const float v = stepY * static_cast<float>(y);

        for (size_t x = 0; x < sampleCount.x; x++)
        {
            const float u = stepX * static_cast<float>(x);
            offsets.emplace_back(u, v);
        }
    }

    return offsets;
}
