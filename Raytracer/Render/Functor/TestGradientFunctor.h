#pragma once
#include "Image/BGRA.h"
#include "Image/ImageView.h"

#include <utility>
#include <MassiveCompute/Block.h>

class TestGradientFunctor
{
public:
    explicit TestGradientFunctor(ImageView<BGRA<uint8_t>>& image);

    void operator()(const MassiveCompute::Block& block);

private:
    std::reference_wrapper<ImageView<BGRA<uint8_t>>> image;
};
