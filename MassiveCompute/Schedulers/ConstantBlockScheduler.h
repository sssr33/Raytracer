#pragma once
#include "BlockQueue.h"
#include "../Image.h"
#include "../BaseFunctor.h"

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

class ConstantBlockScheduler
{
public:
	void operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight);

private:
	static void Main(BaseFunctor functor, BlockQueue& blockQueue);
};
