#pragma once
#include "Render/Hitable/IHitable.h"

#include <memory>

struct Book1Scene
{
	std::unique_ptr<IHitable> operator()();
};
