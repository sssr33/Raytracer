#include "BlockQueue.h"
#include "../Helpers.h"

#include <algorithm>

BlockQueue::BlockQueue(Image& img, size_t maxBlockWidth, size_t maxBlockHeight)
	: maxBlockWidth((std::min)(maxBlockWidth, img.GetWidth()))
	, maxBlockHeight((std::min)(maxBlockHeight, img.GetHeight()))
	, image(&img)
{}

std::optional<Block> BlockQueue::Pop()
{
	std::lock_guard<std::mutex> lk(this->mtx);

	if (this->top >= this->image->GetHeight())
	{
		return std::nullopt;
	}

	std::optional<Block> block = this->image->GetBlock(this->left, this->top, this->maxBlockWidth, this->maxBlockHeight);

	if (!block)
	{
		this->left = 0;
		this->top += this->maxBlockHeight;

		block = this->image->GetBlock(this->left, this->top, this->maxBlockWidth, this->maxBlockHeight);
	}

	this->left += this->maxBlockWidth;

	return block;
}