#pragma once
#include "RenderThreadWindowHandler.h"
#include "Render/Functor/RayTraceFunctorParams.h"
#include "Render/Sampler/ITextureSampler.h"
#include "Render/Hitable/IHitable.h"

#include <random>

class RayTraceWindowHandler : public RenderThreadWindowHandler
{
public:
	RayTraceWindowHandler();

	void OnMouseLeftPress(const Helpers::Point2D<float>& pt) override;
	void OnMouseLeftRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseRightPress(const Helpers::Point2D<float>& pt) override;
	void OnMouseRightRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseMiddlePress(const Helpers::Point2D<float>& pt) override;
	void OnMouseMiddleRelease(const Helpers::Point2D<float>& pt) override;

	void OnMouseMove(const Helpers::Point2D<float>& pt) override;
	void OnMouseWheel(float delta) override;

protected:
	std::unique_ptr<IRenderThreadTask> MakeRenderTask(const Helpers::Size2D<uint32_t>& currentImageSize) override;

private:
	class RayTraceTask : public IRenderThreadTask
	{
	public:
		RayTraceTask(RayTraceFunctorParams params);

		void Render(std::unique_ptr<IBackBufferSwapLock> backBufferLk, std::atomic<bool>& cancel) override;

	private:
		RayTraceFunctorParams rayTraceParams;
	};

	float GetRandomFloat();

	RayTraceFunctorParams MakeDefaultScene(const Helpers::Size2D<uint32_t>& currentImageSize);
	RayTraceFunctorParams MakeBook1Scene(const Helpers::Size2D<uint32_t>& currentImageSize);

	std::mt19937 random;
	std::shared_ptr<ITextureSampler<float>> perlinNoise;
	float cameraX = 0.f;
	std::shared_ptr<IHitable> book1Scene;
};
