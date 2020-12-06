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
	vec3<float> Color(const ray<float>& r) const;
	bool HitSphere(const vec3<float>& center, float radius, const ray<float>& ray) const;

	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
};
