// 🐚
#include "StdAfx.h"
#include "FPSMeter.h"


FPSMeter::FPSMeter(void)
{
	_dwStartTime = _dwEndTime = GetTickCount();
	_dwFrameCount = _dwStartFrameCount = _dwFrameRate = 0;
}

DWORD FPSMeter::GetFPS(){
	return _dwFrameRate;
}

void FPSMeter::ProcessFPSMeter(){
	_dwEndTime = GetTickCount();

		if (_dwEndTime - _dwStartTime > 1000)
		{
			_dwFrameRate = (_dwFrameCount - _dwStartFrameCount) * 1000 / (_dwEndTime - _dwStartTime);
			_dwStartTime = _dwEndTime;
			_dwStartFrameCount = _dwFrameCount;
		}
		_dwFrameCount++;
}