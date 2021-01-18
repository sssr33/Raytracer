#include "Dielectric.h"

Dielectric::Dielectric(float refractionIndex)
	: refractionIndex(refractionIndex)
{}

std::optional<ScatterRecord> Dielectric::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
	vec3<float> outwardNormal;
	vec3<float> reflected = r.direction.reflect(hitRecord.normal);
	float niOverNt;

	ScatterRecord scattered;

	scattered.attenuation = vec3<float>(1.f, 1.f, 1.f);

	if (r.direction.dot(hitRecord.normal) > 0.f)
	{
		outwardNormal = -hitRecord.normal;
		niOverNt = this->refractionIndex;
	}
	else
	{
		outwardNormal = hitRecord.normal;
		niOverNt = 1.f / this->refractionIndex;
	}

	if (std::optional<vec3<float>> refracted = r.direction.refract(outwardNormal, niOverNt))
	{
		scattered.scattered = ray<float>(hitRecord.point, *refracted);
	}
	else
	{
		scattered.scattered = ray<float>(hitRecord.point, reflected);
		return std::nullopt;
	}

	return scattered;
}
