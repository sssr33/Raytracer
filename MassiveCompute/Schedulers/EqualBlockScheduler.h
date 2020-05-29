#pragma once
#include "../Image.h"
#include "../BaseFunctor.h"

#include <functional>

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
		Chunk chunk;
		BaseFunctor functor;
	};
};