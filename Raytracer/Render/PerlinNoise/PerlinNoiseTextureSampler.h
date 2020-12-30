#pragma once
#include "Render/Sampler/ITextureSampler.h"

#include <vector>
#include <Helpers/Size2D.h>

// based on https://rtouti.github.io/graphics/perlin-noise-algorithm#:~:text=Perlin%20noise%20is%20a%20popular,number%20of%20inputs%20it%20gets.
class PerlinNoiseTextureSampler : public ITextureSampler<vec3<float>>
{
public:
	PerlinNoiseTextureSampler(size_t noiseTexSize);
	PerlinNoiseTextureSampler(std::vector<size_t> permutationTable);

	vec3<float> Sample(const vec2<float>& texCoords) const override;

private:
	std::vector<size_t> permutationTable;

	static std::vector<size_t> GenPermutationTable(size_t noiseTexSize);
	static std::vector<size_t> DoublePermutationTable(std::vector<size_t> permutationTable);
};
