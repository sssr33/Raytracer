#pragma once
class ColorOp
{
public:
	ColorOp(void);
	~ColorOp(void);

	static void ConvertColor(unsigned int from, unsigned int *to, int sourBpp, int destBpp);

	static void ColorFrom16bit_565(UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color);
	static void ColorFrom24bit(UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color);
	static void ColorFrom32bit(UCHAR *a, UCHAR *r, UCHAR *g, UCHAR *b, unsigned int color);

	static void ColorTo16bit_565(UCHAR r, UCHAR g, UCHAR b, unsigned short *color);
	static void ColorTo24bit(UCHAR r, UCHAR g, UCHAR b, unsigned int *color);
	static void ColorTo32bit(UCHAR a, UCHAR r, UCHAR g, UCHAR b, unsigned int *color);
};

