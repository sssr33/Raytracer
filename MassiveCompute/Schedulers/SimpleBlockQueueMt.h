#pragma once
#include "BlockQueue.h"

#include <mutex>

class SimpleBlockQueueMt
{
public:
	SimpleBlockQueueMt(BlockQueue queue);
	SimpleBlockQueueMt(Image& img, size_t maxBlockWidth, size_t maxBlockHeight);

	std::optional<Block> Pop();

private:
	std::mutex mtx;
	BlockQueue queue;
};
