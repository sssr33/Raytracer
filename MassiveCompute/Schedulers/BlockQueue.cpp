#include "BlockQueue.h"

#include <algorithm>
#include <cassert>

namespace MassiveCompute
{
	BlockQueue::BlockQueue(Image& img, size_t maxBlockWidth, size_t maxBlockHeight)
		: blockIdx(
			img,
			(std::min)(maxBlockWidth, img.GetWidth()),
			(std::min)(maxBlockHeight, img.GetHeight())
		)
		, endBlockIdx(blockIdx.GetBlockCount())
	{}

	bool BlockQueue::Empty() const
	{
		bool empty = this->curBlockIdx >= this->endBlockIdx;
		return empty;
	}

	size_t BlockQueue::Size() const
	{
		if (this->Empty())
		{
			return 0;
		}

		size_t size = this->endBlockIdx - this->curBlockIdx;
		return size;
	}

	std::optional<Block> BlockQueue::Pop()
	{
		if (this->Empty())
		{
			return std::nullopt;
		}

		std::optional<Block> block = this->blockIdx.GetBlock(this->curBlockIdx);
		this->curBlockIdx++;

		assert(block);

		return block;
	}

	BlockQueue BlockQueue::SliceBack(size_t maxItemCount)
	{
		assert(this->blockIdx.GetImage() != nullptr);

		BlockQueue sliced(
			*this->blockIdx.GetImage(),
			this->blockIdx.GetBlockWidth(),
			this->blockIdx.GetBlockHeight()
		);

		size_t sliceSize = (std::min)(this->Size(), maxItemCount);

		this->endBlockIdx -= sliceSize;

		sliced.curBlockIdx = this->endBlockIdx;
		sliced.endBlockIdx = sliced.curBlockIdx + sliceSize;

		return sliced;
	}
}
