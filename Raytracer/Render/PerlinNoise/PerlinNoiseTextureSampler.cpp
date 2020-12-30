#include "PerlinNoiseTextureSampler.h"

PerlinNoiseTextureSampler::PerlinNoiseTextureSampler(size_t noiseTexSize)
	: PerlinNoiseTextureSampler(GenPermutationTable(noiseTexSize))
{}

PerlinNoiseTextureSampler::PerlinNoiseTextureSampler(std::vector<size_t> permutationTable)
	: permutationTable(DoublePermutationTable(std::move(permutationTable)))
{}

vec3<float> PerlinNoiseTextureSampler::Sample(const vec2<float>& texCoords) const
{
	vec3<float> color = {};

	return color;
}

std::vector<size_t> PerlinNoiseTextureSampler::GenPermutationTable(size_t noiseTexSize)
{
	std::vector<size_t> permutationTable;

	// reserve 2x size to take into account next call to DoublePermutationTable
	permutationTable.reserve(noiseTexSize * 2);

	return permutationTable;
}

std::vector<size_t> PerlinNoiseTextureSampler::DoublePermutationTable(std::vector<size_t> permutationTable)
{
	// 
	permutationTable.reserve(permutationTable.size() * 2);
	permutationTable.insert(permutationTable.end(), permutationTable.begin(), permutationTable.end());

	return permutationTable;
}
