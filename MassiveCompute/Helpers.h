#pragma once

#include <cstdint>

class Helpers
{
public:
	static size_t CeiledDiv(size_t a, size_t b);
	static size_t NextMultipleOf(size_t val, size_t multiple);
};