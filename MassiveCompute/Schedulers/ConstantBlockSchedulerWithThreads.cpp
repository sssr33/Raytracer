#include "ConstantBlockSchedulerWithThreads.h"
#include "../Helpers.h"

#include <thread>
#include <algorithm>

ConstantBlockSchedulerWithThreads::ConstantBlockSchedulerWithThreads()
{
	size_t workerCount = std::thread::hardware_concurrency() - 1;
	this->workers.reserve(workerCount);

	for (size_t i = 0; i < workerCount; i++)
	{
		this->workers.push_back(std::thread(&ConstantBlockSchedulerWithThreads::WorkerMain, this));
	}
}

ConstantBlockSchedulerWithThreads::~ConstantBlockSchedulerWithThreads()
{
	{
		std::lock_guard<std::mutex> lk(this->mtx);
		this->exit = true;
	}
	this->cvWorkers.notify_all();

	for (auto& th : this->workers)
	{
		th.join();
	}
}

void ConstantBlockSchedulerWithThreads::operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight)
{
	size_t threadsX = Helpers::CeiledDiv(img.GetWidth(), maxBlockWidth);
	size_t threadsY = Helpers::CeiledDiv(img.GetHeight(), maxBlockHeight);
	size_t workerCount = (std::min)(threadsX * threadsY - 1, this->workers.size());

	BlockQueue blockQueueForRun(img, maxBlockWidth, maxBlockHeight);
	this->blockQueue = &blockQueueForRun;

	{
		std::lock_guard<std::mutex> lk(this->mtx);
		this->functor = functor;
		this->workersStart += workerCount;
		this->workersActive += workerCount;
	}
	this->cvWorkers.notify_all();

	this->Main(functor);

	{
		std::unique_lock<std::mutex> lk(this->mtx);
		this->cvMain.wait(lk, [&] { return this->workersActive == 0; });
		this->functor = BaseFunctor();
	}

	this->blockQueue = nullptr;
}

void ConstantBlockSchedulerWithThreads::WorkerMain()
{
	while (true)
	{
		BaseFunctor functor;

		{
			std::unique_lock<std::mutex> lk(this->mtx);
			this->cvWorkers.wait(lk, [&] { return this->workersStart != 0 || this->exit; });

			if (this->exit)
			{
				return;
			}

			this->workersStart--;
			functor = this->functor;
		}

		this->Main(std::move(functor));

		bool lastWorker = false;
		{
			std::lock_guard<std::mutex> lk(this->mtx);
			this->workersActive--;
			lastWorker = this->workersActive == 0;
		}

		if (lastWorker)
		{
			this->cvMain.notify_one();
		}
	}
}

void ConstantBlockSchedulerWithThreads::Main(BaseFunctor functor)
{
	while (true)
	{
		std::optional<Chunk> chunk = this->blockQueue->Pop();
		if (!chunk)
		{
			return;
		}

		functor(*chunk);
	}
}