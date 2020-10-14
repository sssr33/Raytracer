#pragma once
#include "ISystemBackBuffer.h"

class SystemBackBufferLock
{
public:
	SystemBackBufferLock(ISystemBackBuffer& buffer);
	~SystemBackBufferLock();

	const ISystemBackBuffer::LockedData& GetData() const;
	ISystemBackBuffer::LockedData& GetData();

private:
	ISystemBackBuffer& buffer;
	ISystemBackBuffer::LockedData data;
};
