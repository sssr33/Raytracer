#pragma once

#include <cstdint>
#include <Helpers/Size2D.h>

class ISystemBackBuffer
{
public:
	struct LockedData
	{
		void* data = nullptr;
		Helpers::Size2D<uint32_t> size;
	};

	virtual ~ISystemBackBuffer() = default;

	virtual LockedData Lock() = 0;
	virtual void Unlock() = 0;
};
