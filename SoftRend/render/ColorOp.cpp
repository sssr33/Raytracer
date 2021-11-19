// 🐚
#include "StdAfx.h"
#include "ColorOp.h"


ColorOp::ColorOp(void)
{
}


ColorOp::~ColorOp(void)
{
}

void ColorOp::ConvertColor(unsigned int from, unsigned int *to, int sourBpp, int destBpp)
{
	UCHAR components[4];

	if(sourBpp != 16 && sourBpp != 24 && sourBpp != 32) return;
	if(destBpp != 16 && destBpp != 24 && destBpp != 32) return;

	switch(sourBpp)
	{
	case 16:
		{
			ColorOp::ColorFrom16bit_565(&components[0], &components[1], &components[2], from);
			components[0] <<= 3;
			components[1] <<= 2;
			components[2] <<= 3;
		}break;
	case 24:
		{
			ColorOp::ColorFrom24bit(&components[0], &components[1], &components[2], from);
		}break;
	case 32:
		{
			ColorOp::ColorFrom32bit(&components[3], &components[0], &components[1], &components[2], from);
		}break;
	};

	switch(destBpp)
	{
	case 16:
		{
			if(sourBpp != 16)
			{
				components[0] >>= 3;
				components[1] >>= 2;
				components[2] >>= 3;
			}

			ColorOp::ColorTo16bit_565(components[0], components[1], components[2], (unsigned short *)to);
		}break;
	case 24:
		{
			ColorOp::ColorTo24bit(components[0], components[1], components[2], to);
		}break;
	case 32:
		{
			ColorOp::ColorTo32bit(255, components[0], components[1], components[2], to);
		}break;
	};
}

void ColorOp::ColorFrom16bit_565(UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color)
{
	*r = (color >> 11) & 31;
	*g = (color >> 5) & 63;
	*b = color & 31;
}
void ColorOp::ColorFrom24bit(UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color)
{
	*r = color >> 16;
	*g = color >> 8;
	*b = color & 255;
}
void ColorOp::ColorFrom32bit(UCHAR *a, UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color)
{
	*a = color >> 24;
	*r = color >> 16;
	*g = color >> 8;
	*b = color & 255;
}

void ColorOp::ColorTo16bit_565(UCHAR r, UCHAR g, UCHAR b, unsigned short *color)
{
	*color = (((unsigned short)r & 31) << 11) | (((unsigned short)g & 63) << 5) | ((unsigned short)b & 31);
}
void ColorOp::ColorTo24bit(UCHAR r, UCHAR g, UCHAR b, unsigned int *color)
{
	*color = ((unsigned int)r << 16) | ((unsigned int)g << 8) | ((unsigned int)b);
}
void ColorOp::ColorTo32bit(UCHAR a, UCHAR r, UCHAR g, UCHAR b, unsigned int *color)
{
	*color = ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | ((unsigned int)b);
}