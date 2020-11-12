#pragma once
#include "RayTraceRenderTaskState.h"
#include "RayTraceRenderTaskParams.h"
#include "Image/Image.h"
#include "Image/BGRA.h"

class RayTraceRenderTask
{
public:
	RayTraceRenderTaskState operator()(
		const RayTraceRenderTaskParams& params,
		RayTraceRenderTaskState&& state);

private:
};
