#pragma once
#include "Image.h"

class ImageBlockIndex
{
public:
	ImageBlockIndex(Image& img, size_t blockWidth, size_t blockHeight);

	size_t GetBlockWidth() const;
	size_t GetBlockHeight() const;
	Image& GetImage();
	const Image& GetImage() const;

	size_t GetWidthBlockCount() const;
	size_t GetHeightBlockCount() const;
	size_t GetBlockCount() const;
	std::optional<Block> GetBlock(size_t idx);
	std::optional<Block> GetBlock(size_t x, size_t y);

private:
	Image& img;
	const size_t blockWidth;
	const size_t blockHeight;
	const size_t widthBlockCount;
	const size_t heightBlockCount;
	const size_t blockCount;
};
