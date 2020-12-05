#include "RayTraceFunctor.h"
#include "Math/vec.h"

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	const RayTraceFunctorParams& params
)
	: image(image)
	, params(params)
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const size_t height = image.GetHeight();

    vec3<float> lowerLeftCorner = { -2.f, -1.f, -1.f };
    vec3<float> horizontal = { 4.f, 0.f, 0.f };
    vec3<float> vertical = { 0.f, 2.f, 0.f };
    vec3<float> origin = { 0.f, 0.f, 0.f };

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            float u = static_cast<float>(x) / static_cast<float>(block.imageWidth);
            float v = static_cast<float>(y) / static_cast<float>(block.imageHeight);

            ray<float> r(origin, lowerLeftCorner + u * horizontal + v * vertical);
            vec3<float> col = this->Color(r);

            vec3<float> col8Bit = col * 255.99f;
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
