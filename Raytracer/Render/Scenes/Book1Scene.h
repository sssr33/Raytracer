#pragma once
#include "Render/Hitable/IHitable.h"
#include "Render/Random/IRandomInUnitSphere.h"

#include <memory>

struct Book1Scene
{
	std::unique_ptr<IHitable> operator()(const std::shared_ptr<IRandomInUnitSphere>& randomInUnitSphere);
};
