#pragma once

#include <memory>
#include <Windows.h>

namespace Helpers
{
	struct GdiObjectDeleter
	{
		void operator()(HGDIOBJ obj);
	};
}
