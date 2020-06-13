#include "StealingBlockScheduler.h"

void StealingBlockScheduler::operator()(Image& img, BaseFunctor functor, size_t maxBlockWidth, size_t maxBlockHeight)
{
	/*size_t threadsX = Helpers::CeiledDiv(img.GetWidth(), maxBlockWidth);
	size_t threadsY = Helpers::CeiledDiv(img.GetHeight(), maxBlockHeight);
	size_t workerCount = (std::min)(threadsX * threadsY - 1, static_cast<size_t>(std::thread::hardware_concurrency() - 1));*/
}
