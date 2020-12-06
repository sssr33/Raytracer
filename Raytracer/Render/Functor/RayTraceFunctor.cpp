#include "RayTraceFunctor.h"
#include "Math/vec.h"
#include "Render/Camera.h"

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	const RayTraceFunctorParams& params
)
	: image(image)
	, params(params)
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const vec2<float> imageSize(static_cast<float>(block.imageWidth), static_cast<float>(block.imageHeight));
    Camera camera(imageSize.x / imageSize.y);

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            const vec2<float> uv = vec2<float>(static_cast<float>(x), static_cast<float>(y)) / imageSize;

            const ray<float> r = camera.GetRay(uv);
            const vec3<float> col = this->Color(r);

            const vec3<float> col8Bit = col * 255.99f;
            BGRA<uint8_t> pixel = {};

            pixel.r = static_cast<uint8_t>(col8Bit.r);
            pixel.g = static_cast<uint8_t>(col8Bit.g);
            pixel.b = static_cast<uint8_t>(col8Bit.b);
            pixel.a = 255;

            row[x] = pixel;
        }
    }
}

vec3<float> RayTraceFunctor::Color(const ray<float>& ray) const
{
    if (this->HitSphere({ 0.f, 0.f, -1.f }, 0.5f, ray))
    {
        return { 1.f, 0.f, 0.f };
    }

    vec3<float> unitDirection = ray.direction.normalized();
    float t = 0.5f * (unitDirection.y + 1.f);
    return (1.f - t) * vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
}

bool RayTraceFunctor::HitSphere(const vec3<float>& center, float radius, const ray<float>& ray) const
{
    vec3<float> fromSphereToRayOrigin = ray.origin - center;

    float a = ray.direction.dot(ray.direction);
    float b = 2.f * ray.direction.dot(fromSphereToRayOrigin);
    float c = fromSphereToRayOrigin.dot(fromSphereToRayOrigin) - radius * radius;

    float discriminant = b * b - 4.f * a * c;

    bool hits = false;

    if (discriminant > 0.f)
    {
        float t1 = (-b + std::sqrt(discriminant)) / 2.f * a;
        float t2 = (-b - std::sqrt(discriminant)) / 2.f * a;

        hits = t1 > 0.f || t2 > 0.f;
    }

    return hits;
}
