#pragma once
#include "SimpleBlockQueueMt.h"
#include "../Image.h"
#include "../BaseFunctor.h"

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

class ConstantBlockSchedulerWithThreads
{
public:
	ConstantBlockSchedulerWithThreads();
	~ConstantBlockSchedulerWithThreads();

	void operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight);

private:
	void WorkerMain();
	void Main(BaseFunctor functor);

	std::mutex mtx;
	std::condition_variable cvWorkers;
	std::condition_variable cvMain;
	size_t workersStart = 0;
	size_t workersActive = 0;
	bool exit = false;
	BaseFunctor functor;

	std::vector<std::thread> workers;

	SimpleBlockQueueMt* blockQueue = nullptr;
};
