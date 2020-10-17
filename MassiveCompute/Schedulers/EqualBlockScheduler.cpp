#include "EqualBlockScheduler.h"

#include <future>
#include <vector>
#include <cassert>
#include <algorithm>

namespace MassiveCompute
{
	void EqualBlockScheduler::operator()(Image& img, BaseFunctor functor)
	{
		if (img.Empty())
		{
			return;
		}

		size_t threadCount = std::min(static_cast<size_t>(std::thread::hardware_concurrency()), img.GetHeight());
		size_t workerCount = threadCount - 1; // -1 thread because this thread will also run functor after starting workers
		size_t heightPerThread = img.GetHeight() / threadCount;
		size_t residualHeight = img.GetHeight() % threadCount;
		size_t y = 0;
		std::vector<std::future<void>> threadFutures;

		threadFutures.reserve(workerCount);

		// run workers(max threads - 1)
		for (size_t i = 0; i < workerCount; i++)
		{
			size_t currentHeightPerThread = GetHeightForThread(i, heightPerThread, residualHeight);

			std::future<void> fut = std::async(std::launch::async, ThreadFunctor(img, functor, y, currentHeightPerThread));
			threadFutures.push_back(std::move(fut));

			y += currentHeightPerThread;
		}

		assert(y <= img.GetHeight());

		// run main thread
		ThreadFunctor(img, functor, y, GetHeightForThread(workerCount, heightPerThread, residualHeight))();

		for (auto& fut : threadFutures)
		{
			fut.get();
		}
	}

	size_t EqualBlockScheduler::GetHeightForThread(size_t threadIdx, size_t heightPerThread, size_t residualHeight)
	{
		assert(residualHeight < std::thread::hardware_concurrency());

		if (threadIdx < residualHeight)
		{
			return heightPerThread + 1;
		}
		else
		{
			return heightPerThread;
		}
	}

	EqualBlockScheduler::ThreadFunctor::ThreadFunctor(Image& img, BaseFunctor functor, size_t y, size_t height)
		: block(*img.GetBlock(0, y, img.GetWidth(), height))
		, functor(std::move(functor))
	{}

	void EqualBlockScheduler::ThreadFunctor::operator()()
	{
		this->functor(this->block);
	}
}
