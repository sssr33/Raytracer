// 🐚
#include "StdAfx.h"
#include "Input.h"


InputImpDirectInput::InputImpDirectInput(){
	_lpdi = 0;
	_bIsCursorVisible = 1;
	_dwTime = 0;
	_dwOldKey = 0;
	_dwPTime = 0;
	_pOldCursorPos.x = _pOldCursorPos.y = 0;
}

void InputImpDirectInput::Release(){
	if(_lpdiMouse){
		_lpdiMouse->Unacquire();
		_lpdiMouse->Release();
		_lpdiMouse = 0;
	}

	if(_lpdiKey){
		_lpdiKey->Unacquire();
		_lpdiKey->Release();
		_lpdiKey = 0;
	}

	if(_lpdi){
		_lpdi->Release();
		_lpdi = 0;
	}
}

int InputImpDirectInput::Inititalize(HWND hWnd, HINSTANCE hInst){
	
	if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&_lpdi, 0))) return 0;

	if(FAILED(_lpdi->CreateDevice(GUID_SysKeyboard, &_lpdiKey, 0))) return 0;
	if(FAILED(_lpdi->CreateDevice(GUID_SysMouse, &_lpdiMouse, 0))) return 0;
	
	if(FAILED(_lpdiKey->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) return 0;
	if(FAILED(_lpdiMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) return 0;

	if(FAILED(_lpdiKey->SetDataFormat(&c_dfDIKeyboard))) return 0;
	if(FAILED(_lpdiMouse->SetDataFormat(&c_dfDIMouse))) return 0;

	if(FAILED(_lpdiKey->Acquire())) return 0;
	if(FAILED(_lpdiMouse->Acquire())) return 0;

	POINT pInitial;

	GetCursorPos(&pInitial);

	_iAbsX = pInitial.x;
	_iAbsY = pInitial.y;

	_hWnd = hWnd;

	return 1;
}

int InputImpDirectInput::GetInput(){

	//if(FAILED(_lpdiKey->GetDeviceState(256, &_ucKeyboardState))) return 0;

	while(_lpdiKey->GetDeviceState(256, &_ucKeyboardState) == DIERR_INPUTLOST){
		if(FAILED(_lpdiKey->Acquire())){
			_bIsDataReady = 0;
			return 0;
		}
	}

	while(_lpdiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &_diMouseState) == DIERR_INPUTLOST){
		if(FAILED(_lpdiMouse->Acquire())){
			_bIsDataReady = 0;
			return 0;
		}
	}

	_iAbsX += _diMouseState.lX;
	_iAbsY += _diMouseState.lY;

	if(_iAbsX < 0) _iAbsX = 0;
	if(_iAbsY < 0) _iAbsY = 0;

	_bIsDataReady = 1;

	return 1;
}

bool InputImpDirectInput::GetKeyState(int iDIKCode, unsigned int iTimeToNextOutput, unsigned int iTimeBetweenNextOutput){

	bool isKeyPressed = 0;

	if(!_bIsDataReady) return 0;

	if(_ucKeyboardState[iDIKCode] & 0x80)
	{
		if(iDIKCode != _dwOldKey)
		{
			_dwOldKey = iDIKCode;
			_dwTime = GetTickCount();
			isKeyPressed = 1;
			_dwPTime = iTimeToNextOutput;
		}
		else
		{
			if(GetTickCount() - _dwTime >= _dwPTime)
			{
				isKeyPressed = 1;
				_dwTime = GetTickCount();
				_dwPTime = iTimeBetweenNextOutput;
			}
			else
			{
				isKeyPressed = 0;
			}
		}
	}
	else
	{
		if(iDIKCode == _dwOldKey)
		{
			//dwOldKey = iDIKCode;
			_dwOldKey = 0;
			_dwPTime = 0;
			_dwTime = GetTickCount();
		}
	}

	return isKeyPressed;
}

bool InputImpDirectInput::GetKeyState(int iDIKCode)
{
	if(_bIsDataReady)
		return (_ucKeyboardState[iDIKCode] & 0x80);

	return false;
}

bool InputImpDirectInput::GetMouseKeyState(MouseKeyNumber KeyNum){
	
	if(_bIsDataReady)
		return (_diMouseState.rgbButtons[(int)KeyNum] & 0x80);

	return 0;
}

int InputImpDirectInput::GetMouseAxisState(MouseAxisNumber AxisNum, MouseMode Mode){

	if(Mode == MouseMode::Screen){
		POINT pt;
		RECT /*rWnd,*/ r;
		GetClientRect(_hWnd, &r);

		GetCursorPos(&pt);
		ScreenToClient(_hWnd, &pt);
		if(AxisNum == MouseAxisNumber::X){

			if(pt.x < 0) pt.x = 0;
			if(pt.x >= r.right) pt.x = r.right;
			
			return pt.x;
		}
		else if(AxisNum == MouseAxisNumber::Y){

			if(pt.y < 0) pt.y = 0;
			if(pt.y >= r.bottom) pt.y = r.bottom;

			return pt.y;
		}
	}

	if(Mode == MouseMode::Absolute){
		if(AxisNum == MouseAxisNumber::X){
			return _iAbsX;
		}
		else if(AxisNum == MouseAxisNumber::Y){
			return _iAbsY;
		}
	}


	if(Mode == MouseMode::Relative){
		if(AxisNum == MouseAxisNumber::X){
			return _diMouseState.lX;
		}
		else if(AxisNum == MouseAxisNumber::Y){
			return _diMouseState.lY;
		}
		else if(AxisNum == MouseAxisNumber::Z){
			return _diMouseState.lZ;
		}
	}

	return 0;
}

void InputImpDirectInput::ChangeMouseCursorVisibility(){

	if(_bIsCursorVisible)
		GetCursorPos(&_pOldCursorPos);
	else
		SetCursorPos(_pOldCursorPos.x, _pOldCursorPos.y);

	_bIsCursorVisible = !_bIsCursorVisible;
	ShowCursor(_bIsCursorVisible);
}

unsigned short InputImpDirectInput::GetPressedKey()
{
	if(!_bIsDataReady) return 0;

	short sKey;
	static HKL KeyboardLayout = GetKeyboardLayout(0);
	unsigned char WinKeyStates[256];
	unsigned short /*i,*/ ScanCode, VirtualKey, /*Keys,*/ Num;
	wchar_t str[1];

	GetKeyboardState(WinKeyStates);

	for(int i = 0; i < 256; i++)
	{
		sKey = i;
		if(_ucKeyboardState[i] & 0x80)
		{
			if((VirtualKey = MapVirtualKeyEx((ScanCode = i), 1, KeyboardLayout))) {
			// Get ASCII code of key and return it
			//Num = ToAsciiEx(VirtualKey, ScanCode, WinKeyStates, &Keys, 0, KeyboardLayout);
			Num = (unsigned short)ToUnicodeEx(VirtualKey, ScanCode, WinKeyStates, str, 1, 0, KeyboardLayout);
				if(Num)
					return (unsigned short)str[0];
					//return Keys;
			}
		}
	}

	return 0;
}