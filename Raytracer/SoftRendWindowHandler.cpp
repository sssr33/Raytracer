#include "SoftRendWindowHandler.h"

#include <SoftRend\render\Pipeline\Test\Pipeline2Test.h>

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
    return std::make_unique<SoftRendTask>(*this->softRend);
}

SoftRendWindowHandler::SoftRendTask::SoftRendTask(ISoftRend& render)
    : render(render)
{}

void SoftRendWindowHandler::SoftRendTask::Render(std::unique_ptr<IBackBufferSwapLock> backBufferLk, std::atomic<bool>& cancel)
{
    Pipeline2Test();

    auto backBufData = backBufferLk->GetData();

    if (backBufData.dataLineByteSize != backBufData.size.width * 4) {
        // if happens need to fix render to support dataLineByteSize != width * 4
        assert(false);
        return;
    }

    this->render.Render(
        backBufData.size.width,
        backBufData.size.height,
        backBufData.data);
}
