#pragma once

#include <ddraw.h>
#include <Windows.h>

class FlipDDStrategy
{
public:
	virtual ~FlipDDStrategy(void) {}
	virtual int Flip(bool bFitToWindow = false) = 0;
protected:
	FlipDDStrategy(void) {}
	LPDIRECTDRAWSURFACE7 _primSurf;
	LPDIRECTDRAWSURFACE7 _backSurf;
	HWND _hWnd;
	int _origWidth;
	int _origHeight;
};

class FlipWindowedDDStrategy : public FlipDDStrategy
{
public:
	FlipWindowedDDStrategy(LPDIRECTDRAWSURFACE7 lpPrimarySurface, LPDIRECTDRAWSURFACE7 lpBackSurface, HWND hWnd, int width, int height) 
	{
		_primSurf = lpPrimarySurface;
		_backSurf = lpBackSurface;
		_hWnd = hWnd;
		_origWidth = width;
		_origHeight = height;
	}
	int Flip(bool bFitToWindow = false);
private:
	RECT clientRect;
	POINT p;
};

class FlipFullscreenDDStrategy  : public FlipDDStrategy
{
public:
	FlipFullscreenDDStrategy(LPDIRECTDRAWSURFACE7 lpPrimarySurface) 
	{
		_primSurf = lpPrimarySurface;
		_backSurf = 0;
		_hWnd = 0;
	}
	int Flip(bool bFitToWindow = false);
};

