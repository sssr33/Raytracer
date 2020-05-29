#include "Image.h"

#include <cassert>
#include <algorithm>

Image::Image(size_t width, size_t height)
    : width(width)
    , height(height)
    , data(this->width* this->height)
{}

bool Image::operator==(const Image& other) const
{
    bool equ = this->width == other.width
        && this->height == other.height
        && this->data == other.data;

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

const float* Image::GetData() const
{
    return this->data.data();
}

std::optional<Chunk> Image::GetChunk(size_t left, size_t top, size_t maxWidth, size_t maxHeight)
{
    auto chunk = this->GetChunkRect(left, top, maxWidth, maxHeight);
    if (!chunk)
    {
        return std::nullopt;
    }

    chunk->image = this->data.data();

    return chunk;
}

std::optional<Chunk> Image::GetChunkRect(size_t left, size_t top, size_t maxWidth, size_t maxHeight) const
{
    if (this->data.empty())
    {
        assert(!this->width && !this->height);
        return std::nullopt;
    }

    if (left >= this->width || top >= this->height)
    {
        return std::nullopt;
    }

    size_t right = (std::min)(left + maxWidth, this->width);
    size_t bottom = (std::min)(top + maxHeight, this->height);

    Chunk chunk;

    chunk.left = left;
    chunk.top = top;
    chunk.right = right;
    chunk.bottom = bottom;
    chunk.imageWidth = this->width;
    chunk.imageHeight = this->height;

    return chunk;
}