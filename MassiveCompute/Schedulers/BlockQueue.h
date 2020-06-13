#pragma once
#include "../Image.h"

#include <mutex>

class BlockQueue
{
public:
	BlockQueue() = default;
	BlockQueue(Image& img, size_t maxBlockWidth, size_t maxBlockHeight);

	std::optional<Block> Pop();

private:
	const size_t maxBlockWidth = 0;
	const size_t maxBlockHeight = 0;

	std::mutex mtx;
	size_t left = 0;
	size_t top = 0;
	Image* image = nullptr;
};