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
    , antialiasingOffsets(RayTraceFunctor::MakeAntialiasingOffsets(3))
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

            for (vec2<float> subPixOffset : antialiasingOffsets)
            {
                const vec2<float> uv = (pixCoord + subPixOffset) / imageSize;
                const ray<float> r = camera.GetRay(uv);
                color += this->Color(r, hitableList);
            }

            color /= static_cast<float>(std::size(antialiasingOffsets));

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
        vec3<float> color = 0.5f * (hitRec->normal + 1.f);
        return color;
    }
    else
    {
        vec3<float> unitDirection = ray.direction.normalized();
        float t = 0.5f * (unitDirection.y + 1.f);
        return (1.f - t) * vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
    }
}

std::vector<vec2<float>> RayTraceFunctor::MakeAntialiasingOffsets(size_t width)
{
    std::vector<vec2<float>> offsets;
    offsets.reserve(width * width);

    const float step = 1.f / static_cast<float>(width);

    for (size_t y = 0; y < width; y++)
    {
        const float v = 0.f + step * static_cast<float>(y);

        for (size_t x = 0; x < width; x++)
        {
            const float u = 0.f + step * static_cast<float>(x);

            offsets.push_back(vec2<float>(u, v));
        }
    }

    return offsets;
}
