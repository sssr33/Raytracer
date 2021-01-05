#pragma once
#include "IGameWindowHandler.h"
#include "Image/Image.h"
#include "Image/BGRA.h"
#include "Render/Functor/RayTraceFunctorParams.h"
#include "Random/IRandom.h"
#include "Render/Sampler/TextureSamplerWithOffset.h"

#include <queue>
#include <future>
#include <atomic>
#include <random>

class RayTraceWindowHandler : public IGameWindowHandler
{
public:
	RayTraceWindowHandler();
	~RayTraceWindowHandler();

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
	void TryStartRayTraceTask();
	void TryFinishRayTraceTask();
	float GetRandomFloat();

	static Image<BGRA<uint8_t>> RayTraceMain(
		RayTraceFunctorParams rayTraceParams,
		Image<BGRA<uint8_t>> resultImage,
		std::atomic<bool>& cancel
	);

	std::mt19937 random;

	Helpers::Size2D<uint32_t> currentSize;

	// Images from <renderQueue> are submitted to render task.
	std::queue<Image<BGRA<uint8_t>>> renderQueue;
	// Images from <renderQueue> popped to <currentlyPresentingImage>.
	// <currentlyPresentingImage> contains the image that is rendered on every repaint.
	// When new image from <renderQueue> arrives, <currentlyPresentingImage> pushed to <renderQueue>
	Image<BGRA<uint8_t>> currentlyPresentingImage;

	std::atomic<bool> rayTraceTaskCancel = false;
	std::future<Image<BGRA<uint8_t>>> rayTraceTask;

	//std::shared_ptr<IRandom<float>> perlinNoise;
	TextureSamplerWithOffset<float> perlinNoise;

	float cameraX = 0.f;
};
