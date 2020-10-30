#pragma once

#include <cassert>
#include <vector>
#include <Helpers/PropagateConst.h>
#include <MassiveCompute/Image.h>

template<class PixelT>
class Image : public MassiveCompute::Image
{
public:
    Image(size_t width, size_t height)
        : Image(width, height)
        , data(this->GetWidth() * this->GetHeight())
    {}

    bool operator==(const Image& other) const
    {
        bool equ = Image::operator==(other)
            && this->data == other.data;

        return equ;
    }

    bool operator!=(const Image& other) const
    {
        return !this->operator==(other);
    }

    const PixelT* GetData() const
    {
        return this->data.data();
    }

    PixelT* GetData()
    {
        return this->data.data();
    }

    const PixelT* GetRow(size_t y) const
    {
        return Image::GetRow(this, y);
    }

    PixelT* GetRow(size_t y)
    {
        return Image::GetRow(this, y);
    }

private:
    template<class ThisT>
    static PropagateConstT<ThisT, PixelT*> GetRow(ThisT&& _this, size_t y)
    {
        return _this->data.data() + y * _this->GetWidth();
    }

    std::vector<PixelT> data;
};
