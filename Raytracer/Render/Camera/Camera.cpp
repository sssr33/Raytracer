#include "Camera.h"
#include "Render/Random/RandomFunctions.h"
#include "Math/Math.h"

Camera::Camera(const CameraFovSettings& fovSettings)
    : Camera(static_cast<CameraViewSizeSettings>(fovSettings))
{}

Camera::Camera(const CameraViewSizeSettings& viewSizeSettings)
    : origin(viewSizeSettings.lookFrom)
    , lensRadius(viewSizeSettings.aperture / 2.f)
    , vecForward((viewSizeSettings.lookFrom - viewSizeSettings.lookAt).normalized())
    , vecRight(viewSizeSettings.vecUp.cross(this->vecForward).normalized())
    , vecUp(this->vecForward.cross(this->vecRight).normalized())
    , timeStart(viewSizeSettings.timeStart)
    , timeEnd(viewSizeSettings.timeEnd)
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
    vec2<float> rnd = this->lensRadius * RandomInDisk();
    vec3<float> offset = rnd.x * this->vecRight + rnd.y * this->vecUp;
    float time = Math::lerp(this->timeStart, this->timeEnd, RandomFloat());

    ray<float> r(
        this->origin + offset,
        this->lowerLeftCorner + uv.x * this->horizontal + uv.y * this->vertical - this->origin - offset,
        time
    );

    return r;
}

const vec3<float>& Camera::GetOrigin() const
{
    return this->origin;
}
