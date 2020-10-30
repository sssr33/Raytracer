#pragma once
#include "BlockQueue.h"
#include "../Image.h"
#include "../BaseFunctor.h"

#include <mutex>
#include <deque>
#include <memory>

namespace MassiveCompute
{
	class StealingBlockScheduler
	{
	public:
		void operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight);

	private:
		class StealingBlockQueue
		{
		public:
			StealingBlockQueue(BlockQueue blockQueue);

			bool Empty() const;
			BlockQueue StealHalf();

			std::optional<Block> Pop();

		private:
			mutable std::mutex mtx;
			BlockQueue blockQueue;
		};

		class StealingPool
		{
		public:
			StealingPool(std::vector<std::shared_ptr<StealingBlockQueue>> poolQueues);

			bool Empty() const;
			std::shared_ptr<StealingBlockQueue> TrySteal();

		private:
			mutable std::mutex mtx;
			// largest queues in the front
			std::deque<std::shared_ptr<StealingBlockQueue>> pool;

			static std::deque<std::shared_ptr<StealingBlockQueue>> PoolFromQueues(std::vector<std::shared_ptr<StealingBlockQueue>> poolQueues);
		};

		static void Main(BaseFunctor functor, std::shared_ptr<StealingBlockQueue> queue, StealingPool& stealingPool);

		static std::vector<BlockQueue> DivideBlockQueue(BlockQueue queue, size_t parts);
		static std::vector<std::shared_ptr<StealingBlockQueue>> PoolQueuesFromQueues(std::vector<BlockQueue> poolQueues);
		static bool AnyEmptyOrNoItems(const std::vector<std::shared_ptr<StealingBlockQueue>>& items);
	};
}
