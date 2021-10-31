#pragma once

#include <memory>
#include <cstdint>

class ISoftRend
{
public:
    virtual ~ISoftRend() = default;

    virtual void Render(uint32_t width, uint32_t height, void* dstMemory) = 0;
};

std::unique_ptr<ISoftRend> CreateISoftRend();
