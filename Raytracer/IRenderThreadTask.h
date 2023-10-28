#pragma once
#include "ISystemBufferSwapChain.h"

#include <atomic>

class IRenderThreadTask {
public:
    virtual ~IRenderThreadTask() = default;

    virtual void Render(std::unique_ptr<IBackBufferSwapLock> backBufferLk, std::atomic<bool>& cancel) = 0;
};
