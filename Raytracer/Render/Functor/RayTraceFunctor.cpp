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

vec3<float> RayTraceFunctor::Color(const ray<float>& r) const
{
    vec3<float> unitDirection = r.direction.normalized();
    float t = 0.5f * (unitDirection.y + 1.f);
    return (1.f - t) * vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
}
