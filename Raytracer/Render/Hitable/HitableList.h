#pragma once
#include "IHitable.h"

#include <vector>
#include <memory>

class HitableList : public IHitable
{
public:
	HitableList() = default;
	explicit HitableList(std::vector<std::unique_ptr<IHitable>> objects);

	std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;
	std::optional<aabb> GetBoundingBox(float time0, float time1) const override;

	std::vector<std::unique_ptr<IHitable>> objects;
};
