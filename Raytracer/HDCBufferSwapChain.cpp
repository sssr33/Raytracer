#include "HDCBufferSwapChain.h"

ISystemBuffer& HDCBufferSwapChain::GetFrontBuffer() {
    auto lk = std::lock_guard(this->mtx);
    return *this->frontBufferPtr;
}

SystemBufferLock HDCBufferSwapChain::LockFrontBuffer() {
    auto lk = std::lock_guard(this->mtx);
    return SystemBufferLock(this->frontBufferPtr);
}

ISystemBuffer& HDCBufferSwapChain::GetBackBuffer() {
    auto lk = std::lock_guard(this->mtx);
    return *this->backBufferPtr;
}

SystemBufferLock HDCBufferSwapChain::LockBackBuffer() {
    auto lk = std::lock_guard(this->mtx);
    this->backBufferPtr->SetLastSize(this->lastHwnd, this->lastSize);
    return SystemBufferLock(this->backBufferPtr);
}

void HDCBufferSwapChain::SwapBuffers() {
    auto lk = std::lock_guard(this->mtx);
    std::swap(this->frontBufferPtr, this->backBufferPtr);
}

void HDCBufferSwapChain::PresentFrontBuffer() {
    auto lk = std::lock_guard(this->mtx);
    this->frontBufferPtr->Present();
}

std::unique_ptr<IBackBufferSwapLock> HDCBufferSwapChain::LockBackBufferSwapLock() {
    return std::make_unique<BackBufferSwapLock>(*this);
}

void HDCBufferSwapChain::SetLastSize(HWND hwnd, const Helpers::Size2D<uint32_t>& lastSize) {
    auto lk = std::lock_guard(this->mtx);
    this->lastHwnd = hwnd;
    this->lastSize = lastSize;
}




HDCBufferSwapChain::BackBufferSwapLock::BackBufferSwapLock(HDCBufferSwapChain& swapChain)
    : swapChain(swapChain)
    , lk(this->swapChain.LockBackBuffer())
{}

HDCBufferSwapChain::BackBufferSwapLock::~BackBufferSwapLock() {
    this->swapChain.SwapBuffers();
}

ISystemBuffer::LockedData& HDCBufferSwapChain::BackBufferSwapLock::GetData() {
    return this->lk.GetData();
}
