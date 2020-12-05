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
    const float aspectRatio = static_cast<float>(block.imageWidth) / static_cast<float>(block.imageHeight);
    const float normalizedHalfViewWidth = aspectRatio;
    const float normalizedHalfViewHeight = 1.f;

    const vec3<float> lowerLeftCorner = { -normalizedHalfViewWidth, -normalizedHalfViewHeight, -1.f };
    const vec3<float> horizontal = { normalizedHalfViewWidth * 2.f, 0.f, 0.f };
    const vec3<float> vertical = { 0.f, normalizedHalfViewHeight * 2.f, 0.f };
    const vec3<float> origin = { 0.f, 0.f, 0.f };

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            const float u = static_cast<float>(x) / static_cast<float>(block.imageWidth);
            const float v = static_cast<float>(y) / static_cast<float>(block.imageHeight);

            const ray<float> r(origin, lowerLeftCorner + u * horizontal + v * vertical);
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
