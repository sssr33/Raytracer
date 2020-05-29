#include "BlockQueue.h"

#include <algorithm>

BlockQueue::BlockQueue(Image& img, size_t maxBlockWidth, size_t maxBlockHeight)
	: maxBlockWidth((std::min)(maxBlockWidth, img.GetWidth()))
	, maxBlockHeight((std::min)(maxBlockHeight, img.GetHeight()))
	, image(&img)
{}

std::optional<Chunk> BlockQueue::Pop()
{
	std::lock_guard<std::mutex> lk(this->mtx);

	if (this->top >= this->image->GetHeight())
	{
		return std::nullopt;
	}

	std::optional<Chunk> chunk = this->image->GetChunk(this->left, this->top, this->maxBlockWidth, this->maxBlockHeight);

	if (!chunk)
	{
		this->left = 0;
		this->top += this->maxBlockHeight;

		chunk = this->image->GetChunk(this->left, this->top, this->maxBlockWidth, this->maxBlockHeight);
	}

	this->left += this->maxBlockWidth;

	return chunk;
}