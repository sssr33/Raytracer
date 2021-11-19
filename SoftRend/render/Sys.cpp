// 🐚
#include "StdAfx.h"
#include "Sys.h"

#include <intrin.h>
#include <array>

Sys::Sys(void)
{
}


Sys::~Sys(void)
{
}

unsigned int Sys::iSIMDFlags = 0; 
const TCHAR *Sys::tchErrorFile = TEXT("error.txt");;

int Sys::Initialize()
{
	/*Sys::tchErrorFile 
	Sys::iSIMDFlags = 0;*/

	if(!Sys::isCPUIDPresent())
	{
		Sys::Error(TEXT("CPUID isn't present!"));
		return 0;
	}

	const int num = 8;
	unsigned int* SIMDInfo = new unsigned int[num];
	ZeroMemory(SIMDInfo, sizeof(unsigned int) * num);

	std::array<int, 4> cpui = {};

	__cpuid(cpui.data(), 1);

	constexpr int mmxBit = 1 << 23;
	constexpr int sseBit = 1 << 25;
	constexpr int sse2Bit = 1 << 26;
	constexpr int sse3Bit = 1 << 0;
	constexpr int ssse3Bit = 1 << 9;
	constexpr int ssse4_1Bit = 1 << 19;
	constexpr int ssse4_2Bit = 1 << 20;

	const int& ecx = cpui[2];
	const int& edx = cpui[3];

	SIMDInfo[0] = edx & mmxBit;
	SIMDInfo[1] = edx & sseBit;
	SIMDInfo[2] = edx & sse2Bit;

	SIMDInfo[3] = ecx & sse3Bit;
	SIMDInfo[4] = ecx & ssse3Bit;
	SIMDInfo[5] = ecx & ssse4_1Bit;
	SIMDInfo[6] = ecx & ssse4_2Bit;

	for (int i = 0; i < num; i++)
	{
		if (SIMDInfo[i]) setBit(Sys::iSIMDFlags, i);
	}

	return 1;
}

int Sys::Release()
{
	return 1;
}

bool Sys::isCPUIDPresent()
{
	std::array<int, 4> cpui = {};

	// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-160
	__cpuid(cpui.data(), 0);
	int idCount = cpui[0];

	return idCount != 0;
}

bool Sys::getSIMDInfo(SIMD enm)
{
	return Sys::iSIMDFlags & enm;
}

int Sys::Error(const TCHAR *msg, bool showMsgBox)
{
	return 1;
}
int Sys::setErrorFileName(const TCHAR *name)
{
	return 1;
}