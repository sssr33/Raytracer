#pragma once
#include "ISystemBuffer.h"

#include <memory>

struct SystemBufferUnlockDeleter {
	void operator()(ISystemBuffer* buffer);
};

using SystemBufferLockBase = std::unique_ptr<ISystemBuffer, SystemBufferUnlockDeleter>;

class SystemBufferLock : public SystemBufferLockBase {
public:
	SystemBufferLock() = default;
	SystemBufferLock(ISystemBuffer* buffer);

	const ISystemBuffer::LockedData& GetData() const;
	ISystemBuffer::LockedData& GetData();

private:
	ISystemBuffer::LockedData data;
};
