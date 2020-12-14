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
	struct AntiAliasingOffset
	{
		vec2<float> offset;
		vec3<float> colorMask;
	};

	struct AntiAliasing
	{
		std::vector<AntiAliasingOffset> offset;
		vec3<float> colorNormalizeK;
	};

	vec3<float> Color(const ray<float>& r, const IHitable& world) const;
	static AntiAliasing MakeAntiAliasing(size_t width);
	static AntiAliasing MakeRGBAntiAliasing(size_t width);

	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
	const AntiAliasing antialiasing;
};
