#include "StdAfx.h"
#include "GeometryGen.h"


GeometryGen::GeometryGen(void)
{
	this->attrBase = 0;
	this->frontBackFace.resize(1024, 0);
}


GeometryGen::~GeometryGen(void)
{
}

int GeometryGen::generateTerrainOBJECT4D(OBJECT4D_PTR obj, float width, float height, float scale, TEXTURE2D_PTR tex2DMap, TEXTURE2D_PTR tex2D, unsigned int color, POINT4D_PTR pos, VECTOR4D_PTR rot, unsigned int attr)
{
	//char buffer[256];

	//float colTstep, rowTstep;
	//float colVstep, rowVstep;

	//unsigned int rgbWhite;

	//TEXTURE2D tex

	if(obj)
	{
		obj->Destroy();
	}



	return 1;
}

int GeometryGen::generateRotationForm(POINT4D_PTR pts, int numPts, int numSides, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor)
{

	/*if(pts)
		delete pts;*/
	if(obj)
		obj->Destroy();

	if(numSides < 3) return 0; //Как минимум у объекта вращение должно быть 3 стороны

	numSides--;//уменьшаем количество для того чтобы треугольники между последней и 1 стороной имели правильный размер, но угол для каждой итерации расчитываем 
	//как numSides + 1

	int numHSegs = numPts - 2 - 1;
	int numGenVertsPerIteration = numPts - 2;
	int numVerts = (numPts) + ((numPts - 2) * numSides);
	int numPolysIn1Side = 2 + (((numPts - 2) * 2) - 2);
	int numPolys = numPolysIn1Side * numSides;//(2 * numSides) + ((numVerts - 2) * 2);
	int currVert;
	int currPoly = 0;
	float rotAngle = -360.0f / ((float)numSides + 1.0f);
	int polyAttr = struct3D::POLY4D_ATTR_RGB24 | struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD | struct3D::POLY4D_ATTR_DISABLE_MATERIAL;
	int vertAttr = struct3D::VERTEX4DT_ATTR_NORMAL | struct3D::VERTEX4DT_ATTR_POINT;
	int polyState = struct3D::POLY4D_STATE_ACTIVE;// | struct3D::OBJECT4D_STATE_VISIBLE;
	int polyColor = iColor;
	MATRIX4X4 mrot;

	//float objCenter;
	float objHMax = FLT_MIN;
	float objHMin = FLT_MAX;
	POINT4D pCenterCorrection;

	vecZero(&pCenterCorrection);

	//for(int i = 0; i < numPts; i++)
	//{
	//	/*if(pts[i].y > objHMax) objHMax = pts[i].y;
	//	if(pts[i].y < objHMin) objHMin = pts[i].y;*/
	//	//vecAdd(&pts[i], &pCenterCorrection, &pCenterCorrection);
	//	objCenter += pts[i].y;
	//}

	/*pCenterCorrection.x /= numPts;
	pCenterCorrection.y /= numPts;
	pCenterCorrection.z /= numPts;*/

	//objCenter /= numPts;

	//objCenter = (objHMax - objHMin) / 2.0f;

	/*if(objCenter != 0)
	{*/
		/*POINT4D pCenterCorrect;
		vecZero(&pCenterCorrect);
		pCenterCorrect.y = fabs(objCenter);*/

		//for(int i = 0; i < numPts; i++)
		//{
		//	//vecSub(&pts[i], &pCenterCorrection, &pts[i]);
		//	pts[i].y -= objCenter;
		//}
	//}

	//POINT4D worldPos;

	mrot.Build_XYZ_Rotation_MATRIX4X4(0, rotAngle, 0);

	//TCHAR mas[128];

	obj->Init(numVerts, numPolys + numPolysIn1Side, 1);
	obj->state = struct3D::OBJECT4D_STATE_ACTIVE | struct3D::OBJECT4D_STATE_VISIBLE;
	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = struct3D::OBJECT4D_ATTR_SINGLE_FRAME;

	//vecZero(worldPos);
	vecCopy(worldPos, &obj->world_pos);

	vecCopy(&pts[0], &obj->vlist_local[0].v);//1 vert - 0 idx
	vecCopy(&pts[numPts - 1], &obj->vlist_local[1].v);//last vert - 1 idx
	obj->vlist_local[0].attr = vertAttr;
	obj->vlist_local[1].attr = vertAttr;

	//vecAdd(&pCenterCorrection, &obj->vlist_local[0].v, &pCenterCorrection);
	//vecAdd(&pCenterCorrection, &obj->vlist_local[1].v, &pCenterCorrection);

	currVert = 2;

	for(int i = 1; i < numPts - 1; i++, currVert++)
	{
		vecCopy(&pts[i], &obj->vlist_local[currVert].v);
		obj->vlist_local[currVert].attr = vertAttr;

		//vecAdd(&pCenterCorrection, &obj->vlist_local[currVert].v, &pCenterCorrection);
	}

	for(int i = 0; i < numSides; i++)
	{
		//вращаем все точки кроме первой и последней
		for(int j = 1; j < numPts - 1; j++, currVert++)
		{
			POINT4D pRes;

			vecMulMat(&pts[j], &mrot, &pRes);
			vecCopy(&pRes, &pts[j]);

			vecCopy(&pts[j], &obj->vlist_local[currVert].v);
			obj->vlist_local[currVert].attr = vertAttr;

			//vecAdd(&pCenterCorrection, &obj->vlist_local[currVert].v, &pCenterCorrection);
		}

		//Генерируем треугольники для всех сегментов вдоль стороны(по высоте).
		for(int k = 0; k < numHSegs; k++)
		{
			//задаем индексы вершин по часовой стрелке для всех треугольников кроме верхнего и нижнего
			//задаем сразу по 2 треугольника
				obj->plist[currPoly].vert[0] = (currVert - k - 1) - numGenVertsPerIteration;
				obj->plist[currPoly].vert[1] = (currVert - k - 1) - 1;
				obj->plist[currPoly].vert[2] = (currVert - k - 1);

				obj->plist[currPoly].vlist = obj->vlist_local;
				obj->plist[currPoly].attr = polyAttr;
				obj->plist[currPoly].state = polyState;
				obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;

				obj->plist[currPoly + 1].vert[0] = (currVert - k - 1) - numGenVertsPerIteration;
				obj->plist[currPoly + 1].vert[1] = (currVert - k - 1) - numGenVertsPerIteration - 1;
				obj->plist[currPoly + 1].vert[2] = (currVert - k - 1) - 1;

				obj->plist[currPoly + 1].vlist = obj->vlist_local;
				obj->plist[currPoly + 1].attr = polyAttr;
				obj->plist[currPoly + 1].state = polyState;
				obj->plist[currPoly + 1].color = obj->plist[currPoly + 1].lit_color[0] = polyColor;

				currPoly += 2;
		}

		//Генерируем верхний и нижний треугольники
		//верхний
		obj->plist[currPoly].vert[0] = (currVert - 1) - numHSegs - numGenVertsPerIteration;
		obj->plist[currPoly].vert[1] = 0;
		obj->plist[currPoly].vert[2] = (currVert - 1) - numHSegs;

		obj->plist[currPoly].vlist = obj->vlist_local;
		obj->plist[currPoly].attr = polyAttr;
		obj->plist[currPoly].state = polyState;
		obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;

		//нижний
		obj->plist[currPoly + 1].vert[0] = (currVert - 1);
		obj->plist[currPoly + 1].vert[1] = 1;
		obj->plist[currPoly + 1].vert[2] = (currVert - 1) - numGenVertsPerIteration;

		obj->plist[currPoly + 1].vlist = obj->vlist_local;
		obj->plist[currPoly + 1].attr = polyAttr;
		obj->plist[currPoly + 1].state = polyState;
		obj->plist[currPoly + 1].color = obj->plist[currPoly + 1].lit_color[0] = polyColor;

		currPoly += 2;
	}

	//Генерируем треугольники соединяющий последнюю сторону и первую

	for(int k = 0; k < numHSegs; k++)
		{
			//задаем индексы вершин по часовой стрелке для всех треугольников кроме верхнего и нижнего
			//задаем сразу по 2 треугольника
				obj->plist[currPoly].vert[0] = (currVert - k - 1);
				obj->plist[currPoly].vert[1] = numPts - k - 1 - 1;
				obj->plist[currPoly].vert[2] = numPts - k - 1;

				obj->plist[currPoly].vlist = obj->vlist_local;
				obj->plist[currPoly].attr = polyAttr;
				obj->plist[currPoly].state = polyState;
				obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;

				obj->plist[currPoly + 1].vert[0] = (currVert - k - 1);
				obj->plist[currPoly + 1].vert[1] = (currVert - k - 1) - 1;
				obj->plist[currPoly + 1].vert[2] = numPts - k - 1 - 1;

				obj->plist[currPoly + 1].vlist = obj->vlist_local;
				obj->plist[currPoly + 1].attr = polyAttr;
				obj->plist[currPoly + 1].state = polyState;
				obj->plist[currPoly + 1].color = obj->plist[currPoly + 1].lit_color[0] = polyColor;

				currPoly += 2;
		}

	//Генерируем верхний и нижний треугольники
	//верхний
	obj->plist[currPoly].vert[0] = (currVert - 1) - numHSegs;
	obj->plist[currPoly].vert[1] = 0;
	obj->plist[currPoly].vert[2] = 2;

	obj->plist[currPoly].vlist = obj->vlist_local;
	obj->plist[currPoly].attr = polyAttr;
	obj->plist[currPoly].state = polyState;
	obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;

	//нижний
	obj->plist[currPoly + 1].vert[0] = (numPts - 1);
	obj->plist[currPoly + 1].vert[1] = 1;
	obj->plist[currPoly + 1].vert[2] = (currVert - 1);

	obj->plist[currPoly + 1].vlist = obj->vlist_local;
	obj->plist[currPoly + 1].attr = polyAttr;
	obj->plist[currPoly + 1].state = polyState;
	obj->plist[currPoly + 1].color = obj->plist[currPoly + 1].lit_color[0] = polyColor;

	currPoly += 2;

	/*if(objCenter != 0)
	{
		POINT4D pCenterCorrect;
		vecZero(&pCenterCorrect);
		pCenterCorrect.y = fabs(objCenter);

		for(int i = 0; i < obj->num_vertices; i++)
		{
			vecSub(&obj->vlist_local[i].v, &pCenterCorrect, &obj->vlist_local[i].v);
		}
	}*/

	/*pCenterCorrection.x /= obj->num_vertices;
	pCenterCorrection.y /= obj->num_vertices;
	pCenterCorrection.z /= obj->num_vertices;*/

	//for(int i = 0; i < obj->num_vertices; i++)
	//{
	//	vecSub(&obj->vlist_local[i].v, &pCenterCorrection, &obj->vlist_local[i].v);
	//}

	obj->ComputePolyNormals();
	obj->ComputeVertexNormals();
	obj->ComputeBBox();
	obj->ComputeAdjacency();

	//delete pts;

	return 1;
}

