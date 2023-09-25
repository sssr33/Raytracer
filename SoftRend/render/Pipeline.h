#pragma once
#include "structures.h"
#include "Math3DStructs.h"
#include "DebugLayer\DebugLayer.h"

#include <optional>

using namespace struct3D;
using namespace math3D;
using namespace mat;

//enum coordSelect{
//	TRANSFORM_LOCAL_ONLY, TRANSFORM_TRANS_ONLY, TRANSFORM_LOCAL_TO_TRANS
//};

enum cullObject{
	CULL_OBJECT_X_PLANE = 0x0001, CULL_OBJECT_Y_PLANE = 0x0002, CULL_OBJECT_Z_PLANE = 0x0004, CULL_OBJECT_XYZ_PLANES = CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE
};

enum clipPoly
{
	CLIP_POLY_X_PLANE = 0x0001, CLIP_POLY_Y_PLANE = 0x0002, CLIP_POLY_Z_PLANE = 0x0004, CLIP_POLY_XYZ_PLANES = CLIP_POLY_X_PLANE | CLIP_POLY_Y_PLANE | CLIP_POLY_Z_PLANE
};

enum sortMethod
{
	SORT_POLYLIST_AVZG
};

class Pipeline
{
private:
	static int compareAvgZPOLYF4D(const void *arg1, const void *arg2);
	//static int compareAvgZPOLY4D(const void *arg1, const void *arg2);
public:
	Pipeline(void);
	~Pipeline(void);
	LightSystem lights;
	//���������� ��������� ����������
	bool bLighting;

	//void ResetOBJECT4D(OBJECT4D_PTR obj);
	void ConvertFromHomogenerous4DOBJECT4D(OBJECT4D_PTR obj);
	void ConvertFromHomogenerous4DRENDERLIST4D(RENDERLIST4D_PTR rendList);

	//������� ��� ����� �������������� �������� � ������� ������������: //���������� � struct3D � �����. ��������� 
	/*void TransformRENDERLIST4D(RENDERLIST4D_PTR rendList, MATRIX4X4_PTR mt, coordSelect enm);
	void TransformOBJECT4D(OBJECT4D_PTR obj, MATRIX4X4_PTR mt, coordSelect enm, bool transformBasis);*/
	//������� 1 ����� ��������� ������������. �������������� ��������� ��������� � �������:
	void ModelToWorldOBJECT4D(OBJECT4D_PTR obj, int all_frames = 0, coordSelect enm = TRANSFORM_LOCAL_TO_TRANS);
	//void ModelToWorldRENDERLIST4D(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos, coordSelect enm = coordSelect::TRANSFORM_LOCAL_TO_TRANS);
	//������� 1.1 ����� ��������� ������������. ���������� ��������:
	int CullOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam, cullObject enm);
	//������� 1.2 ����� ��������� ������������. �������� �������� ������������:
	void RemoveBackfacesOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void RemoveBackfacesRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//������� 2 ����� ��������� ������������. �������������� ������� ��������� � ���������� ������:
	void WorldToCameraOBJECT4D(CAM4D_PTR cam, OBJECT4D_PTR obj);
	void WorldToCameraRENDERLIST4D(CAM4D_PTR cam, RENDERLIST4D_PTR rendList);
	//������� 2.1 ����� ��������� ������������. ���������� ��������� �������������:
	void ClipPolysRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam, clipPoly enm);
	//������� 2.2 ����� ��������� ������������. ��������� ������������:
	int LightOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam); //��������������� ��� ������������� 32 ������ ����. ����� �������� ����� ��� ������ � ������� ������ �����������!!!!!
	int LightRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//������� 2.3 ����� ��������� ������������. ���������� �������������:
	//void SortOBJECT4D(OBJECT4D_PTR obj, sortMethod enm);
	void SortRENDERLIST4D(RENDERLIST4D_PTR rendList, sortMethod enm);
	//������� 3 ����� ��������� ������������. ����������������� ��������������:
	void CameraToPerspectiveOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void CameraToPerspectiveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//������� 4 ����� ��������� ������������. �������������� � �������� ����������:
	void PerspectiveToScreenOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void PerspectiveToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	/*
		��� ����������� 3 � 4 �����:

		����:
		alpha = (0.5f * cam->viewport_width - 0.5f);
		beta = (0.5f * cam->viewport_height - 0.5f);

		xs = alpha + alpha * d * xc / zc;
		ys = beta - beta * d * yc * ar / zc;

		�������� �����������:
		beta = alpha / ar

		� ���������� ��� � ������ ���������:
		ys = beta - alpha / ar * d * yc * ar / zc = beta - alpha * d * yc * ar / zc * ar;
		
		ar ������������:
		ys = beta - alpha * d * yc / zc;

		��������� d ��������� ���������� � ����������:
		d = 0.5 * viewplane_width * tan(a / 2)

		��� viewplane_width ��� ������ ��������� ������������� (-1;1) ������ 2
		viewplane_width ����� �������� �� viewport_width = 800 - 1
		� ��������� ���
		d = 0.5 * viewport_width * tan(a / 2)
		� ������ ����� ������� ������� d:

		xs = alpha - d * xc / zc;
		ys = beta - d * yc / zc;
	*/
	//������������ 3 � 4 ����� ��������� ������������.
	void CameraToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//������������ 1.2 � 2.0 ����� ��������� ������������.
	void WorldToCamera_and_BackfaceRemoveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);

	//Line pipiline:
	void LocalToCameraAndClipOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam);
	void CameraToScreenOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam);

	void ClipPolygonScreenBounds(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);

