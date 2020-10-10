#include "StealingBlockScheduler.h"

#include <algorithm>
#include <future>
#include <cassert>
#include <atomic>
#include <iterator>
#include <Helpers/Math.h>

void StealingBlockScheduler::operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight)
{
	BlockQueue blockQueue(img, maxBlockWidth, maxBlockHeight);
	size_t workerCount = (std::min)(blockQueue.Size() - 1, static_cast<size_t>(std::thread::hardware_concurrency() - 1));
	std::vector<std::shared_ptr<StealingBlockQueue>> poolQueues =
		StealingBlockScheduler::PoolQueuesFromQueues(
			StealingBlockScheduler::DivideBlockQueue(std::move(blockQueue), workerCount + 1) // + 1 for main thread
		);

	assert(StealingBlockScheduler::AnyEmptyOrNoItems(poolQueues) == false);

	StealingPool stealingPool(poolQueues);
	std::vector<std::future<void>> workerFutures;

	workerFutures.reserve(poolQueues.size() - 1);

	while (poolQueues.size() > 1)
	{
		std::shared_ptr<StealingBlockQueue> queue = std::move(poolQueues.back());
		poolQueues.pop_back();

		std::future<void> fut = std::async(std::launch::async, &StealingBlockScheduler::Main, functor, std::move(queue), std::ref(stealingPool));
		workerFutures.push_back(std::move(fut));
	}

	assert(poolQueues.size() == 1);

	StealingBlockScheduler::Main(functor, std::move(poolQueues.back()), stealingPool);

	for (auto& i : workerFutures)
	{
		i.get();
	}

	assert(stealingPool.Empty() == true);
}

void StealingBlockScheduler::Main(BaseFunctor functor, std::shared_ptr<StealingBlockQueue> queue, StealingPool& stealingPool)
{
	while (queue)
	{
		while (std::optional<Block> block = queue->Pop())
		{
			functor(*block);
		}

		queue = stealingPool.TrySteal();
	}
}

std::vector<BlockQueue> StealingBlockScheduler::DivideBlockQueue(BlockQueue queue, size_t parts)
{
	std::vector<BlockQueue> dividedParts;
	size_t blockCount = queue.Size() / parts;

	dividedParts.reserve(parts);

	for (size_t part = 0; part < parts - 1; part++)
	{
		dividedParts.push_back(queue.SliceBack(blockCount));
	}

	dividedParts.push_back(std::move(queue));

	assert(dividedParts.size() == parts);

	return dividedParts;
}

std::vector<std::shared_ptr<StealingBlockScheduler::StealingBlockQueue>> StealingBlockScheduler::PoolQueuesFromQueues(std::vector<BlockQueue> poolQueues)
{
	std::sort(poolQueues.begin(), poolQueues.end(),
		[](const BlockQueue& a, const BlockQueue& b)
		{
			return a.Size() < b.Size();
		});

	std::vector<std::shared_ptr<StealingBlockQueue>> pool;

	pool.reserve(poolQueues.size());

	std::transform(poolQueues.begin(), poolQueues.end(), std::back_inserter(pool),
		[](BlockQueue& queue)
		{
			return std::make_shared<StealingBlockQueue>(std::move(queue));
		});

	return pool;
}

bool StealingBlockScheduler::AnyEmptyOrNoItems(const std::vector<std::shared_ptr<StealingBlockQueue>>& items)
{
	if (items.empty())
	{
		return true;
	}

	for (auto& i : items)
	{
		if (!i)
		{
			return true;
		}

		if (i->Empty())
		{
			return true;
		}
	}

	return false;
}




StealingBlockScheduler::StealingBlockQueue::StealingBlockQueue(BlockQueue blockQueue)
	: blockQueue(std::move(blockQueue))
{}

bool StealingBlockScheduler::StealingBlockQueue::Empty() const
{
	std::lock_guard<std::mutex> lk(this->mtx);
	return this->blockQueue.Empty();
}

BlockQueue StealingBlockScheduler::StealingBlockQueue::StealHalf()
{
	std::lock_guard<std::mutex> lk(this->mtx);
	size_t size = this->blockQueue.Size();
	// use CeiledDiv to steal greater part for child queue which will be processed soon
	size_t half = Helpers::Math::CeiledDiv(size, 2);
	BlockQueue otherQueue = this->blockQueue.SliceBack(half);

	return otherQueue;
}

std::optional<Block> StealingBlockScheduler::StealingBlockQueue::Pop()
{
	std::lock_guard<std::mutex> lk(this->mtx);
	return this->blockQueue.Pop();
}




StealingBlockScheduler::StealingPool::StealingPool(std::vector<std::shared_ptr<StealingBlockQueue>> poolQueues)
	: pool(StealingPool::PoolFromQueues(std::move(poolQueues)))
{}

bool StealingBlockScheduler::StealingPool::Empty() const
{
	std::lock_guard<std::mutex> lk(this->mtx);
	return this->pool.empty();
}

std::shared_ptr<StealingBlockScheduler::StealingBlockQueue> StealingBlockScheduler::StealingPool::TrySteal()
{
	std::lock_guard<std::mutex> lk(this->mtx);

	while (!pool.empty())
	{
		// Every StealingBlockQueue asynchronously changes its size
		// so we try to steal anything while there are some StealingBlockQueue's.

		std::shared_ptr<StealingBlockQueue> parent = std::move(pool.front());
		pool.pop_front();

		std::shared_ptr<StealingBlockQueue> child = std::make_shared<StealingBlockQueue>(parent->StealHalf());

		if (!parent->Empty())
		{
			pool.push_back(std::move(parent));
		}

		if (!child->Empty())
		{
			pool.push_back(child);
			return child;
		}
	}

	return nullptr;
}

std::deque<std::shared_ptr<StealingBlockScheduler::StealingBlockQueue>> StealingBlockScheduler::StealingPool::PoolFromQueues(std::vector<std::shared_ptr<StealingBlockQueue>> poolQueues)
{
	std::deque<std::shared_ptr<StealingBlockScheduler::StealingBlockQueue>> pool;

	std::move(poolQueues.begin(), poolQueues.end(), std::back_inserter(pool));

	return pool;
}
