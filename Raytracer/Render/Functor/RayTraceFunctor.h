#pragma once
#include "RayTraceFunctorParams.h"
#include "Image/Image.h"
#include "Image/BGRA.h"
#include "Math/ray.h"
#include "Render/Hitable/IHitable.h"

class RayTraceFunctor
{
public:
	RayTraceFunctor(
		ImageView<BGRA<uint8_t>>& image,
		const RayTraceFunctorParams& params
	);

	void operator()(const MassiveCompute::Block& block);

private:
	vec3<float> Color(const ray<float>& r, const IHitable& world) const;

	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
};
