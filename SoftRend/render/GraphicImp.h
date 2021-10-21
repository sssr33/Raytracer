#pragma once
#include <ddraw.h>
#include "FlipStrategy.h"
#include "DrawStrategy.h"
#include "Pipeline.h"
#include "RenderState.h"
#include <vector>
#include <iostream>
#include <limits>

#ifdef DrawText
#undef DrawText
#endif

using namespace std;

#define _ARGB32BIT(a,r,g,b) (b + (g << 8) + (r << 16) + (a << 24))
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

class GraphicImp
{
public:
	virtual ~GraphicImp(void) = default;

	virtual void Release() = 0;
	virtual int Resize(int newWidth, int newHeight) = 0;

	virtual int DrawBegin(bool bClearScreen) = 0;
	virtual int DrawEnd() = 0;

	virtual int DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color) = 0;
	virtual int DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color) = 0;

	virtual int DrawLine(int x0, int y0, int x1, int y1, unsigned int color) = 0;
	virtual int DrawHLine(float x1, float x2, int y, unsigned int iColor) = 0;
	virtual int DrawRect(RECT *r, unsigned int iColor) = 0;
	virtual int DrawText(TCHAR *tchTextString, int iStrLen, int iX, int iY, unsigned int iColor) = 0;
	virtual int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int iColor) = 0;
	virtual int DrawPOLYF4D2D(struct3D::POLYF4D_PTR face) = 0;
	virtual LIGHT_PTR getLight(int index) = 0;
	virtual int DrawTexture(mat::TEXTURE2D_PTR tex, RECT *rect) = 0;
	virtual int DrawTexTri(struct3D::POLYF4D_PTR face) = 0;

	virtual int DrawCircle(POINT2D *ptCenter, unsigned int iRadius, unsigned int iColor, bool bSmooth) = 0;
	virtual int DrawSmoothLine(POINT2D *pt0, POINT2D *pt1, unsigned int iColor) = 0;
	virtual int DrawSpline(std::vector<POINT2D> *pts) = 0;

	virtual int DrawPolygon(std::vector<POINT2D> *pts, unsigned int iColor) = 0;

	//3D functions:
	virtual void DrawOBJECT4DWire(OBJECT4D_PTR obj) = 0;
	virtual void DrawRENDERLIST4DWire(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos) = 0;
	virtual void DrawOBJECT4DSolid(OBJECT4D_PTR obj) = 0;
	virtual void DrawRENDERLIST4DSolid(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos) = 0;
	virtual void DrawOBJECT4DLINE(OBJECT4D_LINE_PTR obj) = 0;
	virtual CAM4D_PTR getMainCamera() = 0;
	virtual bool isOBJECT4DCulled(OBJECT4D_PTR obj) = 0;

	virtual int setRenderState(RendState enm, int value) = 0;
	virtual int AddLight(int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf) = 0;

	virtual int point2DTo3D(math3D::POINT2D_PTR pt2D, math3D::POINT4D_PTR pt3D) = 0;
	virtual int point2DToVector3D(math3D::POINT2D_PTR pt2D, math3D::VECTOR4D_PTR pt3D) = 0;

	virtual int createShadowProjection(OBJECT4D_PTR obj, POLYF4D_PTR proj, int numLines, int *createdLines) = 0;
	//3D functions.

	//obj3D functions
	virtual POLYF4D_PTR getSelectedPoly(int x, int y) = 0;
	//obj3D functions

	virtual void clearZBuffer() = 0;
	virtual void testAdj(POINT2D_PTR pt, OBJECT4D_PTR obj) = 0;
};
