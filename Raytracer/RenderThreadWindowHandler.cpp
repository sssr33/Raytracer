#include "RenderThreadWindowHandler.h"
#include "Render/Functor/CopyImageFunctor.h"

#include <Helpers/is_ready.h>
#include <MassiveCompute/Schedulers/StealingBlockScheduler.h>

RenderThreadWindowHandler::RenderThreadWindowHandler()
{
    // 1 image for render task
    this->renderQueue.emplace();
}

RenderThreadWindowHandler::~RenderThreadWindowHandler()
{
	if (this->renderTask.valid())
	{
		this->renderTaskCancel = true;
		this->renderTask.wait();
	}
}

void RenderThreadWindowHandler::GameLoop(ISystemBackBuffer& backBuffer)
{
	this->OnRepaint(backBuffer);
}

void RenderThreadWindowHandler::OnResize(const Helpers::Size2D<uint32_t>& newSize)
{
	this->currentSize = newSize;
}

void RenderThreadWindowHandler::OnRepaint(ISystemBackBuffer& backBuffer)
{
	this->TryStartRenderTask();
	this->TryFinishRenderTask();

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

void RenderThreadWindowHandler::TryStartRenderTask()
{
	if (this->renderTask.valid() || this->renderQueue.empty() || this->currentSize.Empty())
	{
		return;
	}

	Image<BGRA<uint8_t>> image = std::move(this->renderQueue.front());
	this->renderQueue.pop();

	if (image.GetWidth() != this->currentSize.width || image.GetHeight() != this->currentSize.height)
	{
		image = Image<BGRA<uint8_t>>::Resize(std::move(image), this->currentSize.width, this->currentSize.height);
	}

	this->renderTask = std::async(
		std::launch::async,
		&RenderThreadWindowHandler::RenderMain,
		this->MakeRenderTask(this->currentSize),
		std::move(image),
		std::ref(this->renderTaskCancel)
	);
}

void RenderThreadWindowHandler::TryFinishRenderTask()
{
	if (!is_ready(this->renderTask))
	{
		return;
	}

	this->renderQueue.push(std::move(this->currentlyPresentingImage));
	this->currentlyPresentingImage = this->renderTask.get();
}

Image<BGRA<uint8_t>> RenderThreadWindowHandler::RenderMain(
	std::unique_ptr<IRenderThreadTask> renderTask,
	Image<BGRA<uint8_t>> resultImage,
	std::atomic<bool>& cancel
)
{
	return renderTask->Render(resultImage, cancel);
}
