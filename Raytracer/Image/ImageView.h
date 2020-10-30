#pragma once

#include <cassert>
#include <Helpers/PropagateConst.h>
#include <MassiveCompute/Image.h>

template<class PixelT>
class ImageView : public MassiveCompute::Image
{
public:
    ImageView(size_t width, size_t height, PixelT* data)
        : Image(width, height)
        , data(data)
    {}

    bool operator==(const ImageView& other) const
    {
        bool equ = Image::operator==(other)
            && this->CmpData(other);

        return equ;
    }

    bool operator!=(const ImageView& other) const
    {
        return !this->operator==(other);
    }

    const PixelT* GetData() const
    {
        return this->data;
    }

    PixelT* GetData()
    {
        return this->data;
    }

    const PixelT* GetRow(size_t y) const
    {
        return ImageView::GetRow(this, y);
    }

    PixelT* GetRow(size_t y)
    {
        return ImageView::GetRow(this, y);
    }

private:
    bool CmpData(const ImageView& other) const
    {
        assert(this->GetWidth() == other.GetWidth() && this->GetHeight() == other.GetHeight());

        for (size_t y = 0; y < this->GetHeight(); y++)
        {
            const PixelT* thisRow = this->GetRow(y);
            const PixelT* otherRow = other.GetRow(y);

            for (size_t x = 0; x < this->GetWidth(); x++)
            {
                if (thisRow[x] != otherRow[x])
                {
                    return false;
                }
            }
        }

        return true;
    }

    template<class ThisT>
    static PropagateConstT<ThisT, PixelT*> GetRow(ThisT&& _this, size_t y)
    {
        return _this->data + y * _this->GetWidth();
    }

    PixelT* data = nullptr;
};
