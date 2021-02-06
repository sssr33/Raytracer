#pragma once
#include "Math/vec.h"

#include <memory>
#include <Helpers/Size2D.h>

struct CameraBaseSettings
{
    vec3<float> lookFrom = { 0.f, 0.f, 0.f };
    vec3<float> lookAt = { 0.f, 0.f, -1.f};
    vec3<float> vecUp = { 0.f, 1.f, 0.f };
    float aperture = 0.f;
    float focusDist = 1.f;
    float timeStart = 0.f;
    float timeEnd = 1.f;
};

struct CameraFovSettings : public CameraBaseSettings
{
    float fov = 90.f;
    float aspectRatio = 16.f / 9.f;
};

struct CameraViewSizeSettings : public CameraBaseSettings
{
    explicit CameraViewSizeSettings(const CameraFovSettings & fovSettings);

    float viewWidth = 2.f;
    float viewHeight = 2.f;

private:
    CameraViewSizeSettings(
        const CameraBaseSettings& base,
        const Helpers::Size2D<float>& viewSize);

    static Helpers::Size2D<float> ViewSizeFromArFov(const CameraFovSettings& fovSettings);
};
