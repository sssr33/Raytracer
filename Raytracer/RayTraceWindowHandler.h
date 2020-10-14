#pragma once
#include "IGameWindowHandler.h"

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
};