int GeometryGen::generateCylinder(int numSides, int numHSegs, int numCapSegs, float radius, float height, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor)
{
	POINT4D_PTR ptsRot;

	int numCapPoints = ((numCapSegs + 1) * 2);//4
	int numPts = numCapPoints + (numHSegs - 1);//8
	//r = 105.125
	//h = 386.512
	float capSegsStep = radius / (float)numCapSegs;//105.125
	float capPoint = 0.0f;
	float heightSegsStep = height / (float)numHSegs;//77,3024
	float heightPoint = height;

	ptsRot = new POINT4D[numPts];

	//cap segments
	for(int i = 0; i < numCapSegs + 1; i++)
	{
		ptsRot[i].x = capPoint;
		ptsRot[i].y = heightPoint;
		ptsRot[i].z = 0.0f;
		ptsRot[i].w = 1.0f;

		ptsRot[numPts - 1 - i].x = capPoint;
		ptsRot[numPts - 1 - i].y = 0;
		ptsRot[numPts - 1 - i].z = 0.0f;
		ptsRot[numPts - 1 - i].w = 1.0f;

		capPoint -= capSegsStep;
	}

	capPoint += capSegsStep;
	heightPoint -= heightSegsStep;

	//height segments
	for(int i = 0; i < numHSegs - 1; i++)
	{
		ptsRot[numCapSegs + 1 + i].x = capPoint;
		ptsRot[numCapSegs + 1 + i].y = heightPoint;
		ptsRot[numCapSegs + 1 + i].z = 0.0f;
		ptsRot[numCapSegs + 1 + i].w = 1.0f;

		heightPoint -= heightSegsStep;
	}

	for(int i = 0; i < numPts; i++)
	{
		ptsRot[i].y -= (height / 2);
	}

	worldPos->y = (height / 2);

	this->generateRotationForm(ptsRot, numPts, numSides, obj, worldPos, iColor);

	delete ptsRot;

	return 1;
}

