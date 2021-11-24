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
	static Sys& Instance();

	static bool getSIMDInfo(SIMD enm);

	int Error(const TCHAR *msg, bool showMsgBox = 0);
	int setErrorFileName(const TCHAR *name);
private:
	unsigned int iSIMDFlags = 0;
	const TCHAR *tchErrorFile = TEXT("error.txt");

	Sys();
	static bool isCPUIDPresent();
};
