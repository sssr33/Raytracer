#pragma once
#include "AlphaBlendStrategy.h"

class DrawLineStrategy{
public:
	virtual int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender) = 0;
	virtual int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender) = 0;
	virtual int DrawHGradientLine2(float startGrad, float endGrad, float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender) = 0;
	virtual int DrawHGradientLine(float x0, float x1, int y, unsigned int color,float startGrad, float endGrad, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender) = 0;
protected:
	DrawLineStrategy() {}
};

class DrawSimpleLine32BitStrategy : public DrawLineStrategy{
public:
	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
	int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
	int DrawHGradientLine(float x0, float x1, int y, unsigned int color,float startGrad, float endGrad, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
	int DrawHGradientLine2(float startGrad, float endGrad, float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
};

class DrawSoftLineSSE32BitStrategy : public DrawLineStrategy{
public:
	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
	int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
	int DrawHGradientLine(float x0, float x1, int y, unsigned int color,float startGrad, float endGrad, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
	int DrawHGradientLine2(float startGrad, float endGrad, float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
private:
	int iPart(float x){
	return (int)x;
	}
	int Round(float x){
		return iPart(x + 0.5f);
	}
	float fPart(float x){
		return x - (int)x;
	}
	float rfPart(float x){
		return 1 - fPart(x);
	}
};

class DrawSoftLine32BitStrategy : public DrawLineStrategy{
public:
	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender);
	int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
	int DrawHGradientLine(float x0, float x1, int y, unsigned int color,float startGrad, float endGrad, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
	int DrawHGradientLine2(float startGrad, float endGrad, float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch, AlphaBlendStrategy *alphaBlender){return 0;}
};