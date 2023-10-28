#pragma once
#include "ISystemBufferSwapChain.h"
#include "HDCBuffer.h"

#include <mutex>

class HDCBufferSwapChain final : public ISystemBufferSwapChain {
public:
    ISystemBuffer& GetFrontBuffer() override;
    SystemBufferLock LockFrontBuffer() override;

    ISystemBuffer& GetBackBuffer() override;
    SystemBufferLock LockBackBuffer() override;

    void SwapBuffers() override;
    void PresentFrontBuffer() override;

    std::unique_ptr<IBackBufferSwapLock> LockBackBufferSwapLock() override;

    void SetLastSize(HWND hwnd, const Helpers::Size2D<uint32_t>& lastSize);

private:
    class BackBufferSwapLock final : public IBackBufferSwapLock {
    public:
        BackBufferSwapLock(HDCBufferSwapChain& swapChain);
        ~BackBufferSwapLock();

        ISystemBuffer::LockedData& GetData() override;

    private:
        HDCBufferSwapChain& swapChain;
        SystemBufferLock lk;
    };

    std::mutex mtx;
    HDCBuffer backBuffer;
    HDCBuffer frontBuffer;

    HDCBuffer* backBufferPtr = &this->backBuffer;
    HDCBuffer* frontBufferPtr = &this->frontBuffer;

    HWND lastHwnd = nullptr;
    Helpers::Size2D<uint32_t> lastSize;
};
