#include "ImageBlockIndex.h"
#include "Helpers.h"

ImageBlockIndex::ImageBlockIndex(Image& img, size_t blockWidth, size_t blockHeight)
	: img(img)
	, blockWidth(blockWidth)
	, blockHeight(blockHeight)
	, widthBlockCount(Helpers::CeiledDiv(this->img.GetWidth(), this->blockWidth))
	, heightBlockCount(Helpers::CeiledDiv(this->img.GetHeight(), this->blockHeight))
	, blockCount(this->widthBlockCount * this->heightBlockCount)
{}

size_t ImageBlockIndex::GetBlockWidth() const
{
	return this->blockWidth;
}

size_t ImageBlockIndex::GetBlockHeight() const
{
	return this->blockHeight;
}

Image& ImageBlockIndex::GetImage()
{
	return this->img;
}

const Image& ImageBlockIndex::GetImage() const
{
	return this->img;
}

size_t ImageBlockIndex::GetWidthBlockCount() const
{
	return this->widthBlockCount;
}

size_t ImageBlockIndex::GetHeightBlockCount() const
{
	return this->heightBlockCount;
}

size_t ImageBlockIndex::GetBlockCount() const
{
	return this->blockCount;
}

std::optional<Block> ImageBlockIndex::GetBlock(size_t idx)
{
	size_t x = idx % this->GetWidthBlockCount();
	size_t y = idx / this->GetWidthBlockCount();

	return this->GetBlock(x, y);
}

std::optional<Block> ImageBlockIndex::GetBlock(size_t x, size_t y)
{
	size_t left = x * this->blockWidth;
	size_t top = y * this->blockHeight;

	return this->img.GetBlock(left, top, this->blockWidth, this->blockHeight);
}
