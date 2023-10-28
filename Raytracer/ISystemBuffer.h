#pragma once

#include <cstdint>
#include <Helpers/Size2D.h>

class ISystemBuffer {
public:
	struct LockedData
	{
		void* data = nullptr;
		// size of data line in bytes can be greater than size.width, for alignment
		uint32_t dataLineByteSize = 0;
		Helpers::Size2D<uint32_t> size;
	};

	virtual ~ISystemBuffer() = default;

	virtual LockedData Lock() = 0;
	virtual void Unlock() = 0;
};
