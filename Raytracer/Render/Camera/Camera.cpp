#include "Camera.h"

Camera::Camera(const CameraFovSettings& fovSettings)
    : Camera(static_cast<CameraViewSizeSettings>(fovSettings))
{}

Camera::Camera(const CameraViewSizeSettings& viewSizeSettings)
    : origin(viewSizeSettings.lookFrom)
    , lensRadius(viewSizeSettings.aperture / 2.f)
    , randomInUnitSphere(viewSizeSettings.randomInUnitSphere)
    , vecForward((viewSizeSettings.lookFrom - viewSizeSettings.lookAt).normalized())
    , vecRight(viewSizeSettings.vecUp.cross(this->vecForward).normalized())
    , vecUp(this->vecForward.cross(this->vecRight).normalized())
{
    // maybe origin can be removed as well as in Camera::GetRay
    this->lowerLeftCorner =
        this->origin
        - (viewSizeSettings.viewWidth * 0.5f) * this->vecRight * viewSizeSettings.focusDist
        - (viewSizeSettings.viewHeight * 0.5f) * this->vecUp * viewSizeSettings.focusDist
        - this->vecForward * viewSizeSettings.focusDist;

    this->horizontal = viewSizeSettings.viewWidth * this->vecRight * viewSizeSettings.focusDist;
    this->vertical = viewSizeSettings.viewHeight * this->vecUp * viewSizeSettings.focusDist;
}

ray<float> Camera::GetRay(const vec2<float>& uv) const
{
    vec2<float> rnd = this->lensRadius * this->RandomInUnitDisk(uv);
    vec3<float> offset = rnd.x * this->vecRight + rnd.y * this->vecUp;

    ray<float> r(
        this->origin + offset,
        this->lowerLeftCorner + uv.x * this->horizontal + uv.y * this->vertical - this->origin - offset
    );

    return r;
}

const vec3<float>& Camera::GetOrigin() const
{
    return this->origin;
}

vec2<float> Camera::RandomInUnitDisk(const vec2<float>& uv) const
{
    vec2<float> rndInDisk;

    do
    {
        float x = std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);
        float y = std::fmod(static_cast<float>(rand() % 10000) / 10000.f, 1.f);

        rndInDisk = 2.f * vec2<float>(x, y) - 1.f;
    } while (rndInDisk.dot(rndInDisk) >= 1.f);

    return rndInDisk;
}