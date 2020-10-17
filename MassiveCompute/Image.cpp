#include "Image.h"

#include <cassert>
#include <algorithm>

namespace MassiveCompute
{
    Image::Image(size_t width, size_t height)
        : width(width)
        , height(height)
    {}

    bool Image::operator==(const Image& other) const
    {
        bool equ = this->width == other.width
            && this->height == other.height;

        return equ;
    }

    bool Image::operator!=(const Image& other) const
    {
        return !this->operator==(other);
    }

    size_t Image::GetWidth() const
    {
        return this->width;
    }

    size_t Image::GetHeight() const
    {
        return this->height;
    }

    bool Image::Empty() const
    {
        bool empty = this->width == 0 || this->height == 0;
        return empty;
    }

    std::optional<Block> Image::GetBlock(size_t left, size_t top, size_t maxWidth, size_t maxHeight) const
    {
        if (!this->width || !this->height)
        {
            return std::nullopt;
        }

        if (left >= this->width || top >= this->height)
        {
            return std::nullopt;
        }

        size_t right = (std::min)(left + maxWidth, this->width);
        size_t bottom = (std::min)(top + maxHeight, this->height);

        Block block;

        block.left = left;
        block.top = top;
        block.right = right;
        block.bottom = bottom;
        block.imageWidth = this->width;
        block.imageHeight = this->height;

        return block;
    }
}
