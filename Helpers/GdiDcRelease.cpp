#include "GdiDcRelease.h"

namespace Helpers
{
	GdiDcRelease::GdiDcRelease(HWND hwnd)
		: hwnd(hwnd)
	{}

	void GdiDcRelease::operator()(HDC hdc)
	{
		ReleaseDC(this->hwnd, hdc);
	}
}
