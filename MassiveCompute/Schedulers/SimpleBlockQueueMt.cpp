#include "SimpleBlockQueueMt.h"

namespace MassiveCompute
{
	SimpleBlockQueueMt::SimpleBlockQueueMt(BlockQueue queue)
		: queue(std::move(queue))
	{}

	SimpleBlockQueueMt::SimpleBlockQueueMt(Image& img, size_t maxBlockWidth, size_t maxBlockHeight)
		: queue(img, maxBlockWidth, maxBlockHeight)
	{}

	size_t SimpleBlockQueueMt::Size() const
	{
		std::lock_guard<std::mutex> lk(this->mtx);
		return this->queue.Size();
	}

	std::optional<Block> SimpleBlockQueueMt::Pop()
	{
		std::lock_guard<std::mutex> lk(this->mtx);
		return this->queue.Pop();
	}
}
