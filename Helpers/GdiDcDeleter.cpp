#include "GdiDcDeleter.h"

namespace Helpers
{
	void GdiDcDeleter::operator()(HDC hdc)
	{
		DeleteDC(hdc);
	}
}
