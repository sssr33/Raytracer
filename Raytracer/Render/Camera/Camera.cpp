#include "Camera.h"

#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
#include <mutex>

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

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float qerp(float a, float b, float c, float t)
{
    float ab = lerp(a, b, t);
    float bc = lerp(b, c, t);

    return lerp(ab, bc, t);
}

float cerp(float a, float b, float c, float d, float t)
{
    float abc = qerp(a, b, c, t);
    float bcd = qerp(b, c, d, t);

    return lerp(abc, bcd, t);
}

thread_local std::random_device rd;  //Will be used to obtain a seed for the random number engine
thread_local std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
thread_local std::uniform_real_distribution<float> dis(0.0f, 1.0f);
//std::exponential_distribution<float> dis(1.0f);
//std::normal_distribution<float> dis;

float RndFloat()
{
    /*float rnd = static_cast<float>(rand() % 10000) / 10000.f;
    return rnd;*/
    return dis(gen);
}

// https://developer.nvidia.com/gpugems/gpugems3/part-vi-gpu-computing/chapter-37-efficient-random-number-generation-and-application
unsigned TausStep(unsigned& z, int S1, int S2, int S3, unsigned M) { unsigned b = (((z << S1) ^ z) >> S2);   return z = (((z & M) << S3) ^ b); }
unsigned LCGStep(unsigned& z, unsigned A, unsigned C) { return z = (A * z + C); }

thread_local unsigned z1 = 0, z2 = 0, z3 = 0, z4 = 0;
float HybridTaus()
{
    // Combined period is lcm(p1,p2,p3,p4)~ 2^121
    return 2.3283064365387e-10 * (
        // Periods
        TausStep(z1, 13, 19, 12, 4294967294UL) ^
        // p1=2^31-1
        TausStep(z2, 2, 25, 4, 4294967288UL) ^
        // p2=2^30-1
        TausStep(z3, 3, 11, 17, 4294967280UL) ^
        // p3=2^28-1
        LCGStep(z4, 1664525, 1013904223UL)
        // p4=2^32
        );
}

vec2<float> Camera::RandomInUnitDisk(const vec2<float>& uv) const
{
    //float theta = (static_cast<float>(rand() % 10000) / 10000.f) * M_PI * 2.f;
    //float r = static_cast<float>(rand() % 10000) / 10000.f;
    //r = pow(r, 1.f / 4.f);

    //float r = sqrt(-2.f * log(RndFloat()));
    /*float theta = RndFloat() * M_PI * 2.f;
    float r = sqrt(RndFloat());
    float x = r * cos(theta);
    float y = r * sin(theta);

    return { x, y };*/

    float u0 = HybridTaus();
    float u1 = HybridTaus();
    float r = sqrt(-2.f * log(u0));
    float theta = M_PI * 2.f * u1;
    return { r * sin(theta), r * cos(theta) };

    /*vec2<float> rndInDisk;

    do
    {
        float x = static_cast<float>(rand() % 10000) / 10000.f;
        float y = static_cast<float>(rand() % 10000) / 10000.f;

        rndInDisk = 2.f * vec2<float>(x, y) - 1.f;
    } while (rndInDisk.dot(rndInDisk) >= 1.f);

    return rndInDisk;*/
}
