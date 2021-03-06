#pragma once
#include "Math/vec.h"

/*
* thread safe random functions
*/

// Returns random from 0.f to 1.f
float RandomFloat();

vec2<float> RandomInDisk(float radius);
vec2<float> RandomInDisk();
vec2<float> RandomInUnitDisk();

vec3<float> RandomInSphere(float radius);
vec3<float> RandomInSphere();
vec3<float> RandomInUnitSphere();
