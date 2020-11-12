#include "RayTraceRenderTask.h"

RayTraceRenderTaskState RayTraceRenderTask::operator()(
	const RayTraceRenderTaskParams& params,
	RayTraceRenderTaskState&& state)
{
	return state;
}
