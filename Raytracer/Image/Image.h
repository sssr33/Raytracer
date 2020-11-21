#pragma once
#include "ImageView.h"

#include <cassert>
#include <vector>
#include <Helpers/PropagateConst.h>
#include <MassiveCompute/Image.h>

template<class PixelT>
class Image : public MassiveCompute::Image
{
public:
    Image() = default;
    Image(size_t width, size_t height)
        : MassiveCompute::Image(width, height)
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

    static Image Resize(Image&& other, size_t width, size_t height)
    {
        // use partial constructor
        Image image(MassiveCompute::Image(width, height));

        image.data = std::move(other.data);
        image.data.resize(width * height);

        other = Image();

        return image;
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

    ImageView<PixelT> MakeView()
    {
        return ImageView<PixelT>(this->GetWidth(), this->GetHeight(), this->GetData());
    }

private:
    // partial constructor
    explicit Image(const MassiveCompute::Image& other)
        : MassiveCompute::Image(other.GetWidth(), other.GetHeight())
    {}

    template<class ThisT>
    static PropagateConstT<ThisT, PixelT*> GetRow(ThisT&& _this, size_t y)
    {
        return _this->data.data() + y * _this->GetWidth();
    }

    std::vector<PixelT> data;
};
