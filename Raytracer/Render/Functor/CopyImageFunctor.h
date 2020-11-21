#pragma once
#include "Image/ImageView.h"

#include <algorithm>

template<class PixelT>
class CopyImageFunctor
{
public:
    explicit CopyImageFunctor(ImageView<PixelT>& dst, const ImageView<PixelT>& src, size_t minWidth, size_t minHeight)
        : dst(dst)
		, src(src)
		, minWidth(minWidth)
		, minHeight(minHeight)
    {}

    void operator()(const MassiveCompute::Block& block)
    {
		const size_t left = (std::min)(block.left, this->minWidth);
		const size_t top = (std::min)(block.top, this->minHeight);

		const size_t right = (std::min)(block.right, this->minWidth);
		const size_t bottom = (std::min)(block.bottom, this->minHeight);

		const size_t width = right - left;

		for (size_t y = top; y < bottom; y++)
		{
			const PixelT* srcRow = this->src.GetRow(y);
			PixelT* dstRow = this->dst.GetRow(y);

			std::copy(srcRow, srcRow + width, dstRow);
		}
    }

private:
	ImageView<PixelT>& dst;
	const ImageView<PixelT>& src;
	size_t minWidth = 0;
	size_t minHeight = 0;
};

template<class PixelT>
CopyImageFunctor<PixelT> MakeCopyImageFunctor(ImageView<PixelT>& dst, const ImageView<PixelT>& src, size_t minWidth, size_t minHeight)
{
	return CopyImageFunctor<PixelT>(dst, src, minWidth, minHeight);
}
