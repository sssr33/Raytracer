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
	RenderThreadWindowHandler();
	~RenderThreadWindowHandler();

	void GameLoop(ISystemBackBuffer& backBuffer) override;

	void OnResize(const Helpers::Size2D<uint32_t>& newSize) override;
	void OnRepaint(ISystemBackBuffer& backBuffer) override;

protected:
	virtual std::unique_ptr<IRenderThreadTask> MakeRenderTask(const Helpers::Size2D<uint32_t>& currentImageSize) = 0;

private:
	void TryStartRenderTask();
	void TryFinishRenderTask();

	static Image<BGRA<uint8_t>> RenderMain(
		std::unique_ptr<IRenderThreadTask> renderTask,
		Image<BGRA<uint8_t>> resultImage,
		std::atomic<bool>& cancel
	);

	Helpers::Size2D<uint32_t> currentSize;

	// Images from <renderQueue> are submitted to render task.
	std::queue<Image<BGRA<uint8_t>>> renderQueue;
	// Images from <renderQueue> popped to <currentlyPresentingImage>.
	// <currentlyPresentingImage> contains the image that is rendered on every repaint.
	// When new image from <renderQueue> arrives, <currentlyPresentingImage> pushed to <renderQueue>
	Image<BGRA<uint8_t>> currentlyPresentingImage;

	std::atomic<bool> renderTaskCancel = false;
	std::future<Image<BGRA<uint8_t>>> renderTask;
};
