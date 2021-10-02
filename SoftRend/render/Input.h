#pragma once
#include <dinput.h>
#include <locale.h>
#include <Windows.h>

enum MouseKeyNumber{
	LeftButton, RightButton, MiddleButton
};

enum MouseAxisNumber{
	X, Y, Z
};

enum MouseMode{
	Relative, Absolute, Screen
};

class InputImpDirectInput{
public:
	InputImpDirectInput();
	void Release();
	int Inititalize(HWND hWnd, HINSTANCE hInst);
	int GetInput();
	bool GetKeyState(int iDIKCode, unsigned int iTimeToNextOutput, unsigned int iTimeBetweenNextOutput);
	bool GetKeyState(int iDIKCode);
	bool GetMouseKeyState(MouseKeyNumber KeyNum);
	int GetMouseAxisState(MouseAxisNumber AxisNum, MouseMode Mode);
	void ChangeMouseCursorVisibility();

	//tst
	unsigned short GetPressedKey();
private:
	HWND _hWnd;
	LPDIRECTINPUT8 _lpdi;
	IDirectInputDevice8 *_lpdiKey;
	IDirectInputDevice8 *_lpdiMouse;

	unsigned char _ucKeyboardState[256];
	DIMOUSESTATE _diMouseState;
	bool _bIsDataReady;
	bool _bIsCursorVisible;

	DWORD _dwTime;
	DWORD _dwPTime;
	DWORD _dwOldKey;

	POINT _pOldCursorPos;
	unsigned int _iAbsX;
	unsigned int _iAbsY;
};

