#include "PerlinNoiseTextureSampler.h"

#include <algorithm>
#include <iterator>
#include <random>
#include <cassert>

PerlinNoiseTextureSampler::PerlinNoiseTextureSampler(size_t noiseTexSize)
	: PerlinNoiseTextureSampler(GenPermutationTable(noiseTexSize), GetDefaultConstantVectors())
{}

PerlinNoiseTextureSampler::PerlinNoiseTextureSampler(
	std::vector<uint32_t> permutationTable,
	std::vector<vec2<float>> constantVectors)
	: permutationTable(DoublePermutationTable(std::move(permutationTable)))
	, constantVectors(std::move(constantVectors))
	, noiseTexSizeF(static_cast<float>(this->permutationTable.size() / 2) - 1.f) // -1 to not include index == size
	, noiseTexSize(this->permutationTable.size() / 2)
{}

float PerlinNoiseTextureSampler::Sample(const vec2<float>& texCoords) const
{
	vec2<float> pixCoords = WrapTexCoords(texCoords) * this->noiseTexSizeF;

	float flooredX = std::floor(pixCoords.x);
	float flooredY = std::floor(pixCoords.y);

	size_t x = static_cast<size_t>(flooredX) % this->noiseTexSize;
	size_t y = static_cast<size_t>(flooredY) % this->noiseTexSize;
	float xf = pixCoords.x - flooredX;
	float yf = pixCoords.y - flooredY;

	vec2<float> topRight(xf - 1.0f, yf - 1.0f);
	vec2<float> topLeft(xf, yf - 1.0f);
	vec2<float> bottomRight(xf - 1.0f, yf);
	vec2<float> bottomLeft(xf, yf);

	uint32_t valueTopRight = this->GetPermutationTableValue(x + 1, y + 1);
	uint32_t valueTopLeft = this->GetPermutationTableValue(x, y + 1);
	uint32_t valueBottomRight = this->GetPermutationTableValue(x + 1, y);
	uint32_t valueBottomLeft = this->GetPermutationTableValue(x, y);

	float dotTopRight = topRight.dot(this->GetConstantVector(valueTopRight));
	float dotTopLeft = topLeft.dot(this->GetConstantVector(valueTopLeft));
	float dotBottomRight = bottomRight.dot(this->GetConstantVector(valueBottomRight));
	float dotBottomLeft = bottomLeft.dot(this->GetConstantVector(valueBottomLeft));

	float u = Fade(xf);
	float v = Fade(yf);

	float result = Lerp(
		Lerp(dotBottomLeft, dotTopLeft, v),
		Lerp(dotBottomRight, dotTopRight, v),
		u);

	return result;
}

std::unique_ptr<ITextureSampler<float>> PerlinNoiseTextureSampler::Clone() const
{
	return std::make_unique<PerlinNoiseTextureSampler>(*this);
}

uint32_t PerlinNoiseTextureSampler::GetPermutationTableValue(size_t x, size_t y) const
{
	assert(x < this->noiseTexSize);
	assert(y < this->noiseTexSize);

	return this->permutationTable[this->permutationTable[x] + y];
}

const vec2<float>& PerlinNoiseTextureSampler::GetConstantVector(uint32_t index) const
{
	uint32_t normIdx = index % static_cast<uint32_t>(this->constantVectors.size());
	return this->constantVectors[normIdx];
}

float PerlinNoiseTextureSampler::Fade(float t)
{
	return ((6.f * t - 15.f) * t + 10.f) * t * t * t;
}

float PerlinNoiseTextureSampler::Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

std::vector<uint32_t> PerlinNoiseTextureSampler::GenPermutationTable(size_t noiseTexSize)
{
	std::vector<uint32_t> permutationTable;

	// reserve 2x size to take into account next call to DoublePermutationTable
	permutationTable.reserve(noiseTexSize * 2);

	std::generate_n(std::back_inserter(permutationTable), noiseTexSize,
		[counter = 0u]() mutable
	{
		return counter++;
	});

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(permutationTable.begin(), permutationTable.end(), g);

	return permutationTable;
}

std::vector<uint32_t> PerlinNoiseTextureSampler::DoublePermutationTable(std::vector<uint32_t> permutationTable)
{
	permutationTable.reserve(permutationTable.size() * 2);
	permutationTable.insert(permutationTable.end(), permutationTable.begin(), permutationTable.end());

	return permutationTable;
}

std::vector<vec2<float>> PerlinNoiseTextureSampler::GetDefaultConstantVectors()
{
	return
	{
		vec2<float>(1.f, 1.f),
		vec2<float>(-1.f, 1.f),
		vec2<float>(-1.f, -1.f),
		vec2<float>(1.f, -1.f)
	};
}

vec2<float> PerlinNoiseTextureSampler::WrapTexCoords(const vec2<float>& texCoords)
{
	vec2<float> wrapped;

	wrapped.x = std::fmodf(texCoords.x, 1.f);
	wrapped.y = std::fmodf(texCoords.y, 1.f);

	if (wrapped.x < 0.f)
	{
		wrapped.x += 1.f;
	}

	if (wrapped.y < 0.f)
	{
		wrapped.y += 1.f;
	}

	return wrapped;
}
