#pragma once
#include "BlockQueue.h"

#include <mutex>

class SimpleBlockQueueMt
{
public:
	SimpleBlockQueueMt(BlockQueue queue);
	SimpleBlockQueueMt(Image& img, size_t maxBlockWidth, size_t maxBlockHeight);

	size_t Size() const;

	std::optional<Block> Pop();

private:
	mutable std::mutex mtx;
	BlockQueue queue;
};
