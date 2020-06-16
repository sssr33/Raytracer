#include "SimpleBlockQueueMt.h"

SimpleBlockQueueMt::SimpleBlockQueueMt(BlockQueue queue)
	: queue(std::move(queue))
{}

SimpleBlockQueueMt::SimpleBlockQueueMt(Image& img, size_t maxBlockWidth, size_t maxBlockHeight)
	: queue(img, maxBlockWidth, maxBlockHeight)
{}

std::optional<Block> SimpleBlockQueueMt::Pop()
{
	std::lock_guard<std::mutex> lk(this->mtx);
	return this->queue.Pop();
}
