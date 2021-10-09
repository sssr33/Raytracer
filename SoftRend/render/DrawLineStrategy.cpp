#include "StdAfx.h"
#include "DrawLineStrategy.h"
#include <math.h>
#include <algorithm>

int DrawSimpleLine32BitStrategy::DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){

	int dx, dy, dx2, dy2, x_inc, y_inc, error, index;
	//unsigned int blendedColor;

	lpitch >>= 2;

	unsigned int *vb = (unsigned int*)videoMemory + (x0 + y0*lpitch);

	dx = x1 - x0;
	dy = y1 - y0;

	if(dx >= 0){
		x_inc = 1;
	}
	else {
		dx = -dx;
		x_inc = -1;
	}

	if(dy >= 0){
		y_inc = lpitch;
	}
	else{
		dy = -dy;
		y_inc = -lpitch;
	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if(dx > dy){
		error = dy2 - dx; //устанавливаем ошибку равную половине пикселя. Т.е. сейчас y = y + 0.5
		//error показывает сколько(error/dy) пикселей по x нужно закрасить прежде чем перейти к след. y
		//dx2 в данном случае можно представить как 1.0 по оси Y
		//dy2 как n = dy2 / dx2, т.е. n это дробная часть по оси Y для текущего X
		for(index = 0; index < dx; index++){
			/*blendedColor = MyAlphaBlend7(*vb, color);
			*vb = blendedColor;*/
			alphaBlender->AlphaBlend(vb, &color, 1);
			*vb = color;

			if(error >= 0){
				error -= dx2;

				vb += y_inc;
			}

			error += dy2;
			vb += x_inc;
		}
	}
	else{
		error = dx2 - dy;

		for(index = 0; index < dy; index++){
			/*blendedColor = MyAlphaBlend7(*vb, color);
			*vb = blendedColor;*/
			alphaBlender->AlphaBlend(vb, &color, 1);

			if(error >= 0){
				error -= dy2;

				vb += x_inc;
			}

			error += dx2;
			vb += y_inc;
		}
	}

	return 1;
}

int DrawSoftLineSSE32BitStrategy::DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){

	float fTemp;
	int dx = x1 - x0;
	int dy = y1 - y0;
	int alpha;
	float alphaOrig = static_cast<float>(color >> 24);
	color &= 0x00FFFFFF;
	bool cond = abs(dx) < abs(dy);
	int lpitch2 = lpitch >> 2;
	__declspec(align(16)) unsigned int colors[4] = {color, color, color, color};
	__declspec(align(16)) float fOne[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	__declspec(align(16)) unsigned int resColors[4];
	__declspec(align(16)) unsigned int backColors[4];
	__declspec(align(16)) float interys[2];
	__declspec(align(16)) float gradients[2];
	int aY;
	int bY;
	unsigned int *vb = (unsigned int *)videoMemory;

	if(/*fabs(dx) < fabs(dy)*/cond){
		SWAP(x0, y0, fTemp);
		SWAP(x1, y1, fTemp);
		SWAP(dx, dy, fTemp);
	}
	if(x1 < x0){
		SWAP(x1, x0, fTemp);
		SWAP(y1, y0, fTemp);
	}

	float gradient = (float)dy / (float)dx;

	//first endpoint
	float xend = static_cast<float>(Round(static_cast<float>(x0)));
	float yend = y0 + gradient * (xend - x0);
	float xgap = rfPart(x0 + 0.5f);
	int xpxl1 = static_cast<int>(xend);
	int ypxl1 = iPart(yend);
	//alpha = /*255.0f*/alphaOrig * (rfPart(yend) * xgap);
	//PlotPixel(xpxl1, ypxl1, color | alpha << 24, vb, lpitch);
	//alpha = /*255.0f*/alphaOrig * (fPart(yend) * xgap);
	//PlotPixel(xpxl1, ypxl1 + 1, color | alpha << 24, vb, lpitch);
	float intery = yend + gradient;
	//

	//second endpoint
	xend = static_cast<float>(Round(static_cast<float>(x1)));
	yend = y1 + gradient * (xend - x1);
	xgap = fPart(x1 + 0.5f);
	int xpxl2 = static_cast<int>(xend);
	int ypxl2 = iPart(yend);
	
	if(!cond){
		alpha = static_cast<int>(/*255.0f*/alphaOrig * (rfPart(yend) * xgap));
		//PlotPixel(xpxl1, ypxl1, color | alpha << 24, vb, lpitch);

//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;

		alpha = static_cast<int>(/*255.0f*/alphaOrig * (fPart(yend) * xgap));
		//PlotPixel(xpxl1, ypxl1 + 1, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;

		alpha = static_cast<int>(/*255*/alphaOrig * (rfPart(yend) * xgap));
		//PlotPixel(xpxl2, ypxl2, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;
		alpha = static_cast<int>(/*255*/alphaOrig * (fPart(yend) * xgap));
		//PlotPixel(xpxl2, ypxl2 + 1, color | alpha << 24, vb, lpitch); 
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;
	}
	else{
		alpha = static_cast<int>(/*255.0f*/alphaOrig * (rfPart(yend) * xgap));
		//PlotPixel(ypxl1, xpxl1, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;
		alpha = static_cast<int>(/*255.0f*/alphaOrig * (fPart(yend) * xgap));
		//PlotPixel(ypxl1 + 1, xpxl1, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;

		alpha = static_cast<int>(/*255*/alphaOrig * (rfPart(yend) * xgap));
		//PlotPixel(ypxl2, xpxl2, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;
		alpha = static_cast<int>(/*255*/alphaOrig * (fPart(yend) * xgap));
		//PlotPixel(ypxl2 + 1, xpxl2, color | alpha << 24, vb, lpitch);
//		vb[xpxl1 + ypxl1 * (lpitch >> 2)] = color | alpha << 24;
	}
	//

	gradients[0] = gradient * 2;
	gradients[1] = gradient * 2;
	interys[0] = intery;
	interys[1] = intery + gradient;

	if(!cond){
	for(int x = xpxl1 + 1; x < xpxl2/* - 1*/; x+=2){

		aY = (int)interys[0]  * lpitch2 + x;
		bY = (int)interys[1]  * lpitch2 + x + 1;

		backColors[0] = vb[aY];
		backColors[1] = vb[bY];
		backColors[2] = vb[aY + lpitch2];
		backColors[3] = vb[bY + lpitch2];

		// TODO rewrite with intrinsics and plain C/C++
		//__asm{
		//	movd xmm2, alphaOrig //alphas
		//	movaps xmm5, fOne //fOne
		//	
		//	pshufd xmm2, xmm2, 0
		//	movlps xmm6, interys //interys

		//	//movlps oldInterys, xmm6
		//	movlps xmm7, gradients //gradients
		//	pshufd xmm3, xmm6, 44h//xmm_3 = {intreys[1], interys[0], intreys[1], interys[0]}
		//	
		//	addps xmm6, xmm7 //xmm6 = {interys[1] + gradients[1], interys[0] + gradients[0]}
		//	cvttps2dq xmm1, xmm3
		//	
		//	movlps interys, xmm6 //saving new interys
		//	cvtdq2ps xmm1, xmm1

		//	movaps xmm0, colors
		//	subps xmm3, xmm1//float parts of interys in xmm3

		//	movhlps xmm4, xmm3//xmm3 high part to xmm4 low part
		//	subps xmm5, xmm4//1.0 - interys
		//	movlhps xmm3, xmm5//xmm3 = {1 - intreys[1], 1 - interys[0], intreys[1], interys[0]}
		//	
		//	mulps xmm3, xmm2//xmm3 = {(1 - intreys[1]) * alphaOrig, (1 - interys[0]) * alphaOrig, intreys[1] * alphaOrig, interys[0] * alphaOrig}
		//	cvttps2dq xmm3, xmm3

		//	pslld xmm3, 24

		//	por xmm0,xmm3

		//	movaps resColors, xmm0

		//}

		//MyAlphaBlend9(backColors, resColors);
		alphaBlender->AlphaBlend(backColors, resColors, 4);

		vb[aY] = backColors[2];//_ARGB32BIT(0,255,0,0);
		vb[bY] = backColors[3];//_ARGB32BIT(0,255,0,0);
		vb[aY + lpitch2] = backColors[0];//_ARGB32BIT(0,255,0,0);
		vb[bY + lpitch2] = backColors[1];//_ARGB32BIT(0,255,0,0);
	}
	}
	else {
		for(int x = xpxl1 + 1; x < xpxl2/* - 1*/; x+=2){
		aY = x  * lpitch2 + (int)interys[0];
		bY = (x + 1)  * lpitch2 + (int)interys[1];

		backColors[0] = vb[aY];
		backColors[1] = vb[bY];
		backColors[2] = vb[aY + /*lpitch2*/1];
		backColors[3] = vb[bY + /*lpitch2*/1];

		// TODO rewrite with intrinsics and plain C/C++
		//__asm{
		//	movd xmm2, alphaOrig //alphas
		//	movaps xmm5, fOne //fOne
		//	
		//	pshufd xmm2, xmm2, 0
		//	movlps xmm6, interys //interys

		//	//movlps oldInterys, xmm6
		//	movlps xmm7, gradients //gradients
		//	pshufd xmm3, xmm6, 44h//xmm_3 = {intreys[1], interys[0], intreys[1], interys[0]}
		//	
		//	addps xmm6, xmm7 //xmm6 = {interys[1] + gradients[1], interys[0] + gradients[0]}
		//	cvttps2dq xmm1, xmm3
		//	
		//	movlps interys, xmm6 //saving new interys
		//	cvtdq2ps xmm1, xmm1

		//	movaps xmm0, colors
		//	subps xmm3, xmm1//float parts of interys in xmm3

		//	movhlps xmm4, xmm3//xmm3 high part to xmm4 low part
		//	subps xmm5, xmm4//1.0 - interys
		//	movlhps xmm3, xmm5//xmm3 = {1 - intreys[1], 1 - interys[0], intreys[1], interys[0]}
		//	
		//	mulps xmm3, xmm2//xmm3 = {(1 - intreys[1]) * alphaOrig, (1 - interys[0]) * alphaOrig, intreys[1] * alphaOrig, interys[0] * alphaOrig}
		//	cvttps2dq xmm3, xmm3

		//	pslld xmm3, 24

		//	por xmm0,xmm3

		//	movaps resColors, xmm0

		//}

		alphaBlender->AlphaBlend(backColors, resColors, 4);
		//MyAlphaBlend9(backColors, resColors);

		vb[aY] = backColors[2];//_ARGB32BIT(0,255,0,0);
		vb[bY] = backColors[3];//_ARGB32BIT(0,255,0,0);
		vb[aY + /*lpitch2*/1] = backColors[0];//_ARGB32BIT(0,255,0,0);
		vb[bY + /*lpitch2*/1] = backColors[1];//_ARGB32BIT(0,255,0,0);
	}
	}

	return 1;
}

int DrawSoftLineSSE32BitStrategy::DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender)
{
	int iLineLength = static_cast<int>(x1 - x0);
	unsigned int *vb = (unsigned int *)videoMemory;
	lpitch >>= 2;
	int colorAlpha = color >> 24;
	float t;
	int i;

	__declspec(align(16)) float alphas[4];
	__declspec(align(16)) int sourColors[4];
	__declspec(align(16)) int destColors[4];

	if(iLineLength == 0) return 0;
	if(iLineLength < 0)
	{
		iLineLength = -iLineLength;
		SWAP(x0, x1, t);
	}

	alphas[0] = rfPart(x0) * (float)colorAlpha;
	//alphas[1] = fPart(x0) * (float)colorAlpha;
	//alphas[2] = rfPart(x1) * (float)colorAlpha;
	alphas[3] = fPart(x1) * (float)colorAlpha;

	//if(alphas[0] > alphas[1]) SWAP(alphas[0], alphas[1], t);
	//if(alphas[2] < alphas[3]) SWAP(alphas[2], alphas[3], t);

	for(int i = 0; i < 4; i++)
	{
		//if(alphas[i])
			sourColors[i] = (color & 0x00FFFFFF) | ((unsigned int)alphas[i] << 24);
	}

	destColors[0] = vb[(int)x0 + y * lpitch];
	//destColors[1] = vb[(int)x0 + 1 + y * lpitch];
	//destColors[2] = vb[(int)x1 - 1 + y * lpitch];
	destColors[3] = vb[(int)x1 - 1 + y * lpitch];
	
	alphaBlender->AlphaBlend(destColors, sourColors, 4);

	//TCHAR mms[256];
	//wsprintf(mms, TEXT("alpha: %d\nsourColor: A %d R %d G %d B %d \ndestColor: A %d R %d G %d B %d"), (unsigned int)alphas[3], (sourColors[3] >> 24), (sourColors[3] >> 16) & 0x000000FF, (sourColors[3] >> 8) & 0x000000FF, (sourColors[3]) & 0x000000FF, (destColors[3] >> 24) & 0x000000FF, (destColors[3] >> 16) & 0x000000FF, (destColors[3] >> 8) & 0x000000FF, (destColors[3]) & 0x000000FF);
	//MessageBox(0, mms, 0,0);

	sourColors[0] = sourColors[1] = sourColors[2] = sourColors[3] = color;

	if(colorAlpha < 252)
	{
		for(i = 1; i < iLineLength; i+=4)
		{
			if(iLineLength - i < 4) break;

			alphaBlender->AlphaBlend((vb + (int)x0 + i + y * lpitch), sourColors, 4);
		}

		if(iLineLength - i < 4)
		{
			for(i; i < iLineLength; i++)
			{
				alphaBlender->AlphaBlend((vb + (int)x0 + i + y * lpitch), sourColors, 1);
			}
		}
	}
	else{
		unsigned int *vb2 = vb + (int)x0 + y * lpitch;
		std::fill(vb2, vb2 + iLineLength, color);
	}

	vb[(int)x0 + y * lpitch] = destColors[0];
	//vb[(int)x0 + 1 + y * lpitch] = destColors[1];
	//vb[(int)x1 - 1 + y * lpitch] = destColors[2];
	vb[(int)x1 - 1 + y * lpitch] = destColors[3];

	return 1;
}

