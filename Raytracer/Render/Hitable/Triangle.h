#pragma once
#include "IHitable.h"
#include "Random/IRandom.h"
#include "Render/Sampler/ITextureSampler.h"

class Triangle : public IHitable
{
public:
	Triangle(
		const vec3<float>& v0, const vec3<float>& v1, const vec3<float>& v2,
		const vec2<float>& t0, const vec2<float>& t1, const vec2<float>& t2,
		ITextureSampler<float>* texSampler
	);

	std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;

	vec3<float> v0;
	vec3<float> v1;
	vec3<float> v2;

	vec2<float> t0;
	vec2<float> t1;
	vec2<float> t2;

	ITextureSampler<float>* texSampler;
};