int GeometryGen::generateSphere(int numSegs, float radius, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor)
{
	int numSides = numSegs;
	int numPts = numSegs / 2 + 1;//Дробную чать не учитываем, т.к. кол-во вершин не может быть дробным
	int numHSegs = numPts - 2; //количество точек по вертикали минус начальная и конечная
	float rotAngle = 360.0f / (float)numSegs;

	if(!obj) return 0;
	if(numSegs < 4) return 0;

	POINT4D p, pRes;
	POINT4D_PTR pts = new POINT4D[numPts];
	MATRIX4X4 mrot;

	//Записываем начальную и конечную точки
	pts[0].x = 0;
	pts[0].y = radius;
	pts[0].z = 0.0f;
	pts[0].w = 1.0f;

	pts[numPts - 1].x = 0;
	pts[numPts - 1].y = -radius;
	pts[numPts - 1].z = 0.0f;
	pts[numPts - 1].w = 1.0f;

	//генерируем остальные точки
	mrot.Build_XYZ_Rotation_MATRIX4X4(0.0f, 0.0f, rotAngle);
	vecCopy(&pts[0], &p);

	for(int i = 1; i < numPts - 1; i++)
	{
		vecMulMat(&p, &mrot, &pRes);

		vecCopy(&pRes, &pts[i]);

		vecCopy(&pRes, &p);
	}

	this->generateRotationForm(pts, numPts, numSegs, obj, worldPos, iColor);

	delete[] pts;

	return 1;
}

