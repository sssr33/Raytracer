#pragma once
#include "Image/Image.h"
#include "Image/BGRA.h"

#include <Helpers/Size2D.h>

class RayTraceRenderTaskState
{
public:
	RayTraceRenderTaskState(Image<BGRA<uint8_t>>&& resultImage);

	Image<BGRA<uint8_t>>& GetResultImage();

private:
	Image<BGRA<uint8_t>> resultImage;
};
