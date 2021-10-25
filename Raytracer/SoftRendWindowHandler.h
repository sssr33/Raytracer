#pragma once
#include "RenderThreadWindowHandler.h"

#include <SoftRend/ISoftRend.h>

class SoftRendWindowHandler : public RenderThreadWindowHandler
{
public:
	SoftRendWindowHandler();

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
	class SoftRendTask : public IRenderThreadTask
	{
	public:
		SoftRendTask();

		Image<BGRA<uint8_t>> Render(Image<BGRA<uint8_t>> resultImage, std::atomic<bool>& cancel) override;
	};

	std::unique_ptr<ISoftRend> softRend;
};
