#pragma once
#include "DrawLineStrategy.h"
#include "structures.h"
#include "Sys.h"
#include "zBuffer.h"
#include "triInterpolation.h"
#include "triInterpIter.h"
#include "fixp\fixed_class.h"
#include "fixp64.h"

using namespace fixedpoint;
/*DrawPhongTriangle хорошая основа для пиксельного шейдера. Т.е. в месте где выполняеться расчет освещения может быть шейдер.*/

class DrawStrategy
{
public:
	struct DrawTriangleDefaultParams {
		struct3D::POLYF4D_PTR face = nullptr;
		unsigned int* videoMemory = nullptr;
		uint32_t videoMemoryWidth = 0;
		uint32_t videoMemoryHeight = 0;
		uint32_t videoMemoryPitch = 0;
		int polyIdx = -1;
	};

	virtual ~DrawStrategy(void);
	virtual int Release() = 0;
	virtual int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch) = 0;
	virtual int DrawLine(VERTEX4DT_PTR v0, VERTEX4DT_PTR v1, unsigned int color, void *videoMemory, int lpitch) = 0;
	//virtual int DrawPixel(int x, int y, unsigned int color, void *videoMemory, int lpitch) = 0;
	virtual int DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color, void *videoMemory, int lpitch) = 0;
	virtual int DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color, void *videoMemory, int lpitch) = 0;//используються координаты с плавающей точкой
	virtual int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch) = 0;
	virtual int DrawRect(RECT *r, unsigned int color, void *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawGouraudTriangle(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawGouraudTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawGouraudTriangle(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int V0color, unsigned int V1color, unsigned int V2color, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawPhongTriangle(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;//Требуеться система освещения(mat::LightSystem *lights) чтобы вычислить интенсивность освещения к каждому пикселю
	virtual int DrawPhongTriangle2(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawGouraudTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;

	virtual int DrawTriangleOriginal(struct3D::POLYF4D_PTR face, unsigned int* videoMemory, int lpitch) { return 0; }
	virtual int DrawTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle4(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle5(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle6(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	virtual int DrawTriangle7_sse(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;

	virtual void DrawTriangleDefault(const DrawTriangleDefaultParams& params) {};

	virtual int clipLineCS(RECT *clippingRect, POINT4D *pt1, POINT4D *pt2) = 0;

	//textured triangles
	virtual int DrawTriangleTex(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch) = 0;
	//textured triangles
	
	virtual int setScreenSize(int width, int height) = 0;
	virtual int setClipBorders(float minClipX, float minClipY, float maxClipX, float maxClipY) = 0;
	virtual int setObj3DMap(struct3D::POLYF4D_PTR *ppMap, unsigned int width, unsigned int height) = 0;

	virtual int clearZBuffer() = 0;

	virtual int FullScreenShader(unsigned int *videoMemory, int lpitch) = 0;
protected:
	DrawStrategy(void);
private:
};

class Draw32BitStrategy : public DrawStrategy
{
public:
	~Draw32BitStrategy();
	Draw32BitStrategy(unsigned int graphicQuality);
	int Release();
	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch);
	int DrawLine(VERTEX4DT_PTR v0, VERTEX4DT_PTR v1, unsigned int color, void *videoMemory, int lpitch);
	int DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color, void *videoMemory, int lpitch);
	int DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color, void *videoMemory, int lpitch);
	int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch);
	int DrawRect(RECT *r, unsigned int color, void *videoMemory, int lpitch);
	int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch);
	int DrawGouraudTriangle(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int V0color, unsigned int V1color, unsigned int V2color, unsigned int *videoMemory, int lpitch);
	int DrawPhongTriangle(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawPhongTriangle2(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawGouraudTriangle(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawGouraudTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawGouraudTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);

	int DrawTriangleOriginal(struct3D::POLYF4D_PTR face, unsigned int* videoMemory, int lpitch) override;
	int DrawTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawTriangle4(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawTriangle5(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawTriangle6(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	int DrawTriangle7_sse(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);

	void DrawTriangleDefault(const DrawTriangleDefaultParams& params) override;
	
	int clipLineCS(RECT *clippingRect, POINT4D *pt1, POINT4D *pt2);

	//textured triangles
	int DrawTriangleTex(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch);
	//textured triangles
	
	int setClipBorders(float minClipX, float minClipY, float maxClipX, float maxClipY);
	int setObj3DMap(struct3D::POLYF4D_PTR *ppMap, unsigned int width, unsigned int height);

	int setScreenSize(int width, int height);

	int clearZBuffer();

	int FullScreenShader(unsigned int *videoMemory, int lpitch);
private:
	struct RoundedRange
	{
		int start;
		int end;
	};

	AlphaBlendStrategy *_alphaBlender;
	DrawLineStrategy *_drawLine;
	zBuffer zbuffer;

	float maxClipX;
	float maxClipY;
	float minClipX;
	float minClipY;
	RECT clipRt;

	int scrWidth;
	int scrHeight;

	void DrawTopTri(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *vb, int lpitch);
	void DrawBottomTri(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *vb, int lpitch);

	void DrawTopTriDefault(float xTop, float yTop, float xBottom1, float yBottom1, float xBottom2, float yBottom, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawBottomTriDefault(float xBottom, float yBottom, float xTop1, float yTop1, float xTop2, float yTop, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawHLineDefault(float leftX, float rightX, float topY, float bottomY, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawTopTriDefault2(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawBottomTriDefault2(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawHLineDefault2(float leftX, float rightX, float topY, float bottomY, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawTopTriDefault3(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawBottomTriDefault3(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	void DrawHLineDefault3(float leftX, float rightX, float topY, float bottomY, unsigned int color, unsigned int* vb, uint32_t lpitch, int polyIdx);
	// half plane
	void DrawTriDefault4(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	// edge scan + multiplication for X
	void DrawTriDefault5(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	void DrawTriDefault5Top(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	void DrawTriDefault5Bottom(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	void DrawTriDefault5SortY(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3) const;
	bool DrawTriDefault5IsOutOfScreen(float x1, float y1, float x2, float y2, float x3, float y3, const DrawTriangleDefaultParams& params) const;
	// edge scan + addition instead of multiplication for X
	void DrawTriDefault6(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	void DrawTriDefault6Top(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	void DrawTriDefault6Bottom(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, const DrawTriangleDefaultParams& params);
	//void DrawGradHLine(float x0, float x1, int y, unsigned int color,int startGrad, int endGrad, void *vb, int lpitch);

	float ClampScreenX(float x) const;
	float ClampScreenY(float y) const;

	static RoundedRange RoundRange(float start, float end);
	static double lerpd(double t, double start, double end);
	static float lerpf(float t, float start, float end);
	static float clamp(float x, float min, float max);

	//obj3dmap
	struct3D::POLYF4D_PTR *ppObjMap;
	unsigned int mapWidth;
	unsigned int mapHeight;
	//obj3dmap

	triInterpolation triInterp;
	triInterpIter triInterpI;
};

class Draw16BitStrategy : public DrawStrategy
{
public:
	Draw16BitStrategy(unsigned int graphicQuality){}
	int Release(){return 0;}
	int DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawLine(VERTEX4DT_PTR v0, VERTEX4DT_PTR v1, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawRect(RECT *r, unsigned int color, void *videoMemory, int lpitch){return 0;}
	int DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawGouraudTriangle(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawGouraudTriangle(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int V0color, unsigned int V1color, unsigned int V2color, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawPhongTriangle(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawPhongTriangle2(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawGouraudTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawGouraudTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}

	int DrawTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawTriangle4(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawTriangle5(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawTriangle6(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	int DrawTriangle7_sse(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}

	int clipLineCS(RECT *clippingRect, POINT4D *pt1, POINT4D *pt2){return 0;}

	//textured triangles
	int DrawTriangleTex(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch){return 0;}
	//textured triangles
	
	int setClipBorders(float minClipX, float minClipY, float maxClipX, float maxClipY){return 0;}
	int setObj3DMap(struct3D::POLYF4D_PTR *ppMap, unsigned int width, unsigned int height){return 0;}
	int setScreenSize(int width, int height) {return 0;}

	int clearZBuffer() {return 0;}

	int FullScreenShader(unsigned int *videoMemory, int lpitch){return 0;}
private:
	AlphaBlendStrategy *_alphaBlender;
	DrawLineStrategy *_drawLine;
};

