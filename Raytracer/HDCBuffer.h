#pragma once
#include "ISystemBuffer.h"

#include <memory>
#include <Windows.h>
#include <Helpers/GdiObjectDeleter.h>
#include <Helpers/GdiDcDeleter.h>
#include <Helpers/GdiDcRelease.h>

class HDCBuffer : public ISystemBuffer
{
public:
	LockedData Lock() override;
	void Unlock() override;

	void SetLastSize(HWND hwnd, const Helpers::Size2D<uint32_t>& lastSize);
	void Present();

private:
	void CheckDibSize(bool force);

	HWND hwnd;
	Helpers::Size2D<uint32_t> lastSize;

	std::unique_ptr<std::remove_pointer<HDC>::type, Helpers::GdiDcRelease> hdc;

	std::unique_ptr<std::remove_pointer<HDC>::type, Helpers::GdiDcDeleter> dibHdc;
	std::unique_ptr<std::remove_pointer<HBITMAP>::type, Helpers::GdiObjectDeleter> dib;
	void* dibMemory = nullptr;
	Helpers::Size2D<uint32_t> dibSize;
};
