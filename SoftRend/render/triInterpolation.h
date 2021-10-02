#pragma once
#include "structures.h"
#include "stdafx.h"

using namespace struct3D;
using namespace mat;

enum TriType
{
	FlatTop, FlatBottom, General
};

class triInterpolation
{
public:
	triInterpolation(void);
	~triInterpolation(void);

	//void setTriangle(VERTEX4DT_PTR v0, VERTEX4DT_PTR v1, VERTEX4DT_PTR v2);
	void setTriangle(POLYF4D_PTR poly);
	void initForFlatBottom();
	void intiForFlatTop();
	void intiForGeneral();

	void setIndexY(int y_idx);
	void setIndexX(int x_idx);
	void getCoords(int *x_out, int *y_out, float *z_inv_out);
	void getColor(int *c_out);
	int getDX();
	int getXL();
	int getXR();
	TriType getTriType();

	//VERTEX4DT_PTR tri[3];
	POINT4D vlist[3];
	int rBase[3];
	int gBase[3];
	int bBase[3];
	float dy_inv1;
	float dy_inv2;
	float dy_inv3;
	float d_correction;
	float y_restart;
	float x_idx, y_idx, y_prev_idx;
	float x_start, x_end;
	TriType triType;

	//std interpolation values:
	float dxdyl, dxdyl2, dxdyr, xl, xr, dx_inv, dx;
	float dzdyl, dzdyl2, dzdyr, zl, zr, dz;
	VECTOR4D dcdyl, dcdyl2, dcdyr, cl ,cr, dc; //color
	//VECTOR2D dtdyl, dtdyr, tl, tr; //texture
	//std interpolation values
};

