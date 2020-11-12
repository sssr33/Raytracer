#include "RayTraceRenderTaskState.h"

RayTraceRenderTaskState::RayTraceRenderTaskState(Image<BGRA<uint8_t>>&& resultImage)
	: resultImage(std::move(resultImage))
{}

Image<BGRA<uint8_t>>& RayTraceRenderTaskState::GetResultImage()
{
	return this->resultImage;
}
