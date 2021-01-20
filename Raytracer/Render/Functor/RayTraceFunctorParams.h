#pragma once
#include "Render/Camera/ICamera.h"
#include "Render/Hitable/IHitable.h"

#include <memory>

class RayTraceFunctorParams
{
public:
	std::shared_ptr<ICamera> camera;
	std::shared_ptr<IHitable> scene;
};
