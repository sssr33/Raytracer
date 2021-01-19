#include "CameraSettings.h"

#define _USE_MATH_DEFINES
#include <math.h>

CameraViewSizeSettings::CameraViewSizeSettings(const CameraFovSettings& fovSettings)
	: CameraViewSizeSettings(
        static_cast<const CameraTransformSettings&>(fovSettings),
        CameraViewSizeSettings::ViewSizeFromArFov(fovSettings))
{}

CameraViewSizeSettings::CameraViewSizeSettings(
    const CameraTransformSettings& transform,
    const Helpers::Size2D<float>& viewSize)
    : CameraTransformSettings(transform)
    , viewWidth(viewSize.width)
    , viewHeight(viewSize.height)
{}

Helpers::Size2D<float> CameraViewSizeSettings::ViewSizeFromArFov(const CameraFovSettings& fovSettings)
{
    float theta = fovSettings.fov * static_cast<float>(M_PI) / 180.f;
    float halfViewHeight = std::tan(theta / 2.f);
    float halfViewWidth = fovSettings.aspectRatio * halfViewHeight;

    return { halfViewWidth * 2.f, halfViewHeight * 2.f };
}
