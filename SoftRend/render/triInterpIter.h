#pragma once
//#include "structures.h"
#include "lineInterp.h"
#include "stdafx.h"

//using namespace struct3D;
//using namespace mat;

enum TriType2
{
	Simple, GeneralLHS, GeneralRHS
};

class triInterpIter
{
public:
	triInterpIter(void);
	~triInterpIter(void);

	void setTriangle(POLYF4D_PTR poly);
	void initForFlatBottom(float y0);
	void intiForFlatTop(float y0);
	void intiForGeneral(float y0);

	void setIndexY(int y_idx);
	void setIndexX(int x_idx);
	void getZInv(float *z);
	int getXL();
	int getXR();
	void getCoords(int *x_out, int *y_out, float *z_inv_out);
	void getColor(int *color_out);

	float x_idx, y_idx, y_prev_idx;
	float xs, xe;
	float height;
	float heightHalf;
	float dy_inv1;
	float dy_inv2;
	float y0_gen;
	float y1_gen;
	float y2_gen;
	bool isRestarted;
	TriType2 triType;
	float y_idx_r, y_idx_l;
	float y_idx_row;

	float dxdyl, dxdyr, xl, xr, dx_inv, dx;

	POINT4D vlist[3];
	float rBase[3];
	float gBase[3];
	float bBase[3];

	VECTOR4D dcdyl, dcdyr, cl, cr, cs, ce, dc;
	float dzdyl, dzdyr, zl, zr, zs, ze, dz;

	//new
	bool linv, rinv;

	//new_new
	lineInterp le, re;
	float yc0, yc1, yc2;
};

