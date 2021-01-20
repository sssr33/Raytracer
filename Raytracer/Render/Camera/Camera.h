#pragma once
#include "CameraSettings.h"
#include "Math/ray.h"

#include <Helpers/Size2D.h>

class Camera
{
public:
    Camera() = default;
    explicit Camera(const CameraFovSettings& fovSettings);
    explicit Camera(const CameraViewSizeSettings& viewSizeSettings);

    ray<float> GetRay(const vec2<float>& uv) const;

    const vec3<float>& GetOrigin() const;

private:
    vec2<float> RandomInUnitDisk(const vec2<float>& uv) const;

    vec3<float> origin = { 0.f, 0.f, 0.f };
    vec3<float> lowerLeftCorner = { 0.f, 0.f, 0.f };
    vec3<float> horizontal = { 0.f, 0.f, 0.f };
    vec3<float> vertical = { 0.f, 0.f, 0.f };

    vec3<float> vecForward;
    vec3<float> vecRight;
    vec3<float> vecUp;

    float lensRadius = 0.f;
    std::shared_ptr<IRandomInUnitSphere> randomInUnitSphere;
};
