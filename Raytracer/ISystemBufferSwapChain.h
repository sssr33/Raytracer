#pragma once
#include "ISystemBuffer.h"
#include "SystemBufferLock.h"

#include <memory>

class IBackBufferSwapLock {
public:
    virtual ~IBackBufferSwapLock() = default;

    virtual ISystemBuffer::LockedData& GetData() = 0;
};

class ISystemBackBufferPart {
public:
    virtual ~ISystemBackBufferPart() = default;

    virtual std::unique_ptr<IBackBufferSwapLock> LockBackBufferSwapLock() = 0;
};

class ISystemBufferSwapChain : public ISystemBackBufferPart {
public:
    virtual ~ISystemBufferSwapChain() = default;

    virtual ISystemBuffer& GetFrontBuffer() = 0;
    virtual SystemBufferLock LockFrontBuffer() = 0;

    virtual ISystemBuffer& GetBackBuffer() = 0;
    virtual SystemBufferLock LockBackBuffer() = 0;

    virtual void SwapBuffers() = 0;
    virtual void PresentFrontBuffer() = 0;
};
