#include "RandomFunctions.h"
#include "HybridTaus.h"

#define _USE_MATH_DEFINES
#include <math.h>

float HybridTausFn()
{
	thread_local HybridTaus fn;
	return fn();
}

vec2<float> RandomInDisk()
{
    float u0 = HybridTausFn();
    float u1 = HybridTausFn();
    float r = std::sqrt(-2.f * std::log(u0)); // exceeds 0 ... 1 interval
    float theta = static_cast<float>(M_PI) * 2.f * u1;
    vec2<float> res = { r * std::sin(theta), r * std::cos(theta) };

    return res;
}

vec2<float> RandomInUnitDisk()
{
    float u0 = HybridTausFn();
    float u1 = HybridTausFn();
    float r = std::sqrt(u0); // inside 0 ... 1 interval
    float theta = static_cast<float>(M_PI) * 2.f * u1;
    vec2<float> res = { r * std::sin(theta), r * std::cos(theta) };

    return res;
}
