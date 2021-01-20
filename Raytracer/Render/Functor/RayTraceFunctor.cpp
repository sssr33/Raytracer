#include "RayTraceFunctor.h"
#include "Math/vec.h"
#include "Render/AntiAliasing/PixelMsaa.h"
#include "Render/AntiAliasing/SubpixelMsaa.h"

#include <algorithm>

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	RayTraceFunctorParams params
)
	: image(image)
	, params(std::move(params))
    , pixelAA(std::make_shared<PixelMsaa>(10))
    //, pixelAA(std::make_shared<SubpixelMsaa>(10, 0.9f)) // more coverage for green for smoother gradients
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const vec2<float> imageSize(static_cast<float>(block.imageWidth), static_cast<float>(block.imageHeight));
    PixelSampler pixSampler(imageSize, *this->params.camera, *this->params.scene, *this);

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            constexpr float rgbMax = 255.99f;

            const vec2<float> pixCoord = vec2<float>(static_cast<float>(x), static_cast<float>(y));
            const vec3<float> color = this->pixelAA->Resolve(pixCoord, pixSampler);
            const vec3<float> gammaCorrectColor = vec3<float>(std::sqrtf(color.r), std::sqrtf(color.g), std::sqrtf(color.b));
            const vec3<float> col8Bit = gammaCorrectColor * rgbMax;
            BGRA<uint8_t> pixel = {};

            pixel.r = static_cast<uint8_t>(std::clamp(col8Bit.r, 0.f, rgbMax));
            pixel.g = static_cast<uint8_t>(std::clamp(col8Bit.g, 0.f, rgbMax));
            pixel.b = static_cast<uint8_t>(std::clamp(col8Bit.b, 0.f, rgbMax));
            pixel.a = 255;

            row[x] = pixel;
        }
    }
}

vec3<float> RayTraceFunctor::Color(const ray<float>& r, const IHitable& world, uint32_t depth) const
{
    if (!depth)
    {
        return 0.f;
    }

    if (std::optional<HitRecord> hitRec = world.Hit(r, 0.001f, std::numeric_limits<float>::max()))
    {
        if (!hitRec->material)
        {
            return 0.f;
        }

        if (std::optional<ScatterRecord> scatter = hitRec->material->Scatter(r, *hitRec))
        {
            return scatter->attenuation * this->Color(scatter->scattered, world, depth - 1);
        }
        else
        {
            return 0.f;
        }
    }
    else
    {
        vec3<float> unitDirection = r.direction.normalized();
        float t = 0.5f * (unitDirection.y + 1.f);
        return (1.f - t) * vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
    }
}

RayTraceFunctor::PixelSampler::PixelSampler(
    const vec2<float>& imageSize,
    const ICamera& camera,
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
