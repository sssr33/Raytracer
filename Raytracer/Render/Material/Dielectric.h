#pragma once
#include "IMaterial.h"
#include "Render/Random/IRandomInUnitSphere.h"

#include <memory>

class Dielectric : public IMaterial
{
public:
	Dielectric(float refractionIndex, std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere);

	std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const override;

private:
	static float Schlick(float cosine, float refractionIndex);

	float refractionIndex;
	std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere;
};
