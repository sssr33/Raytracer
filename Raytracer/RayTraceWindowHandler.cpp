#include "RayTraceWindowHandler.h"
#include "Image/BGRA.h"
#include "Image/ImageView.h"
#include "Render/Functor/TestGradientFunctor.h"

#include <Helpers/is_ready.h>
#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>

RayTraceWindowHandler::RayTraceWindowHandler()
{
	// 1 image for raytracing task
	this->renderQueue.emplace();
}

RayTraceWindowHandler::~RayTraceWindowHandler()
{
	if (this->rayTraceTask.valid())
	{
		this->rayTraceTaskCancel = true;
		this->rayTraceTask.wait();
	}
}

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
	this->TryStartRayTraceTask();
	this->TryFinishRayTraceTask();

	SystemBackBufferLock bufLock(backBuffer);
	ISystemBackBuffer::LockedData& data = bufLock.GetData();

	if (!data.data)
	{
		return;
	}

	BGRA<uint8_t>* pixels = reinterpret_cast<BGRA<uint8_t>*>(data.data);
	ImageView<BGRA<uint8_t>> imageView(data.size.width, data.size.height, pixels);

	const size_t width = std::min(this->currentlyPresentingImage.GetWidth(), imageView.GetWidth());
	const size_t height = std::min(this->currentlyPresentingImage.GetHeight(), imageView.GetHeight());

	MassiveCompute::StealingBlockScheduler stealingScheduler;

	/*stealingScheduler(imageView,
		[](const MassiveCompute::Block& block)
		{
			block.
		}, imageView.GetWidth(), 1);*/

	for (size_t y = 0; y < height; y++)
	{
		const BGRA<uint8_t>* srcRow = this->currentlyPresentingImage.GetRow(y);
		BGRA<uint8_t>* dstRow = imageView.GetRow(y);

		std::memcpy(dstRow, srcRow, width * sizeof(*srcRow));
	}

	/*BGRA<uint8_t>* pixels = reinterpret_cast<BGRA<uint8_t>*>(data.data);
	ImageView<BGRA<uint8_t>> imageView(data.size.width, data.size.height, pixels);
	MassiveCompute::StealingBlockScheduler stealingScheduler;

	stealingScheduler(imageView, TestGradientFunctor(imageView), imageView.GetWidth(), 1);*/

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

void RayTraceWindowHandler::TryStartRayTraceTask()
{
	if (this->rayTraceTask.valid() || this->renderQueue.empty() || this->currentSize.Empty())
	{
		return;
	}

	Image<BGRA<uint8_t>> image = std::move(this->renderQueue.front());
	this->renderQueue.pop();

	if (image.GetWidth() != this->currentSize.width || image.GetHeight() != this->currentSize.height)
	{
		image = Image<BGRA<uint8_t>>(this->currentSize.width, this->currentSize.height);
	}

	this->rayTraceTask = std::async(
		std::launch::async,
		&RayTraceWindowHandler::RayTraceMain,
		std::move(image),
		std::ref(this->rayTraceTaskCancel)
	);
}

void RayTraceWindowHandler::TryFinishRayTraceTask()
{
	if (!is_ready(this->rayTraceTask))
	{
		return;
	}

	this->renderQueue.push(std::move(this->currentlyPresentingImage));
	this->currentlyPresentingImage = this->rayTraceTask.get();
}

Image<BGRA<uint8_t>> RayTraceWindowHandler::RayTraceMain(Image<BGRA<uint8_t>> resultImage, std::atomic<bool>& cancel)
{
	ImageView<BGRA<uint8_t>> imageView(resultImage.GetWidth(), resultImage.GetHeight(), resultImage.GetData());
	MassiveCompute::StealingBlockScheduler stealingScheduler;

	stealingScheduler(imageView, TestGradientFunctor(imageView), imageView.GetWidth(), 1);

	return resultImage;
}
