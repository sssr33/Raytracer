#pragma once
#include "structures.h"
#include "Math3DStructs.h"

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
	//переменные состояния коневейера
	bool bLighting;

	//void ResetOBJECT4D(OBJECT4D_PTR obj);
	void ConvertFromHomogenerous4DOBJECT4D(OBJECT4D_PTR obj);
	void ConvertFromHomogenerous4DRENDERLIST4D(RENDERLIST4D_PTR rendList);

	//Функции для общих преобразований объектов и списков визуализации: //перенесено в struct3D в соотв. структуры 
	/*void TransformRENDERLIST4D(RENDERLIST4D_PTR rendList, MATRIX4X4_PTR mt, coordSelect enm);
	void TransformOBJECT4D(OBJECT4D_PTR obj, MATRIX4X4_PTR mt, coordSelect enm, bool transformBasis);*/
	//Функции 1 этапа конвейера визуализации. Преобразование локальных координат в мировые:
	void ModelToWorldOBJECT4D(OBJECT4D_PTR obj, int all_frames = 0, coordSelect enm = TRANSFORM_LOCAL_TO_TRANS);
	//void ModelToWorldRENDERLIST4D(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos, coordSelect enm = coordSelect::TRANSFORM_LOCAL_TO_TRANS);
	//Функции 1.1 этапа конвейера визуализации. Отбраковка объектов:
	int CullOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam, cullObject enm);
	//Функции 1.2 этапа конвейера визуализации. Удачение обратных поверхностей:
	void RemoveBackfacesOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void RemoveBackfacesRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//Функции 2 этапа конвейера визуализации. Преобразование мировых координат в координаты камеры:
	void WorldToCameraOBJECT4D(CAM4D_PTR cam, OBJECT4D_PTR obj);
	void WorldToCameraRENDERLIST4D(CAM4D_PTR cam, RENDERLIST4D_PTR rendList);
	//Функции 2.1 этапа конвейера визуализации. Трехмерное отсечение треугольников:
	void ClipPolysRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam, clipPoly enm);
	//Функции 2.2 этапа конвейера визуализации. Освещение поверхностей:
	int LightOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam); //предпологаеться что используеться 32 битный цвет. Нужно написать класс для работы с цветами разной разрядности!!!!!
	int LightRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//Функции 2.3 этапа конвейера визуализации. Сортировка треугольников:
	//void SortOBJECT4D(OBJECT4D_PTR obj, sortMethod enm);
	void SortRENDERLIST4D(RENDERLIST4D_PTR rendList, sortMethod enm);
	//Функции 3 этапа конвейера визуализации. Аксонометрическое преобразование:
	void CameraToPerspectiveOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void CameraToPerspectiveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//Функции 4 этапа конвейера визуализации. Преобразование в экранные координаты:
	void PerspectiveToScreenOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam);
	void PerspectiveToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	/*
		Для объединения 3 и 4 этапа:

		Было:
		alpha = (0.5f * cam->viewport_width - 0.5f);
		beta = (0.5f * cam->viewport_height - 0.5f);

		xs = alpha + alpha * d * xc / zc;
		ys = beta - beta * d * yc * ar / zc;

		Ипользуя соотношение:
		beta = alpha / ar

		и подставляя его в старое уравнение:
		ys = beta - alpha / ar * d * yc * ar / zc = beta - alpha * d * yc * ar / zc * ar;
		
		ar сокращаеться:
		ys = beta - alpha * d * yc / zc;

		поскольку d являеться константой и равняеться:
		d = 0.5 * viewplane_width * tan(a / 2)

		где viewplane_width это ширина плоскости проецирования (-1;1) равная 2
		viewplane_width можно заменить на viewport_width = 800 - 1
		и расчитать как
		d = 0.5 * viewport_width * tan(a / 2)
		с учетом новой формулы расчета d:

		xs = alpha - d * xc / zc;
		ys = beta - d * yc / zc;
	*/
	//Объедененные 3 и 4 этапы конвейера визуализации.
	void CameraToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);
	//Объедененные 1.2 и 2.0 этапы конвейера визуализации.
	void WorldToCamera_and_BackfaceRemoveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam);

	//Line pipiline:
	void LocalToCameraAndClipOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam);
	void CameraToScreenOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam);
};

