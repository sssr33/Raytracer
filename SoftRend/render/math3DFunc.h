#pragma once
#include <math.h>
#include "stdafx.h"

class math3DFunc
{
private:
	float sin_look[361];
	float cos_look[361];

	static const float PI;
	static const float EPSILON_E3;
public:
	math3DFunc(void);
	~math3DFunc(void);

	float Fast_Cos(float theta);
	float Fast_Sin(float theta);
	float Fast_Distance_3D(float fx, float fy, float fz);

	inline float DEG_TO_RAD(float ang) 
	{ 
		return ((ang)*PI/180.0);
	}
	inline float RAD_TO_DEG(float rads) 
	{
		return ((rads)*180.0/PI);
	}
	inline int RAND_RANGE(int x, int y)
	{
		return ( (x) + (rand()%((y)-(x)+1)));
	}
	inline bool FCMP(float a, float b)
	{
		return (fabs(a - b) < this->EPSILON_E3);
	}
};

