#include "RenderThreadWindowHandler.h"

#include <Helpers/is_ready.h>

RenderThreadWindowHandler::~RenderThreadWindowHandler()
{
	if (this->renderTask.valid())
	{
		this->renderTaskCancel = true;
		this->renderTask.wait();
	}
}

void RenderThreadWindowHandler::GameLoop(ISystemBufferSwapChain& swapChain)
{
	this->OnRepaint(swapChain);
}

void RenderThreadWindowHandler::OnResize(const Helpers::Size2D<uint32_t>& newSize)
{
	this->currentSize = newSize;
}

void RenderThreadWindowHandler::OnRepaint(ISystemBufferSwapChain& swapChain)
{
	this->TryStartRenderTask(swapChain);
	this->TryFinishRenderTask();

	swapChain.PresentFrontBuffer();
}

void RenderThreadWindowHandler::TryStartRenderTask(ISystemBufferSwapChain& swapChain)
{
	if (this->renderTask.valid() || this->currentSize.Empty())
	{
		return;
	}

	auto backBufferLk = swapChain.LockBackBufferSwapLock();

	this->renderTask = std::async(
		std::launch::async,
		&RenderThreadWindowHandler::RenderMain,
		this->MakeRenderTask(backBufferLk->GetData().size),
		std::move(backBufferLk),
		std::ref(this->renderTaskCancel)
	);
}

void RenderThreadWindowHandler::TryFinishRenderTask()
{
	if (!is_ready(this->renderTask))
	{
		return;
	}

	// finish task
	this->renderTask.get();
}

void RenderThreadWindowHandler::RenderMain(
	std::unique_ptr<IRenderThreadTask> renderTask,
	std::unique_ptr<IBackBufferSwapLock> backBufferLk,
	std::atomic<bool>& cancel
)
{
	return renderTask->Render(std::move(backBufferLk), cancel);
}
