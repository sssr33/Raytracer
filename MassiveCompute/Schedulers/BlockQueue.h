#pragma once
#include "../Image.h"
#include "../ImageBlockIndex.h"

class BlockQueue
{
public:
	//BlockQueue() = default;
	BlockQueue(Image& img, size_t maxBlockWidth, size_t maxBlockHeight);

	//BlockQueue& operator=(const BlockQueue&) = default;

	bool Empty() const;
	size_t Size() const;

	std::optional<Block> Pop();

	// divides the queue into two parts. Returned queue contains items from middle to end.
	BlockQueue SliceBack(size_t maxItemCount);

private:
	ImageBlockIndex blockIdx;
	size_t curBlockIdx = 0;
	size_t endBlockIdx = 0; // block behind last valid index
};
