#include "RayTraceWindowHandler.h"
#include "Image/BGRA.h"
#include "Image/ImageView.h"

#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>

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

	BGRA<uint8_t>* pixels = reinterpret_cast<BGRA<uint8_t>*>(data.data);
	ImageView<BGRA<uint8_t>> imageView(data.size.width, data.size.height, pixels);

	for (size_t y = 0, height = imageView.GetHeight(); y < height; y++)
	{
		BGRA<uint8_t>* row = imageView.GetRow(y);

		for (size_t x = 0, width = imageView.GetWidth(); x < width; x++)
		{
			BGRA<uint8_t> pixel = {};

			float val = (float)y / (float)height;

			pixel.r = (uint8_t)(val * 255.f);
			pixel.a = 255;

			row[x] = pixel;
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
