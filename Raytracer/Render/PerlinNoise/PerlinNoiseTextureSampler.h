#pragma once
#include "Render/Sampler/ITextureSampler.h"

#include <vector>
#include <Helpers/Size2D.h>

// based on https://rtouti.github.io/graphics/perlin-noise-algorithm
class PerlinNoiseTextureSampler : public ITextureSampler<float>
{
public:
	explicit PerlinNoiseTextureSampler(size_t noiseTexSize);
	PerlinNoiseTextureSampler(
		std::vector<uint32_t> permutationTable,
		std::vector<vec2<float>> constantVectors);

	float Sample(const vec2<float>& texCoords) const override;

private:
	uint32_t GetPermutationTableValue(size_t x, size_t y) const;
	const vec2<float>& GetConstantVector(uint32_t index) const;

	static float Fade(float t);
	static float Lerp(float a, float b, float t);
	static std::vector<uint32_t> GenPermutationTable(size_t noiseTexSize);
	static std::vector<uint32_t> DoublePermutationTable(std::vector<uint32_t> permutationTable);
	static std::vector<vec2<float>> GetDefaultConstantVectors();
	static vec2<float> WrapTexCoords(const vec2<float>& texCoords);

	std::vector<uint32_t> permutationTable;
	std::vector<vec2<float>> constantVectors;
	float noiseTexSizeF = 0.f;
	size_t noiseTexSize = 0;
};
