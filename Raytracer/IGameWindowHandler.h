#pragma once
#include "ISystemBufferSwapChain.h"

#include <cstdint>
#include <Helpers/Point2D.h>
#include <Helpers/Size2D.h>

class IGameWindowHandler
{
public:
	virtual ~IGameWindowHandler() = default;

	virtual void GameLoop(ISystemBufferSwapChain& swapChain) = 0;

	virtual void OnResize(const Helpers::Size2D<uint32_t>& newSize) = 0;
	virtual void OnRepaint(ISystemBufferSwapChain& swapChain) = 0;

	virtual void OnMouseLeftPress(const Helpers::Point2D<float>& pt) = 0;
	virtual void OnMouseLeftRelease(const Helpers::Point2D<float>& pt) = 0;

	virtual void OnMouseRightPress(const Helpers::Point2D<float>& pt) = 0;
	virtual void OnMouseRightRelease(const Helpers::Point2D<float>& pt) = 0;

	virtual void OnMouseMiddlePress(const Helpers::Point2D<float>& pt) = 0;
	virtual void OnMouseMiddleRelease(const Helpers::Point2D<float>& pt) = 0;

	virtual void OnMouseMove(const Helpers::Point2D<float>& pt) = 0;
	virtual void OnMouseWheel(float delta) = 0;
};