int GeometryGen::generateCone(int numSides, int numHSegs, int numCapSegs, float radiusT, float radiusB, float height, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor)
{
	POINT4D_PTR ptsRot;

	int numCapPointsT = 1;//((numCapSegs + 1) * 2);
	int numCapPointsB = 1;
	int numPts = 0;//numCapPoints + (numHSegs - 1);

	float capSegsStepT = 0;//radiusT / (float)numCapSegs;
	float capSegsStepB = 0;//radiusB / (float)numCapSegs;

	float capPoint = 0.0f;
	float heightSegsStep = height / (float)numHSegs;
	float heightPoint = height;

	if(math3D::mathFunc.FCMP(radiusT, 0) && math3D::mathFunc.FCMP(radiusB, 0)) return 0;

	if(!math3D::mathFunc.FCMP(radiusT, 0))
	{
		capSegsStepT = radiusT / (float)numCapSegs;
		numCapPointsT = numCapSegs + 1;
	}

	if(!math3D::mathFunc.FCMP(radiusB, 0))
	{
		capSegsStepB = radiusB / (float)numCapSegs;
		numCapPointsB = numCapSegs + 1;
	}

	numPts = numCapPointsT + numCapPointsB + (numHSegs - 1);

	//TCHAR mas[128];

	ptsRot = new POINT4D[numPts];

	//cap segmentsT
	for(int i = 0; i < numCapPointsT; i++)
	{
		ptsRot[i].x = capPoint;
		ptsRot[i].y = heightPoint;
		ptsRot[i].z = 0.0f;
		ptsRot[i].w = 1.0f;

		capPoint -= capSegsStepT;
	}

	//cap segmentsB
	capPoint = 0;
	for(int i = 0; i < numCapPointsB; i++)
	{
		ptsRot[numPts - 1 - i].x = capPoint;
		ptsRot[numPts - 1 - i].y = 0;
		ptsRot[numPts - 1 - i].z = 0.0f;
		ptsRot[numPts - 1 - i].w = 1.0f;

		capPoint -= capSegsStepB;
	}

	float hstepX = (radiusB - radiusT) / numHSegs;
	capPoint = -radiusT - hstepX;
	heightPoint -= heightSegsStep;

	//height segments
	for(int i = 0; i < numHSegs - 1; i++)
	{
		ptsRot[numCapPointsT + i].x = capPoint;
		ptsRot[numCapPointsT + i].y = heightPoint;
		ptsRot[numCapPointsT + i].z = 0.0f;
		ptsRot[numCapPointsT + i].w = 1.0f;

		heightPoint -= heightSegsStep;
		capPoint -= hstepX;
	}

	for(int i = 0; i < numPts; i++)
	{
		ptsRot[i].y -= (height / 2);
	}

	worldPos->y = (height / 2);

	this->generateRotationForm(ptsRot, numPts, numSides, obj, worldPos, iColor);

	delete ptsRot;

	return 1;
}

