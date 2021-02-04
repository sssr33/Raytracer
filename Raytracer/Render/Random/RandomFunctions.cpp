#include "RandomFunctions.h"
#include "HybridTausFn.h"

#define _USE_MATH_DEFINES
#include <math.h>

float RandomFloat()
{
    thread_local HybridTausFn fn;
    return fn();
}

vec2<float> RandomInDisk(float radius)
{
    float u1 = RandomFloat();
    float theta = static_cast<float>(M_PI) * 2.f * u1;

    vec2<float> res =
    {
        radius * std::sin(theta),
        radius * std::cos(theta)
    };

    return res;
}

vec2<float> RandomInDisk()
{
    float u0 = RandomFloat();
    float radius = std::sqrt(-2.f * std::log(u0)); // exceeds 0 ... 1 interval

    return RandomInDisk(radius);
}

vec2<float> RandomInUnitDisk()
{
    float u0 = RandomFloat();
    float radius = std::sqrt(u0); // inside 0 ... 1 interval

    return RandomInDisk(radius);
}

vec3<float> RandomInSphere(float radius)
{
    float u1 = RandomFloat();
    float u2 = RandomFloat();

    float theta = u1 * static_cast<float>(M_PI);
    float phi = std::acos((2.f * u2) - 1.f);

    vec3<float> res =
    {
        radius * std::sin(phi) * std::cos(theta),
        radius * std::sin(phi) * std::sin(theta),
        radius * std::cos(phi)
    };

    return res;
}

vec3<float> RandomInSphere()
{
    float u0 = RandomFloat();
    float radius = std::pow(-2.f * std::log(u0), 1.f / 3.f); // exceeds 0 ... 1 interval

    return RandomInSphere(radius);
}

vec3<float> RandomInUnitSphere()
{
    float u0 = RandomFloat();
    float radius = std::pow(u0, 1.f / 3.f); // inside 0 ... 1 interval

    return RandomInSphere(radius);
}
