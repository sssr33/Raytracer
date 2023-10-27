#pragma once
#include "RasterScanTriangle.h"

#include <cmath>

template<class Fn /*= void(uint32_t y, uint32_t startX, uint32_t endX)*/>
void RasterScanTriangle::Scan(Fn fn) const {
    for (const auto& i : this->scanRanges) {
        uint32_t istartY = static_cast<uint32_t>(std::floor(i.startY));
        uint32_t iendY = static_cast<uint32_t>(std::floor(i.endY));
        float y = i.startY;

        // check Y range because some lines missing comparing to RasterScanBox
        for (uint32_t iy = istartY; iy < iendY; ++iy, ++y) {
            const float leftX = i.LerpXLeft(y);
            const float rightX = i.LerpXRight(y);

            // floor for left and ceil for right gets max length on X axis
            // TODO check if LeftTop rasterization rule can be applies to left or/and right
            /*const uint32_t ileftX = static_cast<uint32_t>(std::floor(leftX));
            const uint32_t irightX = static_cast<uint32_t>(std::ceil(rightX));*/

            auto clampScreen = [&](float v)
            {
                return (std::min)((std::max)(v, 0.f), static_cast<float>(this->params.videoMemoryWidth));
            };

            // TODO check why +-1 needed to fix gaps when rendering
            // maybe HalfPlane or its points must be used
            const uint32_t ileftX = static_cast<uint32_t>(clampScreen(std::floor(leftX) - 1.f));
            const uint32_t irightX = static_cast<uint32_t>(clampScreen(std::ceil(rightX) + 1.f));

            fn(iy, ileftX, irightX);
        }
    }
}
