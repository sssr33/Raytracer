#pragma once
#include "RayTraceRenderTaskState.h"
#include "Image/Image.h"
#include "Image/BGRA.h"

class RayTraceRenderTask
{
public:
	void operator()(const RayTraceRenderTaskState& params);

private:
	Image<BGRA<uint8_t>> image;

	/*class State
	{
	public:
		State(const Helpers::Size2D<uint32_t>& size);

	private:
		Image<BGRA<uint8_t>> image;
	};*/
};
