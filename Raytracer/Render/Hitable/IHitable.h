#pragma once
#include "HitRecord.h"
#include "Render/Material/IMaterial.h"
#include "Math/aabb.h"

#include <optional>
#include <memory>

class IHitable
{
public:
	virtual ~IHitable() = default;

	virtual std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const = 0;
	virtual std::optional<aabb> GetBoundingBox(float time0, float time1) const = 0;
};
