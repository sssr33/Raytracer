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

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            vec3<float> col =
            {
                static_cast<float>(x) / static_cast<float>(block.imageWidth),
                static_cast<float>(y) / static_cast<float>(block.imageHeight),
                0.2f
            };

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
