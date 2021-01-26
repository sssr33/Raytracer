#include "HybridTaus.h"

#include <random>

HybridTaus::HybridTaus()
    : HybridTaus(std::random_device()(), std::random_device()(), std::random_device()(), std::random_device()())
{}

HybridTaus::HybridTaus(uint32_t z1, uint32_t z2, uint32_t z3, uint32_t z4)
    : z1(z1)
    , z2(z2)
    , z3(z3)
    , z4(z4)
{}

float HybridTaus::operator()()
{
    // Periods

    // p1=2^31-1
    this->z1 = TausStep(this->z1, 13, 19, 12, 4294967294UL);
    // p2=2^30-1
    this->z2 = TausStep(this->z2, 2, 25, 4, 4294967288UL);
    // p3=2^28-1
    this->z3 = TausStep(this->z3, 3, 11, 17, 4294967280UL);
    // p4=2^32
    this->z4 = LCGStep(this->z4, 1664525, 1013904223UL);

    // Combined period is lcm(p1,p2,p3,p4)~ 2^121
    float res = 2.3283064365387e-10f * static_cast<float>(this->z1 ^ this->z2 ^ this->z3 ^ this->z4);

    // Combined period is lcm(p1,p2,p3,p4)~ 2^121
    return res;
}

uint32_t HybridTaus::TausStep(uint32_t z, int S1, int S2, int S3, uint32_t M)
{
	uint32_t b = (((z << S1) ^ z) >> S2);
	return (((z & M) << S3) ^ b);
}

uint32_t HybridTaus::LCGStep(uint32_t z, uint32_t A, uint32_t C)
{
	return (A * z + C);
}
