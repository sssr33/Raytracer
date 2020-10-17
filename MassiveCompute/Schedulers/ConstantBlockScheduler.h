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

namespace MassiveCompute
{
	class ConstantBlockScheduler
	{
	public:
		void operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight);

	private:
		static void Main(BaseFunctor functor, SimpleBlockQueueMt& blockQueue);
	};
}
