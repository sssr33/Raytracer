#pragma once

#include <cstdint>

// https://developer.nvidia.com/gpugems/gpugems3/part-vi-gpu-computing/chapter-37-efficient-random-number-generation-and-application
// returns random from 0.f to 1.f
class HybridTaus
{
public:
	HybridTaus();
	HybridTaus(uint32_t z1, uint32_t z2, uint32_t z3, uint32_t z4);

	float operator()();

private:
	static uint32_t TausStep(uint32_t z, int S1, int S2, int S3, uint32_t M);
	static uint32_t LCGStep(uint32_t z, uint32_t A, uint32_t C);

	uint32_t z1 = 0;
	uint32_t z2 = 0;
	uint32_t z3 = 0;
	uint32_t z4 = 0;
};
