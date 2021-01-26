#pragma once
#include "Math/vec.h"

/*
* thread safe random functions
*/

// returns random from 0.f to 1.f
float HybridTausFn();

vec2<float> RandomInDisk();
vec2<float> RandomInUnitDisk();
