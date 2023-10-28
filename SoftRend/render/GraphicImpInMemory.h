#pragma once
#include "GraphicImp.h"

#include <vector>
#include <memory>
#include <cstdint>

struct DstFlipParams
{
	void* dstMemory = nullptr;
	uint32_t dstMemoryByteWidth = 0;
	uint32_t width = 0;
	uint32_t height = 0;
};

struct ScreenParamsInMemory {
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t bitsPerPixel = 0;
	uint32_t graphicQuality = 0;
};

class GraphicImpInMemory : public GraphicImp
{
public:
	void Initialize(const ScreenParamsInMemory& screenParams, const DstFlipParams& dstFlipParams);
	void SetDstFlipParams(const DstFlipParams& dstFlipParams);

	void Release() override;
	int Resize(int newWidth, int newHeight) override;

	int DrawBegin(bool bClearScreen) override;
	int DrawEnd() override;

	int DrawPixel(math3D::POINT2DI* pts, int numPts, unsigned int color) override;
	int DrawPixel(math3D::POINT2D* pts, int numPts, unsigned int color) override;

	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color) override;
	int DrawHLine(float x1, float x2, int y, unsigned int iColor) override;
	int DrawRect(RECT* r, unsigned int iColor) override;
	int DrawText(TCHAR* tchTextString, int iStrLen, int iX, int iY, unsigned int iColor) override;
	int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int iColor) override;
	int DrawPOLYF4D2D(struct3D::POLYF4D_PTR face) override;
	LIGHT_PTR getLight(int index) override;
	int DrawTexture(mat::TEXTURE2D_PTR tex, RECT* rect) override;
	int DrawTexTri(struct3D::POLYF4D_PTR face) override;

	int DrawCircle(POINT2D* ptCenter, unsigned int iRadius, unsigned int iColor, bool bSmooth) override;
	int DrawSmoothLine(POINT2D* pt0, POINT2D* pt1, unsigned int iColor) override;
	int DrawSpline(std::vector<POINT2D>* pts) override;

	int DrawPolygon(std::vector<POINT2D>* pts, unsigned int iColor) override;

	//3D functions:
	void DrawOBJECT4DWire(OBJECT4D_PTR obj) override;
	void DrawRENDERLIST4DWire(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos) override;
	void DrawOBJECT4DSolid(OBJECT4D_PTR obj) override;
	void DrawRENDERLIST4DSolid(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos) override;
	void DrawOBJECT4DLINE(OBJECT4D_LINE_PTR obj) override;
	CAM4D_PTR getMainCamera() override;
	bool isOBJECT4DCulled(OBJECT4D_PTR obj) override;

	int setRenderState(RendState enm, int value) override;
	int AddLight(int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf) override;

	int point2DTo3D(math3D::POINT2D_PTR pt2D, math3D::POINT4D_PTR pt3D) override;
	int point2DToVector3D(math3D::POINT2D_PTR pt2D, math3D::VECTOR4D_PTR pt3D) override;

	int createShadowProjection(OBJECT4D_PTR obj, POLYF4D_PTR proj, int numLines, int* createdLines) override;
	//3D functions.

	//obj3D functions
	POLYF4D_PTR getSelectedPoly(int x, int y) override;
	//obj3D functions

	void clearZBuffer() override;
	void testAdj(POINT2D_PTR pt, OBJECT4D_PTR obj) override;

	int idx = 0, idx2 = 0;
	POLYF4D_PTR* ppObj3DMap = nullptr;

private:
	int InitializeDrawStrategies();
	int ClipLine(int& x1, int& y1, int& x2, int& y2);

	void FlipVideoBuffer();
	bool isInPoly(POINT4D_PTR p, VECTOR4D_PTR v, POLY4D_PTR poly, OBJECT4D_PTR obj);
	static float factorial(int num);

	uint8_t* videoBuffer = nullptr;
	uint32_t videoBufferPitch = 0;
	size_t videoBufferSize = 0;

	ScreenParamsInMemory screenParams;
	DstFlipParams dstFlipParams;

	std::unique_ptr<DrawStrategy> draw;
	RenderState state = {};

	//3D Pipeline:
	Pipeline pipe = {};
	CAM4D mainCam = {};
};
