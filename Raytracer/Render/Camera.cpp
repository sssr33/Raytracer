#include "Camera.h"

const float Camera::DefaultViewHeight = 2.f;

Camera::Camera(float aspectRatio)
    : Camera(aspectRatio * Camera::DefaultViewHeight, Camera::DefaultViewHeight)
{}

Camera::Camera(float viewWidth, float viewHeight)
    : lowerLeftCorner(-(viewWidth * 0.5f), -(viewHeight * 0.5f), -1.f)
    , horizontal(viewWidth, 0.f, 0.f)
    , vertical(0.f, viewHeight, 0.f)
    , origin(0.f, 0.f, 0.f)
{}

ray<float> Camera::GetRay(const vec2<float>& uv) const
{
    const ray<float> r(
        this->origin,
        this->lowerLeftCorner + uv.x * this->horizontal + uv.y * this->vertical
    );

    return r;
}

const vec3<float>& Camera::GetOrigin() const
{
    return this->origin;
}

void Camera::SetOrigin(const vec3<float>& v)
{
    this->origin = v;
}
