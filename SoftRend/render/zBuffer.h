#pragma once
#include <cfloat>
#include "stdafx.h"

class zBuffer
{
public:
	zBuffer(void);
	~zBuffer(void);

	int attr;
	float *zbuffer;
	int width;
	int height;
	int count;
	float z_add;

	int initialize(int width, int height, int attr = 0);
	int release();

	int clear();
};

