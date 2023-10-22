#pragma once
#include "RasterScanBox.h"

template<class Fn /*= void(uint32_t y, uint32_t startX, uint32_t endX)*/>
void RasterScanBox::Scan(Fn fn) const {
    for (uint32_t y = this->startY; y < this->endY; ++y) {
        fn(y, this->startX, this->endX);
    }
}
