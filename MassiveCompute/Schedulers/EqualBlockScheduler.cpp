#include "EqualBlockScheduler.h"

#include <future>
#include <vector>
#include <cassert>

void EqualBlockScheduler::operator()(Image& img, BaseFunctor functor)
{
	size_t threadCount = std::thread::hardware_concurrency() - 1;
	size_t heightPerThread = img.GetHeight() / (threadCount + 1);
	size_t y = 0;
	std::vector<std::future<void>> threadFutures;

	threadFutures.reserve(threadCount);

	// run workers(max threads - 1)
	for (size_t i = 0; i < threadCount; i++, y += heightPerThread)
	{
		std::future<void> fut = std::async(std::launch::async, ThreadFunctor(img, functor, y, heightPerThread));
		threadFutures.push_back(std::move(fut));
	}

	assert(y <= img.GetHeight());

	// run main thread
	ThreadFunctor(img, functor, y, img.GetHeight() - y)();

	for (auto& fut : threadFutures)
	{
		fut.get();
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