#include "SystemBackBufferLock.h"

SystemBackBufferLock::SystemBackBufferLock(ISystemBackBuffer& buffer)
	: buffer(buffer)
	, data(buffer.Lock())
{}

SystemBackBufferLock::~SystemBackBufferLock()
{
	this->buffer.Unlock();
}

const ISystemBackBuffer::LockedData& SystemBackBufferLock::GetData() const
{
	return this->data;
}

ISystemBackBuffer::LockedData& SystemBackBufferLock::GetData()
{
	return this->data;
}
