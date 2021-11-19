// 🐚
#include "fixp64.h"

__int64 fixp64::double2fix(double d)
{
	__int64 p = 0x100000000;

	return (__int64)(d * p);
}

double fixp64::fix2double(__int64 d)
{
	__int64 p = 0x100000000;

	return (double)d / p;
}