#pragma once
#include "Render/Random/IRandomInUnitSphere.h"
#include "Render/Sampler/TextureRaySampler.h"

#include <memory>

class RayTraceFunctorParams
{
public:
	float cameraX = 0.f;
	std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere;

private:
};
