#pragma once

#include <Windows.h>

namespace Helpers
{
	struct GdiDcDeleter
	{
		void operator()(HDC hdc);
	};
}
