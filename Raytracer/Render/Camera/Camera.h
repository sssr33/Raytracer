#pragma once
#include "ICamera.h"
#include "CameraSettings.h"
#include "Math/ray.h"

#include <Helpers/Size2D.h>

class Camera : public ICamera
{
public:
    Camera() = default;
    explicit Camera(const CameraFovSettings& fovSettings);
    explicit Camera(const CameraViewSizeSettings& viewSizeSettings);

    ray<float> GetRay(const vec2<float>& uv) const override;

    const vec3<float>& GetOrigin() const;

private:
    vec3<float> origin = { 0.f, 0.f, 0.f };
    vec3<float> lowerLeftCorner = { 0.f, 0.f, 0.f };
    vec3<float> horizontal = { 0.f, 0.f, 0.f };
    vec3<float> vertical = { 0.f, 0.f, 0.f };

    vec3<float> vecForward;
    vec3<float> vecRight;
    vec3<float> vecUp;

    float lensRadius = 0.f;
};
