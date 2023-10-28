#pragma once
#include "IGameWindowHandler.h"
#include "IRenderThreadTask.h"
#include "Image/Image.h"
#include "Image/BGRA.h"

#include <queue>
#include <future>
#include <atomic>
#include <memory>

class RenderThreadWindowHandler : public IGameWindowHandler
{
public:
	RenderThreadWindowHandler() = default;
	~RenderThreadWindowHandler();

	void GameLoop(ISystemBufferSwapChain& swapChain) override;

	void OnResize(const Helpers::Size2D<uint32_t>& newSize) override;
	void OnRepaint(ISystemBufferSwapChain& swapChain) override;

protected:
	virtual std::unique_ptr<IRenderThreadTask> MakeRenderTask(const Helpers::Size2D<uint32_t>& currentImageSize) = 0;

private:
	void TryStartRenderTask(ISystemBufferSwapChain& swapChain);
	void TryFinishRenderTask();

	static void RenderMain(
		std::unique_ptr<IRenderThreadTask> renderTask,
		std::unique_ptr<IBackBufferSwapLock> backBufferLk,
		std::atomic<bool>& cancel
	);

	Helpers::Size2D<uint32_t> currentSize;

	std::atomic<bool> renderTaskCancel = false;
	std::future<void> renderTask;
};
