#include "RayTraceFunctor.h"
#include "Math/vec.h"
#include "Render/Camera.h"
#include "Render/Hitable/HitableList.h"
#include "Render/Hitable/Sphere.h"
#include "Render/Hitable/Triangle.h"

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	const RayTraceFunctorParams& params
)
	: image(image)
	, params(params)
    , antialiasing(RayTraceFunctor::MakeRGBAntiAliasing(20))
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const vec2<float> imageSize(static_cast<float>(block.imageWidth), static_cast<float>(block.imageHeight));
    Camera camera(imageSize.x / imageSize.y);

    camera.SetOrigin({ this->params.cameraX, 0.f, 0.f });

    HitableList hitableList;

    hitableList.objects.emplace_back(std::make_unique<Sphere>(vec3<float>{0.f, 0.f, -1.f}, 0.5f));
    hitableList.objects.emplace_back(std::make_unique<Sphere>(vec3<float>{0.f, -100.5f, -1.f}, 100.f));
    hitableList.objects.emplace_back(std::make_unique<Triangle>(vec3<float>{ -1.f, -0.5f, -1.f }, vec3<float>{ -0.5f, 0.f, -1.f }, vec3<float>{ -1.f, 0.5f, -1.f }));

    // 4x multisampling
    const vec2<float> anialiasOffsets[] =
    {
        vec2<float>(0.25f, 0.25f), vec2<float>(0.75f, 0.25f),
        vec2<float>(0.25f, 0.75f), vec2<float>(0.75f, 0.75f),
    };

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            vec3<float> color = { 0.f, 0.f, 0.f };
            const vec2<float> pixCoord = vec2<float>(static_cast<float>(x), static_cast<float>(y));

            for (const AntiAliasingOffset& aaOffset : this->antialiasing.offset)
            {
                const vec2<float> uv = (pixCoord + aaOffset.offset) / imageSize;
                const ray<float> r = camera.GetRay(uv);
                color += this->Color(r, hitableList) * aaOffset.colorMask;
            }

            color *= this->antialiasing.colorNormalizeK;

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
        vec3<float> color = 0.f;// 0.5f * (hitRec->normal + 1.f);
        return color;
    }
    else
    {
        vec3<float> unitDirection = ray.direction.normalized();
        float t = 0.5f * (unitDirection.y + 1.f);
        return 1.f;// (1.f - t)* vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
    }
}

RayTraceFunctor::AntiAliasing RayTraceFunctor::MakeAntiAliasing(size_t width)
{
    std::vector<AntiAliasingOffset> offsets;
    offsets.reserve(width * width);

    const float step = 1.f / static_cast<float>(width);

    for (size_t y = 0; y < width; y++)
    {
        const float v = 0.f + step * static_cast<float>(y);

        for (size_t x = 0; x < width; x++)
        {
            const float u = 0.f + step * static_cast<float>(x);

            AntiAliasingOffset offset;

            offset.offset = vec2<float>(u, v);
            offset.colorMask = vec3<float>(1.f, 1.f, 1.f);

            offsets.push_back(offset);
        }
    }

    AntiAliasing aa;

    aa.colorNormalizeK = 1.f / static_cast<float>(offsets.size());
    aa.offset = std::move(offsets);

    return aa;
}

// controlling red, green, blue coverage
// more coverage for green for smoother gradients
RayTraceFunctor::AntiAliasing RayTraceFunctor::MakeRGBAntiAliasing(size_t width)
{
    const float redCoverage = 0.05f;
    const float greenCoverage = 0.9f;
    const float blueCoverage = 0.05f;

    const float totalCoverage = redCoverage + greenCoverage + blueCoverage;
    const float redCoverageNorm = redCoverage / totalCoverage;
    const float greenCoverageNorm = greenCoverage / totalCoverage;
    const float blueCoverageNorm = blueCoverage / totalCoverage;

    const size_t redPixCount = static_cast<size_t>(std::ceilf(width * redCoverageNorm));
    const size_t greenPixCount = static_cast<size_t>(std::ceilf(width * greenCoverageNorm));
    const size_t bluePixCount = static_cast<size_t>(std::ceilf(width * blueCoverageNorm));

    const size_t height = width;
    width = redPixCount + greenPixCount + bluePixCount;

    std::vector<AntiAliasingOffset> offsets;
    offsets.reserve(width * height);

    const float stepX = 1.f / static_cast<float>(width);
    const float stepY = 1.f / static_cast<float>(height);

    for (size_t y = 0; y < height; y++)
    {
        const float v = stepY * static_cast<float>(y);

        for (size_t x = 0; x < redPixCount; x++)
        {
            const float u = stepX * static_cast<float>(x);

            AntiAliasingOffset offset;
            offset.offset = vec2<float>(u, v);
            offset.colorMask = vec3<float>(1.f, 0.f, 0.f);

            offsets.push_back(offset);
        }

        for (size_t x = redPixCount; x < redPixCount + greenPixCount; x++)
        {
            const float u = stepX * static_cast<float>(x);

            AntiAliasingOffset offset;
            offset.offset = vec2<float>(u, v);
            offset.colorMask = vec3<float>(0.f, 1.f, 0.f);

            offsets.push_back(offset);
        }

        for (size_t x = redPixCount + greenPixCount; x < width; x++)
        {
            const float u = stepX * static_cast<float>(x);

            AntiAliasingOffset offset;
            offset.offset = vec2<float>(u, v);
            offset.colorMask = vec3<float>(0.f, 0.f, 1.f);

            offsets.push_back(offset);
        }
    }

    AntiAliasing aa;

    const float totalRedPixCount = static_cast<float>(redPixCount * height);
    const float totalGreenPixCount = static_cast<float>(greenPixCount * height);
    const float totalBluePixCount = static_cast<float>(bluePixCount * height);

    aa.colorNormalizeK = 1.f / vec3<float>(totalRedPixCount, totalGreenPixCount, totalBluePixCount);
    aa.offset = std::move(offsets);

    return aa;
}
