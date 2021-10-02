#include "triInterpI2.h"


triInterpI2::triInterpI2(void)
{
}


triInterpI2::~triInterpI2(void)
{
}

void triInterpI2::setTriangle(POLYF4D_PTR poly)
{
	int v0 = 0, v1 = 1, v2 = 2, tmp;
	x_idx = y_idx = y_prev_idx = 0;//FLT_MIN;
	isRestarted = 0;
	y_idx_row = 0;

	//ñîğòèğîâêà ñâåğõó âíèç
	if(poly->tvlist[v1].y < poly->tvlist[v0].y)
	{
		SWAP(v1, v0, tmp);
	}
	if(poly->tvlist[v2].y < poly->tvlist[v0].y)
	{
		SWAP(v2, v0, tmp);
	}
	if(poly->tvlist[v2].y < poly->tvlist[v1].y)
	{
		SWAP(v2, v1, tmp);
	}

	//Îïğåäåëåíèå òèïà òğåóãîëüíèêà
	//if(ceil(poly->tvlist[v1].y) == ceil(poly->tvlist[v0].y))//FLAT_TOP
	if(math3D::mathFunc.FCMP(poly->tvlist[v1].y, poly->tvlist[v0].y))
	{
		if(poly->tvlist[v1].x < poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);

		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = ceil(poly->tvlist[v0].y);
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = ceil(poly->tvlist[v1].y);
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = ceil(poly->tvlist[v2].y) - 1;
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		dy_inv1 = 1.0f / (poly->tvlist[v2].y - poly->tvlist[v0].y);
		dy_inv2 = 1.0f / (poly->tvlist[v2].y - poly->tvlist[v1].y);
		y1_gen = poly->tvlist[v1].y;
		height = this->vlist[2].y - this->vlist[0].y;
		
		this->triType = TriType3::Simple;
		//this->intiForFlatTop(poly->tvlist[v0].y);
	}
	//else if(ceil(poly->tvlist[v2].y) == ceil(poly->tvlist[v1].y))
	else if(math3D::mathFunc.FCMP(poly->tvlist[v1].y, poly->tvlist[v2].y))
	{
		if(poly->tvlist[v2].x < poly->tvlist[v1].x)
			SWAP(v2, v1, tmp);

		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = ceil(poly->tvlist[v0].y);
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = ceil(poly->tvlist[v1].y) - 1;
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = ceil(poly->tvlist[v2].y) - 1;
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		dy_inv1 = 1.0f / (poly->tvlist[v2].y - poly->tvlist[v0].y);
		dy_inv2 = 1.0f / (poly->tvlist[v1].y - poly->tvlist[v0].y);
		height = this->vlist[2].y - this->vlist[0].y;

		this->triType = TriType3::Simple;
		//this->initForFlatBottom(poly->tvlist[v0].y);
	}
	else
	{
		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = ceil(poly->tvlist[v0].y);
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = ceil(poly->tvlist[v1].y);
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = ceil(poly->tvlist[v2].y) - 1;
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		dy_inv1 = 1.0f / (poly->tvlist[v1].y - poly->tvlist[v0].y);
		dy_inv2 = 1.0f / (poly->tvlist[v2].y - poly->tvlist[v0].y);
		y0_gen = poly->tvlist[v0].y;
		y1_gen = poly->tvlist[v1].y;
		y2_gen = poly->tvlist[v2].y;
		height = this->vlist[2].y - this->vlist[0].y;

		//this->intiForGeneral(poly->tvlist[v0].y);
	}
}