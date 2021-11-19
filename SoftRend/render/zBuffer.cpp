// 🐚
#include "zBuffer.h"

#include <algorithm>

zBuffer::zBuffer(void)
{
	this->zbuffer = 0;
}


zBuffer::~zBuffer(void)
{
	this->release();
}

int zBuffer::initialize(int width, int height, int attr)
{
	this->width = width;
	this->height = height;
	this->attr = attr;
	this->count = width * height;
	z_add = 0;//1001;

	this->zbuffer = new float[this->count];

	this->clear();

	return 1;
}
int zBuffer::clear()
{
	/*if(z_add < 1000)
	{
		z_add++;
		return 1;
	}*/

	float data = 0;//FLT_MAX;//
	int count = this->count;
	float *dest = this->zbuffer;

	//MessageBox(0,0,0,0);

	std::fill(dest, dest + count, data);

	z_add = 0;

	return 1;
}

int zBuffer::release()
{
	if(this->zbuffer)
	{
		delete[] this->zbuffer;
		this->zbuffer = 0;
	}

	return 1;
}
