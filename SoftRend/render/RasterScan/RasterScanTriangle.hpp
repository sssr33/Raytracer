#pragma once
#include "RasterScanTriangle.h"

#include <cmath>

template<class Fn /*= void(uint32_t y, uint32_t startX, uint32_t endX)*/>
void RasterScanTriangle::Scan(Fn fn) const {
    auto clampScreenX = [&](float v)
    {
        return (std::min)((std::max)(v, 0.f), static_cast<float>(this->params.videoMemoryWidth));
    };
    auto clampScreenY = [&](float v)
    {
        return (std::min)((std::max)(v, 0.f), static_cast<float>(this->params.videoMemoryHeight));
    };

    // preserve Y between scanRanges to not scan same Y lines more than once
    uint32_t iy = 0;

    for (const auto& i : this->scanRanges) {
        float minY = (std::min)(i.startY, i.endY);
        float maxY = (std::max)(i.startY, i.endY);

        if (this->expandStrategy == ExpandStrategy::PixelScanExpand) {
            minY = minY - 1.f;
            maxY = maxY + 1.f;
        }

        uint32_t istartY = static_cast<uint32_t>(clampScreenY(std::floor(minY)));
        uint32_t iendY = static_cast<uint32_t>(clampScreenY(std::floor(maxY)));

        iy = (std::max)(iy, istartY);
        iy = (std::min)(iy, iendY);

        // check Y range because some lines missing comparing to RasterScanBox
        for (; iy < iendY; ++iy) {
            float y = static_cast<float>(iy) + 0.5f;
            const float leftX = i.LerpXLeft(y);
            const float rightX = i.LerpXRight(y);

            auto minX = (std::min)(leftX, rightX);
            auto maxX = (std::max)(leftX, rightX);

            if (this->expandStrategy == ExpandStrategy::PixelScanExpand) {
                minX = minX - 1.f;
                maxX = maxX + 1.f;
            }

            // TODO check why +-1 needed to fix gaps when rendering
            // maybe HalfPlane or its points must be used
            const uint32_t ileftX = static_cast<uint32_t>(clampScreenX(std::floor(minX)));
            const uint32_t irightX = static_cast<uint32_t>(clampScreenX(std::ceil(maxX)));

            fn(iy, ileftX, irightX);
        }
    }
}
