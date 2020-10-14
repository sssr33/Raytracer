#include "RayTraceWindowHandler.h"

void RayTraceWindowHandler::GameLoop(ISystemBackBuffer& backBuffer)
{
	this->OnRepaint(backBuffer);
}

void RayTraceWindowHandler::OnResize(const Helpers::Size2D<uint32_t>& newSize)
{

}

void RayTraceWindowHandler::OnRepaint(ISystemBackBuffer& backBuffer)
{
	SystemBackBufferLock bufLock(backBuffer);
	ISystemBackBuffer::LockedData& data = bufLock.GetData();

	if (!data.data)
	{
		return;
	}

	struct RGBA
	{
		uint8_t b, g, r, a;
	};

	RGBA* pixels = reinterpret_cast<RGBA*>(data.data);

	for (uint32_t y = 0; y < data.size.height; y++)
	{
		for (uint32_t x = 0; x < data.size.width; x++)
		{
			RGBA pixel = {};

			float val = (float)y / (float)data.size.height;

			pixel.r = (uint8_t)(val * 255.f);
			pixel.a = 255;

			pixels[x + y * data.size.width] = pixel;
		}
	}
}

void RayTraceWindowHandler::OnMouseLeftPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseLeftRelease(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseRightPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseRightRelease(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseMiddlePress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseMiddleRelease(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseMove(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseWheel(float delta)
{

}
