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
