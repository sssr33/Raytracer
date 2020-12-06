#pragma once
#include "Math/ray.h"

class Camera
{
public:
    static const float DefaultViewHeight;

    explicit Camera(float aspectRatio);
    Camera(float viewWidth, float viewHeight);

    ray<float> GetRay(const vec2<float>& uv) const;

    const vec3<float>& GetOrigin() const;
    void SetOrigin(const vec3<float>& v);

private:
    const vec3<float> lowerLeftCorner;
    const vec3<float> horizontal;
    const vec3<float> vertical;
    vec3<float> origin;
};
