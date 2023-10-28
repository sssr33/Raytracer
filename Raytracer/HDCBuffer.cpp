#include "HDCBuffer.h"

HDCBuffer::LockedData HDCBuffer::Lock()
{
	if (!this->hwnd)
	{
		return LockedData();
	}

	this->hdc.reset();
	this->hdc = std::unique_ptr<std::remove_pointer<HDC>::type, Helpers::GdiDcRelease>(GetDC(this->hwnd), Helpers::GdiDcRelease(this->hwnd));
	if (!this->hdc)
	{
		DWORD error = GetLastError();
		throw std::exception();
	}

	bool forceRecreateDib = false;

	if (!this->dibHdc)
	{
		this->dibHdc.reset(CreateCompatibleDC(this->hdc.get()));
		forceRecreateDib = true;

		if (!this->dibHdc)
		{
			DWORD error = GetLastError();
			throw std::exception();
		}
	}

	this->CheckDibSize(forceRecreateDib);

	if (this->dibSize.Empty())
	{
		return LockedData();
	}

	LockedData data;

	data.data = this->dibMemory;
	data.size = this->dibSize;
	data.dataLineByteSize = data.size.width * 4;

	return data;
}

void HDCBuffer::Unlock()
{
	// no op
}

void HDCBuffer::SetLastSize(HWND hwnd, const Helpers::Size2D<uint32_t>& lastSize)
{
	this->hwnd = hwnd;
	this->lastSize = lastSize;
}

void HDCBuffer::Present()
{
	if (!this->hwnd)
	{
		return;
	}

	if (this->dibSize.Empty())
	{
		return;
	}

	// save to local to release on function exit
	//std::unique_ptr<std::remove_pointer<HDC>::type, Helpers::GdiDcRelease> localHdc = std::move(this->hdc);

	HGDIOBJ prevObjet = SelectObject(this->dibHdc.get(), this->dib.get());
	if (!BitBlt(this->hdc.get(), 0, 0, static_cast<int>(this->dibSize.width), static_cast<int>(this->dibSize.height), this->dibHdc.get(), 0, 0, SRCCOPY))
	{
		DWORD error = GetLastError();
		this->dibHdc = nullptr;
		throw std::exception();
	}
	SelectObject(this->dibHdc.get(), prevObjet);
}

void HDCBuffer::CheckDibSize(bool force)
{
	if (this->lastSize == this->dibSize && !force)
	{
		return;
	}

	void* buffer = nullptr;
	HBITMAP bitmap = nullptr;

	if (!this->lastSize.Empty())
	{
		BITMAPINFO bi = {};

		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = static_cast<LONG>(this->lastSize.width);
		// negative height to make (0;0) at left top
		bi.bmiHeader.biHeight = -static_cast<LONG>(this->lastSize.height);
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;

		bitmap = CreateDIBSection(this->dibHdc.get(), &bi, DIB_RGB_COLORS, &buffer, NULL, 0);
		if (!bitmap)
		{
			DWORD error = GetLastError();
			throw std::exception();
		}
	}

	this->dib.reset(bitmap);
	this->dibMemory = buffer;
	this->dibSize = this->lastSize;
}
