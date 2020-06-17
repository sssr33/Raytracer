#pragma once
#include "Image.h"

class ImageBlockIndex
{
public:
	ImageBlockIndex(Image& img, size_t blockWidth, size_t blockHeight);

	size_t GetBlockWidth() const;
	size_t GetBlockHeight() const;
	Image* GetImage();
	const Image* GetImage() const;

	size_t GetWidthBlockCount() const;
	size_t GetHeightBlockCount() const;
	size_t GetBlockCount() const;
	std::optional<Block> GetBlock(size_t idx);
	std::optional<Block> GetBlock(size_t x, size_t y);

private:
	Image* img = nullptr;
	size_t blockWidth = 0;
	size_t blockHeight = 0;
	size_t widthBlockCount = 0;
	size_t heightBlockCount = 0;
	size_t blockCount = 0;
};
