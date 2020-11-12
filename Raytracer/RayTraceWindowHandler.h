#pragma once
#include "IGameWindowHandler.h"
#include "Image/Image.h"
#include "Image/BGRA.h"

#include <queue>

class RayTraceWindowHandler : public IGameWindowHandler
{
public:
	void GameLoop(ISystemBackBuffer& backBuffer) override;

	void OnResize(const Helpers::Size2D<uint32_t>& newSize) override;
	void OnRepaint(ISystemBackBuffer& backBuffer) override;

	void OnMouseLeftPress(const Helpers::Point2D<float>& pt) override;
	void OnMouseLeftRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseRightPress(const Helpers::Point2D<float>& pt) override;
	void OnMouseRightRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseMiddlePress(const Helpers::Point2D<float>& pt) override;
	void OnMouseMiddleRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseMove(const Helpers::Point2D<float>& pt) override;
	void OnMouseWheel(float delta) override;

private:
	Helpers::Size2D<uint32_t> currentSize;

	// Images from <renderQueue> are submitted to render task.
	std::queue<Image<BGRA<uint8_t>>> renderQueue;
	// Images resulting from render task are pused to <presentQueue>
	std::queue<Image<BGRA<uint8_t>>> presentQueue;
	// Images from <presentQueue> popped to <currentlyPresentingImage>.
	// <currentlyPresentingImage> contains the image that is rendered on every repaint.
	// When new image from <presentQueue> arrives, <currentlyPresentingImage> pushed to <renderQueue>
	Image<BGRA<uint8_t>> currentlyPresentingImage;
};
