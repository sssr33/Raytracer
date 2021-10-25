#include "SoftRendWindowHandler.h"

SoftRendWindowHandler::SoftRendWindowHandler()
    : softRend(CreateISoftRend())
{}

void SoftRendWindowHandler::OnMouseLeftPress(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseLeftRelease(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseRightPress(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseRightRelease(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseMiddlePress(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseMiddleRelease(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseMove(const Helpers::Point2D<float>& pt) {}

void SoftRendWindowHandler::OnMouseWheel(float delta) {}

std::unique_ptr<IRenderThreadTask> SoftRendWindowHandler::MakeRenderTask(const Helpers::Size2D<uint32_t>& currentImageSize)
{
    return std::make_unique<SoftRendTask>();
}

SoftRendWindowHandler::SoftRendTask::SoftRendTask()
{}

Image<BGRA<uint8_t>> SoftRendWindowHandler::SoftRendTask::Render(Image<BGRA<uint8_t>> resultImage, std::atomic<bool>& cancel)
{
    return resultImage;
}
