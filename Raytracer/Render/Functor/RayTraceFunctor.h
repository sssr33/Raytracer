#pragma once
#include "RayTraceFunctorParams.h"
#include "Image/Image.h"
#include "Image/BGRA.h"
#include "Math/ray.h"
#include "Render/Camera.h"
#include "Render/Hitable/IHitable.h"
#include "Render/Sampler/IPixelSampler.h"
#include "Render/AntiAliasing/IPixelAntiAliasing.h"

#include <memory>

class RayTraceFunctor
{
public:
	RayTraceFunctor(
		ImageView<BGRA<uint8_t>>& image,
		const RayTraceFunctorParams& params
	);

	void operator()(const MassiveCompute::Block& block);

private:
	class PixelSampler : public IPixelSampler
	{
	public:
		PixelSampler(
			const vec2<float>& imageSize,
			const Camera& camera,
			const IHitable& world,
			const RayTraceFunctor& parent);

		vec3<float> Sample(const vec2<float>& pixCoords) const override;

	private:
		const vec2<float>& imageSize;
		const Camera& camera;
		const IHitable& world;
		const RayTraceFunctor& parent;
	};

	vec3<float> Color(const ray<float>& r, const IHitable& world) const;

	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
	std::shared_ptr<IPixelAntiAliasing> pixelAA;
};
