#pragma once
#include "structures.h"
#include "Math3DStructs.h"

using namespace struct3D;
using namespace mat;
using namespace math3D;

//#define FRONT_CULLED(base) (base)
//#define BACK_CULLED(base) (base + 1)
//#define PROC_CULLED(base) (base - 4)

#define NOT_PROC(x, base) (x < base)
#define FRONT_CULLED(base) (base + 1)
#define BACK_CULLED(base) (base + 2)
#define PROC_CULLED(base) (base + 3)

class GeometryGen
{
private:
	//shadow volume
	unsigned short attrBase;
	std::vector<unsigned short> frontBackFace;
	//shadow volume
public:
	GeometryGen(void);
	~GeometryGen(void);

	int generateTerrainOBJECT4D(OBJECT4D_PTR obj, float width, float height, float scale, TEXTURE2D_PTR tex2DHMap, TEXTURE2D_PTR tex2D, unsigned int color, POINT4D_PTR pos, VECTOR4D_PTR rot, unsigned int attr);
	int generateRotationForm(POINT4D_PTR pts, int numPts, int numSides, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor);

	int generateCylinder(int numSides, int numHSegs, int numCapSegs, float radius, float height, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor);
	int generateSphere(int numSegs, float radius, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor);
	int generateCone(int numSides, int numHSegs, int numCapSegs, float radiusT, float radiusB, float height, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor);
	int generateBox(float length, float width, float height, int LSegs, int WSegs, int HSegs, OBJECT4D_PTR obj, POINT4D_PTR worldPos, unsigned int iColor);

	int generateShadowVolume(LIGHT_PTR l_in, int num_l_in, OBJECT4D_PTR objShadowCaster_in, OBJECT4D_PTR objShadowVol_inout = 0, int sh_vol_num_in = 0);
};

