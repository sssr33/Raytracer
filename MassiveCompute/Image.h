#pragma once
#include "Block.h"

#include <vector>
#include <optional>

class Image
{
public:
    Image() = default;
    Image(size_t width, size_t height);

    bool operator==(const Image& other) const;
    bool operator!=(const Image& other) const;

    size_t GetWidth() const;
    size_t GetHeight() const;

    std::optional<Block> GetBlock(size_t left, size_t top, size_t maxWidth, size_t maxHeight) const;

private:
    size_t width = 0;
    size_t height = 0;
};