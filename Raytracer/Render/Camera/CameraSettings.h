#pragma once
#include "Math/vec.h"

#include <Helpers/Size2D.h>

struct CameraTransformSettings
{
    vec3<float> lookFrom = { 0.f, 0.f, 0.f };
    vec3<float> lookAt = { 0.f, 0.f, -1.f};
    vec3<float> vecUp = { 0.f, 1.f, 0.f };
};

struct CameraFovSettings : public CameraTransformSettings
{
    float fov = 90.f;
    float aspectRatio = 16.f / 9.f;
};

struct CameraViewSizeSettings : public CameraTransformSettings
{
    explicit CameraViewSizeSettings(const CameraFovSettings & fovSettings);

    float viewWidth = 2.f;
    float viewHeight = 2.f;

private:
    CameraViewSizeSettings(
        const CameraTransformSettings& transform,
        const Helpers::Size2D<float>& viewSize);

    static Helpers::Size2D<float> ViewSizeFromArFov(const CameraFovSettings& fovSettings);
};
