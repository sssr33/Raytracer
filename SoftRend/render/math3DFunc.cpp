#include "StdAfx.h"
#include "math3DFunc.h"
#include "structures.h"

const float math3DFunc::PI = ((float)3.141592654f);
const float math3DFunc::EPSILON_E3 = (float)(1E-3);

math3DFunc::math3DFunc(void)
{
	// generate the tables 0 - 360 inclusive
	for (int ang = 0; ang <= 360; ang++)
    {
		// convert ang to radians
		float theta = (float)ang*PI/(float)180;

		// insert next entry into table
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);
    } // end for ang
}


math3DFunc::~math3DFunc(void)
{
}

float math3DFunc::Fast_Cos(float theta)
{
	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(cos_look[theta_int] + 
		   theta_frac*(cos_look[theta_int+1] - cos_look[theta_int]));
}
float math3DFunc::Fast_Sin(float theta)
{
	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(sin_look[theta_int] + 
		   theta_frac*(sin_look[theta_int+1] - sin_look[theta_int]));
}

float math3DFunc::Fast_Distance_3D(float fx, float fy, float fz)
{
	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = static_cast<int>(fabs(fx) * 1024.f);
	y = static_cast<int>(fabs(fy) * 1024.f);
	z = static_cast<int>(fabs(fz) * 1024.f);

	// sort values
	if (y < x) 
	{
		temp = x;
		x = y;
		y = temp;
	}

	if (z < y) 
	{
		temp = z;
		z = y;
		y = temp;
	}

	if (y < x)
	{
		temp = x;
		x = y;
		y = temp;
	}

	int dist = (z + 11 * (y >> 5) + (x >> 2) );

	// compute distance with 8% error
	return((float)(dist >> 10));
}