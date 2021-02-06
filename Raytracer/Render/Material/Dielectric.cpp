#include "Dielectric.h"
#include "Render/Random/RandomFunctions.h"

Dielectric::Dielectric(float refractionIndex)
	: refractionIndex(refractionIndex)
{}

std::optional<ScatterRecord> Dielectric::Scatter(const ray<float>& r, const HitRecord& hitRecord) const
{
	vec3<float> outwardNormal;
	vec3<float> reflected = r.direction.reflect(hitRecord.normal);
	float niOverNt;
	float cosine;

	ScatterRecord scattered;

	scattered.attenuation = vec3<float>(1.f, 1.f, 1.f);

	if (r.direction.dot(hitRecord.normal) > 0.f)
	{
		outwardNormal = -hitRecord.normal;
		niOverNt = this->refractionIndex;
		cosine = this->refractionIndex * r.direction.dot(hitRecord.normal) / r.direction.length();
	}
	else
	{
		outwardNormal = hitRecord.normal;
		niOverNt = 1.f / this->refractionIndex;
		cosine = -r.direction.dot(hitRecord.normal) / r.direction.length();
	}

	if (std::optional<vec3<float>> refracted = r.direction.refract(outwardNormal, niOverNt))
	{
		float reflectProbability = Dielectric::Schlick(cosine, this->refractionIndex);
		float rndVal = RandomFloat();

		if (rndVal <= reflectProbability)
		{
			scattered.scattered = ray<float>(hitRecord.point, reflected, r.time);
		}
		else
		{
			scattered.scattered = ray<float>(hitRecord.point, *refracted, r.time);
		}
	}
	else
	{
		scattered.scattered = ray<float>(hitRecord.point, reflected, r.time);
	}

	return scattered;
}

float Dielectric::Schlick(float cosine, float refractionIndex)
{
	float r0 = (1.f - refractionIndex) / (1.f + refractionIndex);
	r0 = r0 * r0;

	float res = r0 + (1.f - r0) * std::pow((1.f - cosine), 5.f);
	return res;
}
