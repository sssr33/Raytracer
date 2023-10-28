#include "SystemBufferLock.h"

SystemBufferLock::SystemBufferLock(ISystemBuffer* buffer)
	: SystemBufferLockBase(buffer)
	, data(this->get() ? this->get()->Lock() : ISystemBuffer::LockedData {})
{}

const ISystemBuffer::LockedData& SystemBufferLock::GetData() const {
	return this->data;
}

ISystemBuffer::LockedData& SystemBufferLock::GetData() {
	return this->data;
}

void SystemBufferUnlockDeleter::operator()(ISystemBuffer* buffer) {
	buffer->Unlock();
}
