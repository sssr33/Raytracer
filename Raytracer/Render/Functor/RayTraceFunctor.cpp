#include "RayTraceFunctor.h"
#include "Math/vec.h"
#include "Render/Camera.h"
#include "Render/Hitable/HitableList.h"
#include "Render/Hitable/Sphere.h"
#include "Render/Hitable/Triangle.h"
#include "Render/AntiAliasing/PixelMsaa.h"
#include "Render/AntiAliasing/SubpixelMsaa.h"

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	RayTraceFunctorParams params
)
	: image(image)
	, params(std::move(params))
    , pixelAA(std::make_shared<PixelMsaa>(1))
    //, pixelAA(std::make_shared<SubpixelMsaa>(4, 0.9f)) // more coverage for green for smoother gradients
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const vec2<float> imageSize(static_cast<float>(block.imageWidth), static_cast<float>(block.imageHeight));
    Camera camera(imageSize.x / imageSize.y);

    camera.SetOrigin({ this->params.cameraX, 0.f, 0.f });

    HitableList hitableList;

    hitableList.objects.emplace_back(std::make_unique<Sphere>(vec3<float>{0.f, 0.f, -1.f}, 0.5f));
    hitableList.objects.emplace_back(std::make_unique<Sphere>(vec3<float>{0.f, -100.5f, -1.f}, 100.f));

    vec3<float> center = { 0.f, 0.f, -1.f };
    float width = 2.f;
    float height = 2.f;

    hitableList.objects.emplace_back(
        std::make_unique<Triangle>(
            center + vec3<float>(-width * 0.5f, -height * 0.5f, 0.f),
            center + vec3<float>(width * 0.5f, height * 0.5f, 0.f),
            center + vec3<float>(-width * 0.5f, height * 0.5f, 0.f),
            vec2<float>(0.f, 1.f),
            vec2<float>(1.f, 0.f),
            vec2<float>(0.f, 0.f),
            this->params.texSampler.get()
            )
    );

    hitableList.objects.emplace_back(
        std::make_unique<Triangle>(
            center + vec3<float>(-width * 0.5f, -height * 0.5f, 0.f),
            center + vec3<float>(width * 0.5f, -height * 0.5f, 0.f),
            center + vec3<float>(width * 0.5f, height * 0.5f, 0.f),
            vec2<float>(0.f, 1.f),
            vec2<float>(1.f, 1.f),
            vec2<float>(1.f, 0.f),
            this->params.texSampler.get()
            )
    );

    PixelSampler pixSampler(imageSize, camera, hitableList, *this);

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            const vec2<float> pixCoord = vec2<float>(static_cast<float>(x), static_cast<float>(y));
            const vec3<float> color = this->pixelAA->Resolve(pixCoord, pixSampler);
            const vec3<float> col8Bit = color * 255.99f;
            BGRA<uint8_t> pixel = {};

            pixel.r = static_cast<uint8_t>(col8Bit.r);
            pixel.g = static_cast<uint8_t>(col8Bit.g);
            pixel.b = static_cast<uint8_t>(col8Bit.b);
            pixel.a = 255;

            row[x] = pixel;
        }
    }
}

vec3<float> RayTraceFunctor::Color(const ray<float>& ray, const IHitable& world) const
{
    if (std::optional<HitRecord> hitRec = world.Hit(ray, 0.f, std::numeric_limits<float>::max()))
    {
        vec2<float> texCoords;

        float u = ray.direction.y + ray.direction.z;// ray.direction.dot({ 0.f, 1.f, 1.f });
        float v = ray.direction.x + ray.direction.z;// ray.direction.dot({ 1.f, 0.f, 1.f });

        /*
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


        float color = this->params.texSampler->Sample({u, v});

        color += 1.0;
        color /= 2.0;

        return color;

        /*if (hitRec->color)
        {
            return *hitRec->color;
        }

        vec3<float> color = 0.5f * (hitRec->normal + 1.f);
        return color;*/
    }
    else
    {
        vec3<float> unitDirection = ray.direction.normalized();
        float t = 0.5f * (unitDirection.y + 1.f);
        return (1.f - t)* vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
    }
}

RayTraceFunctor::PixelSampler::PixelSampler(
    const vec2<float>& imageSize,
    const Camera& camera,
    const IHitable& world,
    const RayTraceFunctor& parent)
    : imageSize(imageSize)
    , camera(camera)
    , world(world)
    , parent(parent)
{}

vec3<float> RayTraceFunctor::PixelSampler::Sample(const vec2<float>& pixCoords) const
{
    const vec2<float> uv = pixCoords / this->imageSize;
    const ray<float> ray = this->camera.GetRay(uv);

    return this->parent.Color(ray, this->world);
}