private:
	typedef int OutCode;

	static constexpr int INSIDE = 0; // 0000
	static constexpr int LEFT = 1;   // 0001
	static constexpr int RIGHT = 2;  // 0010
	static constexpr int BOTTOM = 4; // 0100
	static constexpr int TOP = 8;    // 1000

	enum class LinePointEdge {
		NoEdge,
		ClipMinX,
		ClipMinY,
		ClipMaxX,
		ClipMaxY
	};

	struct Point2DOnEdge {
		Point2D pt;
		LinePointEdge edge;
		bool fromTriangle = false;

		Point2DOnEdge ClampedByEdge(const Point2D& clipMin, const Point2D& clipMax) const;
		// if point on edge of clip rect then this method returns order value. Order values increases from bottom edge then right, top, left
		// if point is not on edge then order == infinity
		float GetEdgeOrder(const Point2D& clipMax, LinePointEdge startEdge) const;
	};

	struct Line2D {
		Point2D start;
		Point2D end;
		bool swapped = false; // start/outcodeStart & end/outcodeEnd swapped relative to original line
		bool clipped = false; // start or/and end clipped by CohenSutherlandLineClip
		bool declined = false; // whole line is out of screen. Result from CohenSutherlandLineClip
		OutCode outcodeStart = INSIDE;
		OutCode outcodeEnd = INSIDE;
		LinePointEdge startEdge = LinePointEdge::NoEdge; // on which edge of clip rect point is located after CohenSutherlandLineClip
		LinePointEdge endEdge = LinePointEdge::NoEdge; // on which edge of clip rect point is located after CohenSutherlandLineClip

		std::optional<Point2DOnEdge> GetStartPointOnEdge() const;
		std::optional<Point2DOnEdge> GetEndPointOnEdge() const;
	};

	static Line2D SortSwap(Line2D line);
	static Line2D Unswap(Line2D line);
	static OutCode ComputeOutCode(const Point2D& pt, const Point2D& clipMin, const Point2D& clipMax);
	static std::optional<Line2D> CohenSutherlandLineClip(Line2D line, const Point2D& clipMin, const Point2D& clipMax, bool tryShortLen = false);
};
