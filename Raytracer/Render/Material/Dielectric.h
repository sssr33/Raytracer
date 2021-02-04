#pragma once
#include "IMaterial.h"

#include <memory>

class Dielectric : public IMaterial
{
public:
	Dielectric(float refractionIndex);

	std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
	static float Schlick(float cosine, float refractionIndex);

	float refractionIndex;
};
