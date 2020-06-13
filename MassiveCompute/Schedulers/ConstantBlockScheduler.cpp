#include "ConstantBlockScheduler.h"
#include "../Helpers.h"

#include <thread>
#include <algorithm>
#include <future>

void ConstantBlockScheduler::operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight)
{
	std::vector<std::future<void>> workerFutures;
	BlockQueue blockQueue(img, maxBlockWidth, maxBlockHeight);
	size_t threadsX = Helpers::CeiledDiv(img.GetWidth(), maxBlockWidth);
	size_t threadsY = Helpers::CeiledDiv(img.GetHeight(), maxBlockHeight);
	size_t workerCount = (std::min)(threadsX * threadsY - 1, static_cast<size_t>(std::thread::hardware_concurrency() - 1));

	workerFutures.reserve(workerCount);

	for (size_t i = 0; i < workerCount; i++)
	{
		std::future<void> fut = std::async(std::launch::async, &ConstantBlockScheduler::Main, functor, std::ref(blockQueue));
		workerFutures.push_back(std::move(fut));
	}

	ConstantBlockScheduler::Main(functor, blockQueue);

	for (auto& fut : workerFutures)
	{
		fut.get();
	}
}

void ConstantBlockScheduler::Main(BaseFunctor functor, BlockQueue& blockQueue)
{
	while (true)
	{
		std::optional<Block> block = blockQueue.Pop();
		if (!block)
		{
			return;
		}

		functor(*block);
	}
}