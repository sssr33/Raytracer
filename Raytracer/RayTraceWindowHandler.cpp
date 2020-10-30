#include "RayTraceWindowHandler.h"
#include "Image/BGRA.h"
#include "Image/ImageView.h"
#include "Render/Functor/TestGradientFunctor.h"

#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>

void RayTraceWindowHandler::GameLoop(ISystemBackBuffer& backBuffer)
{
	this->OnRepaint(backBuffer);
}

void RayTraceWindowHandler::OnResize(const Helpers::Size2D<uint32_t>& newSize)
{
	this->currentSize = newSize;
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
	MassiveCompute::StealingBlockScheduler stealingScheduler;

	stealingScheduler(imageView, TestGradientFunctor(imageView), imageView.GetWidth(), 1);

	// single thread, for test
	/*MassiveCompute::Block block;
	block.left = 0;
	block.top = 0;
	block.right = block.imageWidth = imageView.GetWidth();
	block.bottom = block.imageHeight = imageView.GetHeight();
	(TestGradientFunctor(imageView))(block);*/
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
