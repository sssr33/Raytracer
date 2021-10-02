#include "StdAfx.h"
#include "Sys.h"


Sys::Sys(void)
{
}


Sys::~Sys(void)
{
}

unsigned int Sys::iSIMDFlags = 0; 
TCHAR *Sys::tchErrorFile = TEXT("\error.txt");;

int Sys::Initialize()
{
	/*Sys::tchErrorFile 
	Sys::iSIMDFlags = 0;*/

	if(!Sys::isCPUIDPresent())
	{
		Sys::Error(TEXT("CPUID isn't present!"));
	}
	else
	{
		const int num = 8;
		unsigned int *SIMDInfo = new unsigned int[num];
		ZeroMemory(SIMDInfo, sizeof(unsigned int)  * num);

		__asm{
			mov eax, 01h
			cpuid
			mov edi, SIMDInfo

			//MMX detect
			add edi, 4
			//MMX detect

			mov eax, edx
			and eax, 2000000h
			mov [edi], eax //�������� �������� SSE1
			add edi, 4

			mov eax, edx
			and eax, 4000000h
			mov [edi], eax //�������� ��������� SSE2
			add edi, 4

			mov eax, ecx
			and eax, 1h
			mov [edi], eax //�������� ��������� SSE3
			add edi, 4

			mov eax, ecx
			and eax, 200h
			mov [edi], eax //�������� ��������� SSSE3
			add edi, 4

			mov eax, ecx
			and eax, 80000h
			mov [edi], eax //�������� ��������� SSE4.1
			add edi, 4

			mov eax, ecx
			and eax, 100000h
			mov [edi], eax //�������� ��������� SSE4.2
		}
		
		for(int i = 0; i < num; i++)
		{
			if(SIMDInfo[i]) setBit(Sys::iSIMDFlags, i); 
		}
	}
}

int Sys::Release()
{
	return 1;
}

bool Sys::isCPUIDPresent()
{
	bool result = 0;

	__asm{
		pushfd           //; ���������� �������� EFLAGS � �����
		pop eax          //; ���������� �������� EFLAGS � EAX
		mov ebx, eax     //; ���������� �������� � EBX
		xor eax, 200000h //; ��������� 21-�� ����
		push eax         //; ���������� ������ �������� � �����
		popfd            //; ���������� ������ �������� � EFLAGS
		pushfd           //; ����� ���������� EFLAGS � �����
		pop eax          //; �������� EFLAGS ������ � EAX
		xor eax, ebx
		je no_cpuid
		mov result, 1h
	no_cpuid:
	}

	return result;
}

bool Sys::getSIMDInfo(SIMD enm)
{
	return Sys::iSIMDFlags & enm;
}

int Sys::Error(TCHAR *msg, bool showMsgBox)
{
	return 1;
}
int Sys::setErrorFileName(TCHAR *name)
{
	return 1;
}