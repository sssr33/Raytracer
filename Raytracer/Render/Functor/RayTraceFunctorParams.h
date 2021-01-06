#pragma once
#include "Render/Sampler/TextureRaySampler.h"

#include <memory>

class RayTraceFunctorParams
{
public:
	float cameraX = 0.f;
	std::shared_ptr<TextureRaySampler<float>> rayNoiseSampler;

private:
};
