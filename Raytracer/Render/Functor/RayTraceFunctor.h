#pragma once
#include "RayTraceFunctorParams.h"
#include "Image/Image.h"
#include "Image/BGRA.h"
#include "Math/ray.h"

class RayTraceFunctor
{
public:
	RayTraceFunctor(
		ImageView<BGRA<uint8_t>>& image,
		const RayTraceFunctorParams& params
	);

	void operator()(const MassiveCompute::Block& block);

private:
	struct SphereHit
	{
		bool hit = false;
		float t;
		vec3<float> normal;
	};

	struct TriangleHit
	{
		bool hit = false;
		float t;
		float u;
		float v;
		vec3<float> normal;
	};

	vec3<float> Color(const ray<float>& r) const;
	SphereHit HitSphere(const vec3<float>& center, float radius, const ray<float>& ray) const;
	TriangleHit HitTriangle(const vec3<float>& v0, const vec3<float>& v1, const vec3<float>& v2, const ray<float>& ray) const;

	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
};
