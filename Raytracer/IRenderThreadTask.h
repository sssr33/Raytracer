#pragma once

#include "Image/Image.h"
#include "Image/BGRA.h"

#include <atomic>

class IRenderThreadTask
{
public:
    virtual ~IRenderThreadTask() = default;

    virtual Image<BGRA<uint8_t>> Render(Image<BGRA<uint8_t>> resultImage, std::atomic<bool>& cancel) = 0;
};
