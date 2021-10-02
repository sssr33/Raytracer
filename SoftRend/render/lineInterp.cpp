#include "lineInterp.h"


lineInterp::lineInterp(void)
{
}


lineInterp::~lineInterp(void)
{
}

void lineInterp::init(POINT4D_PTR p1, POINT4D_PTR p2, bool isYSorted)
{
	if(!isYSorted)
	{
		if(p1->y > p2->y)
		{
			vecCopy(p1, &this->p2);
			vecCopy(p2, &this->p1);
		}
		else
		{
			vecCopy(p1, &this->p1);
			vecCopy(p2, &this->p2);
		}
	}
	else
	{
		vecCopy(p1, &this->p1);
		vecCopy(p2, &this->p2);
	}

	float dxdy = 0;
	float y_correction = 0;
	bool xinv = false;
	float yc1, yc2;

	yc1 = ceil(this->p1.y);
	yc2 = ceil(this->p2.y) - 1.0f;

	this->height = (yc2 - yc1);
	this->heightF = this->p2.y - this->p1.y;

	this->p1.x = ceil(this->p1.x);
	this->p2.x = ceil(this->p2.x);

	//if(this->p1.x > this->p2.x)
	//{
	//	dxdy = (this->p1.x - this->p2.x) / (this->p2.y - this->p1.y);

	//	y_correction = this->p2.y - yc2;
	//	this->p2.x = this->p2.x + dxdy * y_correction;

	//	y_correction = yc1 - this->p1.y;
	//	this->p1.x = this->p1.x - dxdy * y_correction;
	//	/*y_correction = height;
	//	this->p1.x = this->p2.x + dxdy * y_correction;*/
	//}
	//else
	//{
	//	dxdy = (this->p2.x - this->p1.x) / (this->p2.y - this->p1.y);
	//	xinv = true;

	//	y_correction = yc1 - this->p1.y;
	//	this->p1.x = this->p1.x + dxdy * y_correction;

	//	y_correction = this->p2.y - yc2;
	//	this->p2.x = this->p2.x - dxdy * y_correction;
	//	/*y_correction = height;
	//	this->p2.x = this->p1.x + dxdy * y_correction;*/
	//}

	this->p1.y = yc1;
	this->p2.y = yc2;
}

float lineInterp::getX(float y_idx)
{
	float x = 0;
	float t = 0;

	if(height > 0)
	{
		currT = t = y_idx / height;

		x = (t * this->p2.x) + ((1.0f - t) * this->p1.x);
	}
	else
		x = this->p1.x;

	/*TCHAR mas[128];
	swprintf(mas, L"%f > %f || %f > %f || %f || %f", this->p1.x, this->p2.x, this->p1.y, this->p2.y, t, x);
	MessageBox(0, mas, 0,0);*/

	return x;
}