int GeometryGen::generateBox(float length, float width, float height, int LSegs, int WSegs, int HSegs, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor)
{
	if(!worldPos) return 0;
	if(LSegs < 1 || WSegs < 1 || HSegs < 1) return 0;

	length = fabs(length);
	width = fabs(width);
	height = fabs(height);

	POINT4D currPt;
	int currVert = 0;
	int currPoly = 0;
	int polyAttr = struct3D::POLY4D_ATTR_RGB24 | struct3D::POLY4D_ATTR_SHADE_MODE_FLAT | struct3D::POLY4D_ATTR_DISABLE_MATERIAL;
	int vertAttr = struct3D::VERTEX4DT_ATTR_NORMAL | struct3D::VERTEX4DT_ATTR_POINT;
	int polyState = struct3D::POLY4D_STATE_ACTIVE | struct3D::OBJECT4D_STATE_VISIBLE;
	int polyColor = iColor;

	int vnum = (WSegs + 1) * (HSegs + 1) * 2;
	vnum += (((WSegs + 1) * 2) + ((HSegs - 1) * 2)) * (LSegs - 1);

	int pnum = ((WSegs * 2) * HSegs) * 2;
	pnum += ((WSegs * 2) * LSegs) * 2;
	pnum += ((HSegs * 2) * LSegs) * 2;


	if(obj)
	{
		obj->Destroy();
	}
	else
	{
		/*obj = new OBJECT4D();
		ZeroMemory(obj, sizeof(OBJECT4D));*/
		return 0;
	}

	obj->Init(vnum, pnum, 1, 0);

	obj->state = struct3D::OBJECT4D_STATE_ACTIVE | struct3D::OBJECT4D_STATE_VISIBLE;
	obj->num_frames = 1;
	obj->curr_frame = 0;
	obj->attr = struct3D::OBJECT4D_ATTR_SINGLE_FRAME;

	//Вычисляем заднюю вернюю левую точку
	currPt.x = 0 - (width / 2);
	currPt.y = 0 + (height / 2);
	currPt.z = 0 - (length / 2);

	//
	std::vector<int> shapeIdx, shapeIdxNext;

	//Генерируем верхние вершины
	int vm = WSegs + 1;
	float WStep = width / WSegs;
	for(int i = 0; i < vm; i++)
	{
		vecCopy(&currPt, &obj->vlist_local[currVert].v);
		obj->vlist_local[currVert].attr = vertAttr;
		currPt.x += WStep;
		shapeIdx.push_back(currVert);
		currVert++;
	}

	//Генерируем правые вершины
	vm = HSegs - 1;
	currPt.x -= WStep;
	float HStep = height / (HSegs);
	for(int i = 0; i < vm; i++)
	{
		currPt.y -= HStep;
		vecCopy(&currPt, &obj->vlist_local[currVert].v);
		obj->vlist_local[currVert].attr = vertAttr;
		shapeIdx.push_back(currVert);
		currVert++;
	}
	//Генерируем нижние вершины
	currPt.y -= HStep;
	vm = WSegs + 1;
	for(int i = 0; i < vm; i++)
	{
		vecCopy(&currPt, &obj->vlist_local[currVert].v);
		obj->vlist_local[currVert].attr = vertAttr;
		currPt.x -= WStep;
		shapeIdx.push_back(currVert);
		currVert++;
	}
	//Генерируем левые вершины
	vm = HSegs - 1;
	currPt.x += WStep;
	for(int i = 0; i < vm; i++)
	{
		currPt.y += HStep;
		vecCopy(&currPt, &obj->vlist_local[currVert].v);
		obj->vlist_local[currVert].attr = vertAttr;
		shapeIdx.push_back(currVert);
		currVert++;
	}

	//Создаем треугольники для задней стенки===============
	std::vector<int> topIdx, btIdx;

	vm = WSegs + 1;

	for(int i = 0; i < vm; i++)
		topIdx.push_back(shapeIdx[i]);

	int pm = HSegs - 1;
	int shapeIdxSize = static_cast<int>(shapeIdx.size());

	for(int i = 0; i < pm; i++)
	{
		btIdx.clear();

		btIdx.push_back(shapeIdx[shapeIdxSize - 1 - i]);//индекс слева сверху
		vm = WSegs;// - 1;

		for(int j = 1; j < vm; j++)
		{
			vecCopy(&obj->vlist_local[topIdx[j]].v, &currPt);
			currPt.y -= HStep;
			vecCopy(&currPt, &obj->vlist_local[currVert].v);
			obj->vlist_local[currVert].attr = vertAttr;
			btIdx.push_back(currVert);
			currVert++;
		}

		btIdx.push_back(shapeIdx[WSegs + 1 + i]);

		for(int j = 0; j < WSegs; j++)
		{
			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j];
			obj->plist[currPoly].vert[1] = topIdx[j];
			obj->plist[currPoly].vert[2] = topIdx[j + 1];

			currPoly++;

			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j];
			obj->plist[currPoly].vert[1] = topIdx[j + 1];
			obj->plist[currPoly].vert[2] = btIdx[j + 1];

			currPoly++;
		}

		btIdx.swap(topIdx);
	}

	btIdx.clear();

	vm = WSegs + 1;

	for(int i = 0; i < vm; i++)
	{
		btIdx.push_back(shapeIdx[shapeIdxSize - 1 - (HSegs - 1) - i]);
	}

	for(int j = 0; j < WSegs; j++)
		{
			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j];
			obj->plist[currPoly].vert[1] = topIdx[j];
			obj->plist[currPoly].vert[2] = topIdx[j + 1];

			currPoly++;

			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j];
			obj->plist[currPoly].vert[1] = topIdx[j + 1];
			obj->plist[currPoly].vert[2] = btIdx[j + 1];

			currPoly++;
		}
	//==============================================================
	
	//Создаем треугольники по длинне===============
	float LStep = length / LSegs;
	pm = shapeIdxSize - 1;

	for(int i = 0; i < LSegs; i++)
	{
		for(int j = 0; j < shapeIdxSize; j++)
		{
			vecCopy(&obj->vlist_local[shapeIdx[j]].v, &currPt);
			currPt.z += LStep;
			vecCopy(&currPt, &obj->vlist_local[currVert].v);
			obj->vlist_local[currVert].attr = vertAttr;
			shapeIdxNext.push_back(currVert);
			currVert++;
		}

		for(int j = 0; j < pm; j++)
		{
			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = shapeIdx[j];
			obj->plist[currPoly].vert[1] = shapeIdxNext[j];
			obj->plist[currPoly].vert[2] = shapeIdxNext[j + 1];

			currPoly++;

			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = shapeIdx[j];
			obj->plist[currPoly].vert[1] = shapeIdxNext[j + 1];
			obj->plist[currPoly].vert[2] = shapeIdx[j + 1];

			currPoly++;
		}

		obj->plist[currPoly].attr = polyAttr;
		obj->plist[currPoly].state = polyState;
		obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
		obj->plist[currPoly].vlist = obj->vlist_local;

		obj->plist[currPoly].vert[0] = shapeIdx[pm];
		obj->plist[currPoly].vert[1] = shapeIdxNext[pm];
		obj->plist[currPoly].vert[2] = shapeIdxNext[0];

		currPoly++;

		obj->plist[currPoly].attr = polyAttr;
		obj->plist[currPoly].state = polyState;
		obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
		obj->plist[currPoly].vlist = obj->vlist_local;

		obj->plist[currPoly].vert[0] = shapeIdx[pm];
		obj->plist[currPoly].vert[1] = shapeIdxNext[0];
		obj->plist[currPoly].vert[2] = shapeIdx[0];

		currPoly++;

		shapeIdxNext.swap(shapeIdx);
		shapeIdxNext.clear();
	}

	//=============================================================
	//Создаем треугольники для передней стенки===============
	vm = WSegs + 1;
	topIdx.clear();
	for(int i = 0; i < vm; i++)
		topIdx.push_back(shapeIdx[i]);

	pm = HSegs - 1;

	for(int i = 0; i < pm; i++)
	{
		btIdx.clear();

		btIdx.push_back(shapeIdx[shapeIdxSize - 1 - i]);//индекс слева сверху
		vm = WSegs;// - 1;

		for(int j = 1; j < vm; j++)
		{
			vecCopy(&obj->vlist_local[topIdx[j]].v, &currPt);
			currPt.y -= HStep;
			vecCopy(&currPt, &obj->vlist_local[currVert].v);
			obj->vlist_local[currVert].attr = vertAttr;
			btIdx.push_back(currVert);
			currVert++;
		}

		btIdx.push_back(shapeIdx[WSegs + 1 + i]);

		for(int j = 0; j < WSegs; j++)
		{
			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j + 1];
			obj->plist[currPoly].vert[1] = topIdx[j + 1];
			obj->plist[currPoly].vert[2] = topIdx[j];

			currPoly++;

			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j + 1];
			obj->plist[currPoly].vert[1] = topIdx[j];
			obj->plist[currPoly].vert[2] = btIdx[j];

			currPoly++;
		}

		btIdx.swap(topIdx);
	}

	btIdx.clear();

	vm = WSegs + 1;

	for(int i = 0; i < vm; i++)
	{
		btIdx.push_back(shapeIdx[shapeIdxSize - 1 - (HSegs - 1) - i]);
	}

	for(int j = 0; j < WSegs; j++)
		{
			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j + 1];
			obj->plist[currPoly].vert[1] = topIdx[j + 1];
			obj->plist[currPoly].vert[2] = topIdx[j];

			currPoly++;

			obj->plist[currPoly].attr = polyAttr;
			obj->plist[currPoly].state = polyState;
			obj->plist[currPoly].color = obj->plist[currPoly].lit_color[0] = polyColor;
			obj->plist[currPoly].vlist = obj->vlist_local;

			obj->plist[currPoly].vert[0] = btIdx[j + 1];
			obj->plist[currPoly].vert[1] = topIdx[j];
			obj->plist[currPoly].vert[2] = btIdx[j];

			currPoly++;
		}

	//=============================================================

	vecCopy(worldPos, &obj->world_pos);
	obj->world_pos.y = height / 2;

	obj->ComputePolyNormals();
	obj->ComputeVertexNormals();
	obj->ComputeBBox();
	obj->ComputeAdjacency();

	return 1;
}

