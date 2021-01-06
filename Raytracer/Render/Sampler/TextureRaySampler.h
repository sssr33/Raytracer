#pragma once
#include "ITextureSampler.h"
#include "Math/ray.h"

#include <memory>

template<class PixelT>
class TextureRaySampler
{
public:
    TextureRaySampler(std::shared_ptr<ITextureSampler<PixelT>> texSampler)
        : texSampler(std::move(texSampler))
    {}

    PixelT Sample(const ray<float>& ray) const
    {
        vec2<float> uv = RayToUV(ray);
        return this->texSampler->Sample(uv);
    }

private:
    static vec2<float> RayToUV(const ray<float>& ray)
    {
        float u = ray.origin.x + ray.origin.z + ray.direction.y + ray.direction.z;// ray.direction.dot({ 0.f, 1.f, 1.f });
        float v = ray.origin.y + ray.origin.z + ray.direction.x + ray.direction.z;// ray.direction.dot({ 1.f, 0.f, 1.f });

        return { u, v };

        /*
        * ray-plane intersection
         ax + by + cz + d = 0

         a(x - px(t)) + b(y - py(t)) + c(z - pz(t)) + d = 0

         a(x - (sx + t * ex)) + b(y - (sy + t * ey)) + c(z - (sz + t * ez)) + d = 0

         a*x - a*sx + a*t * a*ex + b*y - b*sy + b*t * b*ey + c*z - c*sz + c*t * c*ez + d = 0

         sx = i
         ex = j

         sy = k
         ey = q

         sz = n
         ez = m

         a*x - a*i + a*t * a*j + b*y - b*k + b*t * b*q + c*z - c*n + c*t * c*m + d = 0


         a(x - (i + t * j)) + b(y - (k + t * q)) + c(z - (n + t * m)) + d = 0

         a(x + -i + -t * j) + b(y + -k + -t * q) + c(z + -n + -t * m) + d = 0

         a*x + a*-i + a*-t * j + b*y + b*-k + b*-t * q + c*z + c*-n + c*-t * m + d = 0


         a*-t * j + b*-t * q + c*-t * m + a*x + a*-i + b*y + b*-k + c*z + c*-n + d = 0

         -t*(a*j + b*q + c*m) + a*x + a*-i + b*y + b*-k + c*z + c*-n + d = 0

         /(a*j + b*q + c*m)
         -t + (a*x + a*-i + b*y + b*-k + c*z + c*-n + d) / (a*j + b*q + c*m) = 0


         (a*x + a*-i + b*y + b*-k + c*z + c*-n + d) / (a*j + b*q + c*m) = t

         (a(x - i) + b(y - k) + c(z - n) + d) / (a*j + b*q + c*m) = t

         (a(x - sx) + b(y - sy) + c(z - sz) + d) / (a*ex + b*ey + c*ez) = t

         t = (a(x - sx) + b(y - sy) + c(z - sz) + d) / (a*ex + b*ey + c*ez)

         for X, a not zero, d zero
         t = a(x - sx) / a*ex
         for a == 1
         t = (x - sx) / ex
         for x == 0
         t = -sx / vx
        */

        /*vec3<float> vAbs = { std::fabsf(ray.direction.x), std::fabsf(ray.direction.y), std::fabsf(ray.direction.z) };

        if (vAbs.x > vAbs.y && vAbs.x > vAbs.z)
        {
            float t = 0.1f / ray.direction.x;
            texCoords = (ray.origin + t * ray.direction).swizzle.y.z;
        }
        else if (vAbs.y > vAbs.z)
        {
            float t = 0.1f / ray.direction.y;
            texCoords = (ray.origin + t * ray.direction).swizzle.x.z;
        }
        else
        {
            float t = 0.1f / ray.direction.z;
            texCoords = (ray.origin + t * ray.direction).swizzle.x.y;
        }*/
    }

    std::shared_ptr<ITextureSampler<PixelT>> texSampler;
};
