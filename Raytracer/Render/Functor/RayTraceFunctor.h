#pragma once
#include "RayTraceFunctorParams.h"
#include "Image/Image.h"
#include "Image/BGRA.h"

class RayTraceFunctor
{
public:
	RayTraceFunctor(
		ImageView<BGRA<uint8_t>>& image,
		const RayTraceFunctorParams& params
	);

	void operator()(const MassiveCompute::Block& block);

private:
	ImageView<BGRA<uint8_t>>& image;
	const RayTraceFunctorParams& params;
};
