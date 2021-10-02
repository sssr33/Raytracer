#include "HitableList.h"

HitableList::HitableList(std::vector<std::unique_ptr<IHitable>> objects)
	: objects(std::move(objects))
{}

std::optional<HitRecord> HitableList::Hit(const ray<float>& ray, float tMin, float tMax) const
{
	std::optional<HitRecord> hitRec;
	float closestSoFarT = tMax;

	for (const std::unique_ptr<IHitable>& obj : this->objects)
	{
		if (std::optional<HitRecord> objHitRec = obj->Hit(ray, tMin, closestSoFarT))
		{
			hitRec = objHitRec;
			closestSoFarT = hitRec->rayT;
		}
	}

	return hitRec;
}

std::optional<aabb> HitableList::GetBoundingBox(float time0, float time1) const
{
	if (this->objects.empty())
	{
		return std::nullopt;
	}

	aabb res(vec3<float>(0.f), vec3<float>(0.f));

	for (const auto& obj : this->objects)
	{
		std::optional<aabb> box = obj->GetBoundingBox(time0, time1);
		if (!box)
		{
			// one object doesn't support AABB, so AABB can't be computed for entire list to be sure that this object inside united AABB
			return std::nullopt;
		}

		res = res.United(*box);
	}

	return res;
}
