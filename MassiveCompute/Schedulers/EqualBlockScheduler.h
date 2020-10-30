#pragma once
#include "../Image.h"
#include "../BaseFunctor.h"

#include <functional>

namespace MassiveCompute
{
	class EqualBlockScheduler
	{
	public:
		void operator()(Image& img, BaseFunctor functor);

	private:
		class ThreadFunctor
		{
		public:
			ThreadFunctor(Image& img, BaseFunctor functor, size_t y, size_t height);

			void operator()();

		private:
			Block block;
			BaseFunctor functor;
		};

		// Determines height of the block considering <residualHeight>.
		// <residualHeight> is a height that is left after dividing image height equaly between threads.
		// EXAMPLE
		// For 4 thread cpu and image.Height == 7 each of 4 threads gets 1 row and <residualHeight> == 3.
		// If we will give last thread all of the left rows then row distribution will be
		// th1: 1row th2: 1row th3: 1row th4: 4rows
		// this distribution gives 4th thread 4 times more rows than 1 - 3 thread gets.
		// This method takes into account <residualHeight> == 3 and distributes it to each thread which gives:
		// th1: 2rows th2: 2rows th3: 2rows th4: 1row
		static size_t GetHeightForThread(size_t threadIdx, size_t heightPerThread, size_t residualHeight);
	};
}
