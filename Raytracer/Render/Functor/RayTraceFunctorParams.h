#pragma once
#include "Random/IRandom.h"
#include "Base/ClonePtr.h"
#include "Render/Sampler/ITextureSampler.h"

#include <memory>

class RayTraceFunctorParams
{
public:
	float cameraX = 0.f;
	ClonePtr<std::shared_ptr<ITextureSampler<float>>> texSampler;

private:
};
