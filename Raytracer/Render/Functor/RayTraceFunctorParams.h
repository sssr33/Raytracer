#pragma once
#include "Render/Sampler/ITextureSampler.h"

#include <memory>

class RayTraceFunctorParams
{
public:
	float cameraX = 0.f;
	std::shared_ptr<ITextureSampler<float>> texSampler;

private:
};
