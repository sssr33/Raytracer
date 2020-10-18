#include "TestGradientFunctor.h"

TestGradientFunctor::TestGradientFunctor(ImageView<BGRA<uint8_t>>& image)
    : image(image)
{}

void TestGradientFunctor::operator()(const MassiveCompute::Block& block)
{
    const size_t height = image.get().GetHeight();

    for (size_t y = block.top; y < block.bottom; y++)
    {
        BGRA<uint8_t>* row = image.get().GetRow(y);

        for (size_t x = block.left; x < block.right; x++)
        {
            BGRA<uint8_t> pixel = {};

            float val = (float)y / (float)height;

            pixel.r = (uint8_t)(val * 255.f);
            pixel.a = 255;

            row[x] = pixel;
        }
    }
}
