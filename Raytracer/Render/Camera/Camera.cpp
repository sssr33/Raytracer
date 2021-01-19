#include "Camera.h"

Camera::Camera(const CameraFovSettings& fovSettings)
    : Camera(static_cast<CameraViewSizeSettings>(fovSettings))
{}

Camera::Camera(const CameraViewSizeSettings& viewSizeSettings)
    : origin(viewSizeSettings.lookFrom)
{
    vec3<float> vecForward = (viewSizeSettings.lookFrom - viewSizeSettings.lookAt).normalized();
    vec3<float> vecRight = viewSizeSettings.vecUp.cross(vecForward).normalized();
    vec3<float> vecUp = vecForward.cross(vecRight).normalized();

    // maybe origin can be removed as well as in Camera::GetRay
    this->lowerLeftCorner =
        this->origin
        - (viewSizeSettings.viewWidth * 0.5f) * vecRight
        - (viewSizeSettings.viewHeight * 0.5f) * vecUp
        - vecForward;

    this->horizontal = viewSizeSettings.viewWidth * vecRight;
    this->vertical = viewSizeSettings.viewHeight * vecUp;
}

ray<float> Camera::GetRay(const vec2<float>& uv) const
{
    ray<float> r(
        this->origin,
        this->lowerLeftCorner + uv.x * this->horizontal + uv.y * this->vertical - this->origin
    );

    return r;
}

const vec3<float>& Camera::GetOrigin() const
{
    return this->origin;
}
