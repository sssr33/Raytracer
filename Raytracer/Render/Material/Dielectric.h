#pragma once
#include "IMaterial.h"

class Dielectric : public IMaterial
{
public:
	Dielectric(float refractionIndex);

	std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
	float refractionIndex;
};