int DrawSoftLineSSE32BitStrategy::DrawHGradientLine(float x0, float x1, int y, unsigned int color,float startGrad, float endGrad, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender)
{
	int iLineLength = static_cast<int>(x1 - x0);
	unsigned int *vb = (unsigned int *)videoMemory;
	lpitch >>= 2;
	float colorAlpha = static_cast<float>(color >> 24);
	float t;
	int i;

	__declspec(align(16)) int sourColors[4];
	float gradXend = 0.0f, gradDec = 0.0f;
	float gradXst = 0.0f, gradInc = 0.0f;
	int iAdd = 0;
	int iAdd2 = 0;
	float iAdd3 = 0.0f;

	if(x0 + startGrad > x1) return 0;
	if(iLineLength == 0) return 0;
	if(iLineLength < 0)
	{
		iLineLength = -iLineLength;
		SWAP(x0, x1, t);
	}

	if(endGrad < 1.0f)
	{
		gradXend = fPart(x1);
		gradDec = 0.0f;
		endGrad = 1.0f;
	}
	else
	{
		endGrad+=2;
		endGrad = static_cast<float>((int)endGrad);
		x1 = static_cast<float>((int)x1);
		gradXend = 1.0f;
		gradDec = 1.0f / endGrad;
		//iAdd = 1;
	}
	if(startGrad < 1.0f)
	{
		gradXst = rfPart(x0);
		gradInc = 0.0f;
		startGrad = 1.0f;
	}
	else
	{
		startGrad +=2;
		startGrad = static_cast<float>((int)startGrad);
		//x0 += 0.5f;
		gradXst = 0.0f;
		gradInc = 1.0f / startGrad;
		iAdd2 = 1;
	}

	if(x0 + startGrad > x1 - endGrad) return 0;
	/*{
		if(startGrad < endGrad)
		{
		 
		}
		else return 0;
	}*/

	for(i = static_cast<int>(x0); i < (int)(x0 + startGrad); i++)
	{
		sourColors[0] = (color & 0x00FFFFFF) | ((int)(colorAlpha * gradXst) << 24);
		alphaBlender->AlphaBlend((vb + i + y * lpitch), sourColors, 1);

		gradXst += gradInc;
	}

	sourColors[0] = sourColors[1] = sourColors[2] = sourColors[3] = color;

	for(i = static_cast<int>(x0 + startGrad); i < (int)(x1 - endGrad); i++)
	{
		alphaBlender->AlphaBlend((vb + i + y * lpitch), sourColors, 1);
	}

	for(i = static_cast<int>(x1 - endGrad); i < (int)x1; i++)
	{
		sourColors[0] = (color & 0x00FFFFFF) | ((int)(colorAlpha * gradXend) << 24);
		alphaBlender->AlphaBlend((vb + i + y * lpitch), sourColors, 1);

		gradXend -= gradDec;
	}

	return 1;
}

int DrawSoftLine32BitStrategy::DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){
	return 1;
}


int DrawSoftLineSSE32BitStrategy::DrawHGradientLine2(float startGrad, float endGrad, float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender)
{
	lpitch >>= 2;
	/*x1 += 0.5f;
	x0 -= 0.5f;*/
	unsigned int *vb2 = (unsigned int *)videoMemory + (unsigned int)x0 + y * lpitch;
	int iLineLength = (unsigned int)((int)x1 - (int)x0 + 1);

	if(iLineLength <= 0) return 0;

	std::fill(vb2, vb2 + iLineLength, color);

	return 1;
}