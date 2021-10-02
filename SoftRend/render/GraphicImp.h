#pragma once
#include <ddraw.h>
#include "FlipStrategy.h"
#include "DrawStrategy.h"
#include "Pipeline.h"
#include "RenderState.h"
#include <vector>
#include <iostream>
#include <limits>

#include <wx/file.h>

using namespace std;

#define _ARGB32BIT(a,r,g,b) (b + (g << 8) + (r << 16) + (a << 24))
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

struct _screenParams{
		unsigned int iWidth;
		unsigned int iHeight;
		unsigned int iBitsPerPixel;
		unsigned int iGraphicQuality;
		bool iFullScreen;
		//unsigned int iParams[4];
};

class GraphicImp
{
public:
	int idx, idx2;
	
	virtual int Initialize(_screenParams *params, HWND hWnd) = 0;
	virtual void Release() = 0;
	virtual int Resize(int newWidth, int newHeight) = 0;
	~GraphicImp(void) {}

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
protected:
	GraphicImp(void) {}
};

class GraphicImpDirectDraw : public GraphicImp{
public:
	GraphicImpDirectDraw();
	int Initialize(_screenParams *params, HWND hWnd);
	int Resize(int newWidth, int newHeight);

	int DrawBegin(bool bClearScreen);
	int DrawEnd();

	int DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color);
	int DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color);

	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color);
	int DrawHLine(float x1, float x2, int y, unsigned int iColor);
	int DrawRect(RECT *r, unsigned int iColor);
	int DrawText(TCHAR *tchTextString, int iStrLen, int iX, int iY, unsigned int iColor);
	int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int iColor);
	int DrawPOLYF4D2D(struct3D::POLYF4D_PTR face);
	LIGHT_PTR getLight(int index);
	int DrawTexture(mat::TEXTURE2D_PTR tex, RECT *rect);
	int DrawTexTri(struct3D::POLYF4D_PTR face);

	int DrawCircle(POINT2D *ptCenter, unsigned int iRadius, unsigned int iColor, bool bSmooth);
	int DrawSmoothLine(POINT2D *pt0, POINT2D *pt1, unsigned int iColor);
	int DrawSpline(std::vector<POINT2D> *pts);

	int DrawPolygon(std::vector<POINT2D> *pts, unsigned int iColor);

	//3D functions:
	void DrawOBJECT4DWire(OBJECT4D_PTR obj);
	void DrawRENDERLIST4DWire(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos);
	void DrawOBJECT4DSolid(OBJECT4D_PTR obj);
	void DrawRENDERLIST4DSolid(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos);
	void DrawOBJECT4DLINE(OBJECT4D_LINE_PTR obj);
	CAM4D_PTR getMainCamera();
	bool isOBJECT4DCulled(OBJECT4D_PTR obj);

	int setRenderState(RendState enm, int value);
	int AddLight(int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf);
	
	int point2DTo3D(math3D::POINT2D_PTR pt2D, math3D::POINT4D_PTR pt3D);
	int point2DToVector3D(math3D::POINT2D_PTR pt2D, math3D::VECTOR4D_PTR pt3D);

	int createShadowProjection(OBJECT4D_PTR obj, POLYF4D_PTR proj, int numLines, int *createdLines);
	//3D functions.

	//obj3D functions
	POLYF4D_PTR getSelectedPoly(int x, int y);
	POLYF4D_PTR *ppObj3DMap;
	//obj3D functions

	void clearZBuffer();
	void testAdj(POINT2D_PTR pt, OBJECT4D_PTR obj);

	void Release();
private:
	LPDIRECTDRAW7 _lpdd7;
	LPDIRECTDRAWSURFACE7 _lpddsurf;//main surface
	LPDIRECTDRAWSURFACE7 _lpddbacksurf;//back buffer surface
	LPDIRECTDRAWCLIPPER _lpddclip;

	DDSURFACEDESC2 _ddsd;
	DDBLTFX _ddblt;

	HDC _HDC;
	HWND _hWnd;
	void *_videoBuffer;
	int _lPitch;
	bool isBufferLocked;
	_screenParams _sp;

	FlipDDStrategy *_flipStrategy;
	DrawStrategy *_draw;
	RenderState state;

	int InitializeDrawStrategies(_screenParams *params);
	int ClipLine( int &x1, int &y1, int &x2, int &y2, _screenParams *params);

	void ClearSurface(LPDIRECTDRAWSURFACE7 _lpSurf);
	/*int CopyOnBackSurface(LPDIRECTDRAWSURFACE7 _lpSurf);

	void DrawBlur(_screenParams *params);
	void DrawBlur2(_screenParams *params);
	void DrawBlurSSE2(_screenParams *params);
	void DrawBlurSSE3(_screenParams *params);*/

	//3D Pipeline:
	Pipeline pipe;
	CAM4D mainCam;

	//obj3DMap
	
	bool isInPoly(POINT4D_PTR p, VECTOR4D_PTR v, POLY4D_PTR poly, OBJECT4D_PTR obj);
};

