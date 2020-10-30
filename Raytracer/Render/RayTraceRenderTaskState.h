#pragma once
#include "Image/Image.h"
#include "Image/BGRA.h"

class RayTraceRenderTaskState
{
public:
	RayTraceRenderTaskState(Image<BGRA<uint8_t>> resultImage);

	const Helpers::Size2D<uint32_t>& GetSize() const;

private:
	Helpers::Size2D<uint32_t> size;
};
