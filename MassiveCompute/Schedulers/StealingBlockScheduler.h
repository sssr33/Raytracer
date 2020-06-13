#pragma once
#include "BlockQueue.h"
#include "../Image.h"
#include "../BaseFunctor.h"

class StealingBlockScheduler
{
public:
	void operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight);

private:
	static void Main(BaseFunctor functor, BlockQueue& blockQueue);
};
