#pragma once
#include "stdafx.h"
#define setBit(val, bitNum) { val |= (1 << bitNum); }

enum SIMD
{
	MMX = 0x00000001, SSE1 = 0x00000002, SSE2 = 0x00000004, SSE3 = 0x00000008, 
	SSSE3 = 0x00000010, SSE4_1 = 0x00000020, SSE4_2 = 0x00000040, AVX = 0x00000080,
	NO_SIMD = 0
};

class Sys
{
public:
	~Sys(void);

	static int Initialize();
	static int Release();

	static bool getSIMDInfo(SIMD enm);
	static int Error(const TCHAR *msg, bool showMsgBox = 0);
	static int setErrorFileName(const TCHAR *name);
private:
	static unsigned int iSIMDFlags; 
	static const TCHAR *tchErrorFile;

	Sys(void);
	static bool isCPUIDPresent();
};

