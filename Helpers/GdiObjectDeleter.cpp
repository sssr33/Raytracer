#include "GdiObjectDeleter.h"

namespace Helpers
{
	void GdiObjectDeleter::operator()(HGDIOBJ obj)
	{
		DeleteObject(obj);
	}
}
