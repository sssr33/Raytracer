#pragma once
class FPSMeter
{
public:
	FPSMeter(void);
	DWORD GetFPS();
	void ProcessFPSMeter();
private:
	DWORD _dwFrameCount;
	DWORD _dwStartFrameCount;
	DWORD _dwFrameRate;
	DWORD _dwStartTime;
	DWORD _dwEndTime;
};