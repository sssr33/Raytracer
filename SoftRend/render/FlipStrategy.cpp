// 🐚
#include "StdAfx.h"
#include "FlipStrategy.h"


int FlipWindowedDDStrategy :: Flip(bool bFitToWindow)
{
	p.x = p.y = 0;
	ClientToScreen(_hWnd, &p);

	if(bFitToWindow)
	{
		GetClientRect(_hWnd, &clientRect);
	}
	else
	{
		clientRect.left = clientRect.top = 0;
		clientRect.right = _origWidth;
		clientRect.bottom = _origHeight;
	}
	OffsetRect(&clientRect, p.x, p.y);

	_primSurf->Blt(&clientRect, _backSurf, 0, DDBLT_WAIT, 0);

	return 1;
}

int FlipFullscreenDDStrategy :: Flip(bool bFitToWindow)
{
	
	_primSurf->Flip(NULL, DDFLIP_WAIT);

	return 1;
}
