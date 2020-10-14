#pragma once

#include <Windows.h>

namespace Helpers
{
	struct GdiDcRelease
	{
		GdiDcRelease() = default;
		explicit GdiDcRelease(HWND hwnd);

		void operator()(HDC hdc);

	private:
		HWND hwnd = nullptr;
	};
}
