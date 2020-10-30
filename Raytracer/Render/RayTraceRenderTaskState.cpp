#include "RayTraceRenderTaskParams.h"

RayTraceRenderTaskParams::RayTraceRenderTaskParams(const Helpers::Size2D<uint32_t>& size)
	: size(size)
{}

const Helpers::Size2D<uint32_t>& RayTraceRenderTaskParams::GetSize() const
{
	return this->size;
}