int GeometryGen::generateShadowVolume(LIGHT_PTR l_in, int num_l_in, OBJECT4D_PTR objShadowCaster_in, OBJECT4D_PTR objShadowVol_inout, int sh_vol_num_in)
{
	//версия для одного источника света!!!!!!!

	this->attrBase += 4;

	if(!objShadowCaster_in->adj) return 0;

	if(objShadowCaster_in->num_polys > static_cast<int>(this->frontBackFace.size())) this->frontBackFace.resize(objShadowCaster_in->num_polys);

	if(this->frontBackFace[0] & 0x8000) this->frontBackFace.resize(this->frontBackFace.size(), 0); //Для правильной работы с атрибутами

	if(l_in[0].attr & LIGHT_ATTR_AMBIENT) return 0;
	
	//Для точечного источника света нужно добавить преобразование его позиции в пространсво объекта!!!!

	VECTOR4D u, v, n, /*r, c,*/ l;
	float dp;

	int idx[4] = {0, 1, 2, 0};
	std::vector<int> svVerts;

	//for(int i = 0; i < objShadowCaster_in->num_polys; i++)
	//{
	//	POLY4D_PTR currPoly = &objShadowCaster_in->plist[i];

	//	u.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[1]].v);
	//	v.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[2]].v);

	//	u.VECTOR4D_Cross(&v, &n);

	//	vecScale(-1, &l_in[0].dir, &l);

	//	dp  = n.VECTOR4D_Dot(&l);

	//	if(dp > 0)
	//	{
	//		this->frontBackFace[i] = FRONT_CULLED(this->attrBase);
	//	}
	//	else
	//	{
	//		this->frontBackFace[i] = BACK_CULLED(this->attrBase);
	//	}
	//}

	//svVerts.clear();

	//for(int i = 0; i < objShadowCaster_in->num_polys; i++)
	//{
	//	if(this->frontBackFace[i] == FRONT_CULLED(this->attrBase))
	//	{
	//		for(int j = 0; j < 3; j++)
	//		{
	//			//POLY4D_PTR currPoly = &objShadowCaster_in->plist[objShadowCaster_in->adj[i].adj[j]];

	//			if(this->frontBackFace[objShadowCaster_in->adj[i].adj[j]] == BACK_CULLED(this->attrBase))
	//			{
	//				//this->frontBackFace[objShadowCaster_in->adj[i].adj[j]] = PROC_CULLED(this->attrBase);

	//				objShadowCaster_in->plist[i].color = ARGB32BIT(255,0,0,255);
	//				objShadowCaster_in->plist[objShadowCaster_in->adj[i].adj[j]].color = ARGB32BIT(255,255,255,255);

	//				svVerts.push_back(objShadowCaster_in->plist[i].vert[idx[j]]);
	//				svVerts.push_back(objShadowCaster_in->plist[i].vert[idx[j + 1]]);
	//			}
	//		}
	//	}
	//}


	/*objShadowVol_inout->Init(svVerts.size() * 2, svVerts.size(), 1);

	int currVert = 0;

	for(int i = 0; i < svVerts.size(); i += 2)
	{
		objShadowVol_inout->
	}*/

	for(int i = 0; i < objShadowCaster_in->num_polys; i++)
	{
		if(this->frontBackFace[i] == PROC_CULLED(this->attrBase)) continue;

		if(NOT_PROC(this->frontBackFace[i], this->attrBase))
		{
			POLY4D_PTR currPoly = &objShadowCaster_in->plist[i];

			u.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[1]].v);
			v.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[2]].v);

			u.VECTOR4D_Cross(&v, &n);

			vecScale(-1, &l_in[0].dir, &l);

			dp  = n.VECTOR4D_Dot(&l);

			if(dp > 0)
			{
				this->frontBackFace[i] = FRONT_CULLED(this->attrBase);
			}
			else
			{
				this->frontBackFace[i] = BACK_CULLED(this->attrBase);
			}
		}


		if(this->frontBackFace[i] == FRONT_CULLED(this->attrBase))
		{
			this->frontBackFace[i] = PROC_CULLED(this->attrBase); //треугольник обработан

			for(int j = 0; j < 3; j++)
			{
				int index = objShadowCaster_in->adj[i].adj[j];

				if(NOT_PROC(this->frontBackFace[index], this->attrBase))
				{
					POLY4D_PTR currPoly = &objShadowCaster_in->plist[index];

					u.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[1]].v);
					v.VECTOR4D_Build(&currPoly->vlist[currPoly->vert[0]].v, &currPoly->vlist[currPoly->vert[2]].v);

					u.VECTOR4D_Cross(&v, &n);

					vecScale(-1, &l_in[0].dir, &l);

					dp  = n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						this->frontBackFace[index] = FRONT_CULLED(this->attrBase);
					}
					else
					{
						this->frontBackFace[index] = BACK_CULLED(this->attrBase);
					}
				}

				if(this->frontBackFace[index] == BACK_CULLED(this->attrBase))
				{
					//this->frontBackFace[index] = PROC_CULLED(this->attrBase);

					objShadowCaster_in->plist[i].color = ARGB32BIT(255,0,0,255);
					objShadowCaster_in->plist[index].color = ARGB32BIT(255,255,255,255);

					svVerts.push_back(objShadowCaster_in->plist[i].vert[idx[j]]);
					svVerts.push_back(objShadowCaster_in->plist[i].vert[idx[j + 1]]);
				}
			}
		}

	}

	//return 1;
	//creating shadow volume

	int vertnum = static_cast<int>(svVerts.size()) * 2;
	int pnum = static_cast<int>(svVerts.size());
	int polyAttr = struct3D::POLY4D_ATTR_RGB24 | struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD | struct3D::POLY4D_ATTR_DISABLE_MATERIAL;
	int vertAttr = struct3D::VERTEX4DT_ATTR_NORMAL | struct3D::VERTEX4DT_ATTR_POINT;
	int polyState = struct3D::POLY4D_STATE_ACTIVE | struct3D::OBJECT4D_STATE_VISIBLE;
	int polyColor = ARGB32BIT(127,0,0,0);
	int currVert = 0;
	int svCurr = 0;

	objShadowVol_inout->Destroy();

	objShadowVol_inout->Init(vertnum, pnum, 1);

	vecCopy(&objShadowCaster_in->world_pos, &objShadowVol_inout->world_pos);
	objShadowVol_inout->attr = objShadowCaster_in->attr;
	objShadowVol_inout->state = objShadowCaster_in->state;
	objShadowVol_inout->num_frames = objShadowCaster_in->num_frames;
	objShadowVol_inout->curr_frame = objShadowCaster_in->curr_frame;

	VECTOR4D vl;

	vecCopy(&l_in[0].dir, &vl);
	vecNormalize(&vl);
	vecScale(1000000, &vl);

	for(int i = 0; i < pnum; i+=2)
	{
		//verts
		objShadowVol_inout->vlist_local[currVert].attr = vertAttr;
		vecCopy(&objShadowCaster_in->vlist_local[svVerts[svCurr]].v, &objShadowVol_inout->vlist_local[currVert].v);

		objShadowVol_inout->vlist_local[currVert + 1].attr = vertAttr;
		vecCopy(&objShadowCaster_in->vlist_local[svVerts[svCurr + 1]].v, &objShadowVol_inout->vlist_local[currVert + 1].v);

		objShadowVol_inout->vlist_local[currVert + 2].attr = vertAttr;
		vecCopy(&objShadowCaster_in->vlist_local[svVerts[svCurr + 1]].v, &objShadowVol_inout->vlist_local[currVert + 2].v);
		vecAdd(&objShadowVol_inout->vlist_local[currVert + 2].v, &vl, &objShadowVol_inout->vlist_local[currVert + 2].v);
		//vecSub(&vl, &objShadowVol_inout->vlist_local[currVert + 2].v, &objShadowVol_inout->vlist_local[currVert + 2].v);

		objShadowVol_inout->vlist_local[currVert + 3].attr = vertAttr;
		vecCopy(&objShadowCaster_in->vlist_local[svVerts[svCurr]].v, &objShadowVol_inout->vlist_local[currVert + 3].v);
		vecAdd(&objShadowVol_inout->vlist_local[currVert + 3].v, &vl, &objShadowVol_inout->vlist_local[currVert + 3].v);
		//vecSub(&vl, &objShadowVol_inout->vlist_local[currVert + 3].v, &objShadowVol_inout->vlist_local[currVert + 3].v);



		objShadowVol_inout->plist[i + 1].attr = objShadowVol_inout->plist[i].attr = polyAttr;
		objShadowVol_inout->plist[i + 1].state = objShadowVol_inout->plist[i].state = polyState;
		objShadowVol_inout->plist[i + 1].color = objShadowVol_inout->plist[i].color = polyColor;
		objShadowVol_inout->plist[i + 1].vlist = objShadowVol_inout->plist[i].vlist = objShadowVol_inout->vlist_local;

		//first poly
		objShadowVol_inout->plist[i].vert[0] = currVert + 0;
		objShadowVol_inout->plist[i].vert[1] = currVert + 2;
		objShadowVol_inout->plist[i].vert[2] = currVert + 1;
		//second poly
		objShadowVol_inout->plist[i + 1].vert[0] = currVert + 2;
		objShadowVol_inout->plist[i + 1].vert[1] = currVert + 0;
		objShadowVol_inout->plist[i + 1].vert[2] = currVert + 3;
		//end

		currVert += 4;
		svCurr += 2;
	}

	objShadowVol_inout->ComputePolyNormals();
	objShadowVol_inout->ComputeVertexNormals();

	return 1;
}
