#include "RayTraceWindowHandler.h"
#include "Image/BGRA.h"
#include "Image/ImageView.h"
#include "Render/Functor/TestGradientFunctor.h"
#include "Render/Functor/CopyImageFunctor.h"
#include "Render/Functor/RayTraceFunctor.h"
#include "Render/PerlinNoise/PerlinNoiseRandom.h"
#include "Render/PerlinNoise/PerlinNoiseTextureSampler.h"

#include <Helpers/is_ready.h>
#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>
#include <MassiveCompute/Schedulers/ConstantBlockScheduler.h>
#include <MassiveCompute/Schedulers/EqualBlockScheduler.h>

RayTraceWindowHandler::RayTraceWindowHandler()
{
	// 1 image for raytracing task
	this->renderQueue.emplace();

	//this->perlinNoise = std::make_shared<PerlinNoiseRandom>();
	this->perlinNoise = std::make_shared<PerlinNoiseTextureSampler>(256);
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

	stealingScheduler(imageView, MakeCopyImageFunctor(imageView, this->currentlyPresentingImage.MakeView(), width, height), imageView.GetWidth(), 1);
}

void RayTraceWindowHandler::OnMouseLeftPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseLeftRelease(const Helpers::Point2D<float>& pt)
{
	cameraX -= 0.01f;
}

void RayTraceWindowHandler::OnMouseRightPress(const Helpers::Point2D<float>& pt)
{

}

void RayTraceWindowHandler::OnMouseRightRelease(const Helpers::Point2D<float>& pt)
{
	cameraX += 0.01f;
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
		image = Image<BGRA<uint8_t>>::Resize(std::move(image), this->currentSize.width, this->currentSize.height);
	}

	RayTraceFunctorParams rayTraceParams;

	rayTraceParams.cameraX = this->cameraX;
	rayTraceParams.texSampler = this->perlinNoise;

	this->rayTraceTask = std::async(
		std::launch::async,
		&RayTraceWindowHandler::RayTraceMain,
		std::move(rayTraceParams),
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

Image<BGRA<uint8_t>> RayTraceWindowHandler::RayTraceMain(
	RayTraceFunctorParams rayTraceParams,
	Image<BGRA<uint8_t>> resultImage,
	std::atomic<bool>& /*cancel*/
)
{
	ImageView<BGRA<uint8_t>> imageView(resultImage.GetWidth(), resultImage.GetHeight(), resultImage.GetData());
	MassiveCompute::StealingBlockScheduler stealingScheduler;

	MassiveCompute::ConstantBlockScheduler cbs;

	MassiveCompute::EqualBlockScheduler ebs;

	//ebs(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)));

	//cbs(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)), imageView.GetWidth(), 1);

	stealingScheduler(imageView, RayTraceFunctor(imageView, std::move(rayTraceParams)), imageView.GetWidth(), 1);

	// single thread, for test
	/*MassiveCompute::Block block;
	block.left = 0;
	block.top = 0;
	block.right = block.imageWidth = imageView.GetWidth();
	block.bottom = block.imageHeight = imageView.GetHeight();
	(TestGradientFunctor(imageView))(block);*/

	return resultImage;
}
