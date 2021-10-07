#include "triInterpIter.h"


triInterpIter::triInterpIter(void)
{
}


triInterpIter::~triInterpIter(void)
{
}

void triInterpIter::setTriangle(POLYF4D_PTR poly)
{
	int v0 = 0, v1 = 1, v2 = 2, tmp;
	x_idx = y_idx = y_prev_idx = FLT_MIN;
	isRestarted = 0;
	y_idx_row = 0;
	linv = rinv = false;

	//сортировка сверху вниз
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

	float k = 16;

	//Определение типа треугольника
	if(ceil(poly->tvlist[v1].y) == ceil(poly->tvlist[v0].y))//FLAT_TOP
	//if(math3D::mathFunc.FCMP(poly->tvlist[v1].y, poly->tvlist[v0].y))
	//if(poly->tvlist[v1].y == poly->tvlist[v0].y)
	{
		if(poly->tvlist[v1].x < poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);

		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = ceil(poly->tvlist[v0].y);
		this->vlist[0].z = (1.0f * k) / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = ceil(poly->tvlist[v1].y);
		this->vlist[1].z = (1.0f * k) / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = ceil(poly->tvlist[v2].y) - 1;
		this->vlist[2].z = (1.0f * k) / poly->tvlist[v2].z;

		dy_inv1 = (poly->tvlist[v2].y - poly->tvlist[v0].y);
		dy_inv2 = (poly->tvlist[v2].y - poly->tvlist[v1].y);
		y0_gen = poly->tvlist[v0].y;
		y1_gen = poly->tvlist[v1].y;
		y2_gen = poly->tvlist[v2].y;
		height = this->vlist[2].y - this->vlist[0].y;
		
		this->triType = TriType2::Simple;
		this->intiForFlatTop(poly->tvlist[v0].y);

		le.init(&poly->tvlist[v0].v, &poly->tvlist[v2].v, true);
		re.init(&poly->tvlist[v1].v, &poly->tvlist[v2].v, true);
		yc0 = ceil(poly->tvlist[v0].y);
		yc1 = ceil(poly->tvlist[v1].y);
		yc2 = ceil(poly->tvlist[v2].y) - 1;
	}
	else if(ceil(poly->tvlist[v2].y) == ceil(poly->tvlist[v1].y))//BOTTOM
	//else if(math3D::mathFunc.FCMP(poly->tvlist[v1].y, poly->tvlist[v2].y))
	//else if(poly->tvlist[v2].y == poly->tvlist[v1].y)
	{
		if(poly->tvlist[v2].x < poly->tvlist[v1].x)
			SWAP(v2, v1, tmp);
		
		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = ceil(poly->tvlist[v0].y);
		this->vlist[0].z = (1.0f * k) / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = ceil(poly->tvlist[v1].y) - 1;
		this->vlist[1].z = (1.0f * k) / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = ceil(poly->tvlist[v2].y) - 1;
		this->vlist[2].z = (1.0f * k) / poly->tvlist[v2].z;

		dy_inv1 = (poly->tvlist[v2].y - poly->tvlist[v0].y);
		dy_inv2 = (poly->tvlist[v1].y - poly->tvlist[v0].y);
		y0_gen = poly->tvlist[v0].y;
		y1_gen = poly->tvlist[v1].y;
		y2_gen = poly->tvlist[v2].y;
		height = this->vlist[2].y - this->vlist[0].y;

		this->triType = TriType2::Simple;
		this->initForFlatBottom(poly->tvlist[v0].y);

		le.init(&poly->tvlist[v0].v, &poly->tvlist[v1].v, true);
		re.init(&poly->tvlist[v0].v, &poly->tvlist[v2].v, true);
		yc0 = ceil(poly->tvlist[v0].y);
		yc1 = ceil(poly->tvlist[v1].y);
		yc2 = ceil(poly->tvlist[v2].y) - 1;
	}
	else
	{
		
		this->vlist[0].x = (poly->tvlist[v0].x);
		this->vlist[0].y = poly->tvlist[v0].y;
		this->vlist[0].z = (1.0f * k) / poly->tvlist[v0].z;

		this->vlist[1].x = (poly->tvlist[v1].x);
		this->vlist[1].y = (poly->tvlist[v1].y);
		this->vlist[1].z = (1.0f * k) / poly->tvlist[v1].z;

		this->vlist[2].x = (poly->tvlist[v2].x);
		this->vlist[2].y = (poly->tvlist[v2].y);
		this->vlist[2].z = (1.0f * k) / poly->tvlist[v2].z;

		yc0 = ceil(poly->tvlist[v0].y);
		yc1 = ceil(poly->tvlist[v1].y);
		yc2 = ceil(poly->tvlist[v2].y) - 1;

		height = yc2 - yc0;
		heightHalf = yc1 - yc0;

		this->intiForGeneral(poly->tvlist[v0].y);
	}
}

void triInterpIter::initForFlatBottom(float y0)
{
	if(this->vlist[1].x > this->vlist[0].x)
		this->dxdyl = (this->vlist[1].x - this->vlist[0].x) / dy_inv2;
	else
	{
		this->dxdyl = (this->vlist[0].x - this->vlist[1].x) / dy_inv2;
		linv = true;
	}

	if(this->vlist[2].x > this->vlist[0].x)
		this->dxdyr = (this->vlist[2].x - this->vlist[0].x) / dy_inv1;
	else
	{
		this->dxdyr = (this->vlist[0].x - this->vlist[2].x) / dy_inv1;
		rinv = true;
	}

	y_idx_r = y_idx_l = 0;

	//correction
	float y_correction = 0;

	TCHAR mas[128];

	/*float t = 1.0f;
	float tf = (t * vlist[1].x) + ((1.0f - t) * vlist[0].x);

	if(tf != vlist[1].x) MessageBox(0, L"ALPHA FAIL",0,0);*/

	float dxd = 1.0f / dy_inv2;
	float t = dxd * dy_inv2;

	/**if(t != 1.0f) 
	{
		swprintf(mas, L"%f", t);
		MessageBox(0, mas, 0, 0);
	}*/
	/*float tf = (t * vlist[1].x) + ((1.0f - t) * vlist[0].x);*/


	float v = this->vlist[1].x - this->vlist[0].x;
	float tf = vlist[0].y + height;

	if(tf != vlist[1].y) 
	{	
		swprintf(mas, std::size(mas), L"real %f <<< %f H: %f", vlist[1].y, height, dy_inv2);
		MessageBox(0, mas,0,0);
	}

	if(linv)
	{
		y_correction = y1_gen - this->vlist[1].y;
		xl = xs = this->vlist[1].x + this->dxdyl * y_correction;

		float tstH = height + (this->vlist[0].y - y0_gen);
		float tst = this->vlist[1].x + this->dxdyl * dy_inv2;//xl + dxdyl * tstH;
		/*if(tst != this->vlist[0].x) 
		{
			swprintf(mas, L"BT linv xl error tst: %f x: %f", tst, this->vlist[0].x);
			MessageBox(0,mas,0,0);
		}*/
	}
	else
	{
		y_correction = this->vlist[0].y - y0_gen;
		xl = xs = this->vlist[0].x + this->dxdyl * y_correction;

		float tstH = height + (y1_gen - this->vlist[1].y);
		float tst = this->vlist[0].x + this->dxdyl * dy_inv2;//xl + dxdyl * tstH;
		//if(tst != this->vlist[1].x) MessageBox(0,L"BT xl error",0,0);
	}

	if(rinv)
	{
		y_correction = y2_gen - this->vlist[2].y;
		xr = xe = this->vlist[2].x + this->dxdyr * y_correction;

		float tstH = height + (this->vlist[0].y - y0_gen);
		float tst = xr + dxdyr * tstH;
		//if(tst != this->vlist[0].x) MessageBox(0,L"BT rinv xr error",0,0);
	}
	else
	{
		y_correction = this->vlist[0].y - y0_gen;
		xr = xe = this->vlist[0].x + this->dxdyr * y_correction;

		float tstH = height + (y2_gen - this->vlist[2].y);
		float tst = this->vlist[0].x + this->dxdyr * dy_inv1;//xr + dxdyr * tstH;
		//if(tst != this->vlist[2].x) MessageBox(0,L"BT xr error",0,0);
	}
}
void triInterpIter::intiForFlatTop(float y0)
{
	if(this->vlist[2].x > this->vlist[0].x)
		this->dxdyl = (this->vlist[2].x - this->vlist[0].x) / dy_inv1;
	else
	{
		this->dxdyl = (this->vlist[0].x - this->vlist[2].x) / dy_inv1;
		linv = true;
	}

	if(this->vlist[2].x > this->vlist[1].x)
		this->dxdyr = (this->vlist[2].x - this->vlist[1].x) / dy_inv2;
	else
	{
		this->dxdyr = (this->vlist[1].x - this->vlist[2].x) / dy_inv2;
		rinv = true;
	}

	y_idx_r = y_idx_l = 0;

	//correction
	float y_correction = 0;

	if(linv)
	{
		y_correction = y2_gen - this->vlist[2].y;
		xl = xs = this->vlist[2].x + y_correction * dxdyl;
	}
	else
	{
		y_correction = this->vlist[0].y - y0_gen;
		xl = xs = this->vlist[0].x + this->dxdyl * y_correction;
	}

	if(rinv)
	{
		y_correction = y2_gen - this->vlist[2].y;
		xr = xe = this->vlist[2].x + y_correction * dxdyr;
	}
	else
	{
		y_correction = this->vlist[1].y - y1_gen;
		xr = xe = this->vlist[1].x + this->dxdyr * y_correction;
	}
}

void triInterpIter::intiForGeneral(float y0)
{
	this->dxdyl = (this->vlist[1].x - this->vlist[0].x) / (this->vlist[1].y - this->vlist[0].y);
	this->dxdyr = (this->vlist[2].x - this->vlist[0].x) / (this->vlist[2].y - this->vlist[0].y);

	if(dxdyl > dxdyr) //RHS
	{
		this->triType = TriType2::GeneralRHS;
		
		le.init(&this->vlist[0], &this->vlist[2], true);
		re.init(&this->vlist[0], &this->vlist[1], true);

		height = le.height;
		heightHalf = re.height + 1;
	}
	else//LHS
	{
		this->triType = TriType2::GeneralLHS;
		
		le.init(&this->vlist[0], &this->vlist[1], true);
		re.init(&this->vlist[0], &this->vlist[2], true);

		height = re.height;
		heightHalf = le.height + 1;
	}

	y_idx_r = y_idx_l = 0;
}

void triInterpIter::setIndexY(int y_idx)
{
	this->y_idx = y_idx;
	
	if(this->triType == TriType2::Simple)
	{
		if(this->y_idx != this->y_prev_idx)
		{
			this->xs = le.getX(this->y_idx);
			this->xe = re.getX(this->y_idx);

			/*TCHAR mas[128];
			swprintf(mas, L"%f || %f || %f || %f >> %f",this->y_idx, dxdyl, height - this->y_idx,  xs, xe);
			MessageBox(0, mas, 0, 0);*/

			this->y_prev_idx = this->y_idx;
		}
	}
	else if(this->triType == TriType2::GeneralLHS)
	{
		if(!isRestarted)
		{
			if((yc0 + this->y_idx) >= yc1)//restart y
			{
				le.init(&this->vlist[1], &this->vlist[2], true);

				y_idx_l = heightHalf;

				isRestarted = 1;
			}
		}

		if(this->y_idx != this->y_prev_idx)
		{
			this->xs = le.getX(this->y_idx - this->y_idx_l);
			this->xe = re.getX(this->y_idx);

			/*TCHAR mas[128];
			swprintf(mas, L"%f || %f || %f >> %f",this->y_idx, height - this->y_idx,  xs, xe);
			MessageBox(0, mas, 0, 0);*/

			this->y_prev_idx = this->y_idx;
		}
	}
	else//RHS
	{
		if(!isRestarted)
		{
			if((yc0 + this->y_idx) >= yc1)//restart y
			{
				re.init(&this->vlist[1], &this->vlist[2], true);

				y_idx_r = heightHalf;

				isRestarted = 1;

			}
		}

		if(this->y_idx != this->y_prev_idx)
		{
			this->xs = le.getX(this->y_idx);
			this->xe = re.getX(this->y_idx - this->y_idx_r);

			/*TCHAR mas[128];
			swprintf(mas, L"%f || %f || %f >> %f",this->y_idx, height - this->y_idx,  xs, xe);
			MessageBox(0, mas, 0, 0);*/

			this->y_prev_idx = this->y_idx;
		}
	}

}
void triInterpIter::setIndexX(int x_idx)
{
	this->x_idx = x_idx;
}

void triInterpIter::getColor(int *color_out)
{
	*color_out = ARGB32BIT(127, 0, 0, 0);  
}

int triInterpIter::getXL()
{
	return /*(int)*/(ceil(this->xs));
}
int triInterpIter::getXR()
{
	return /*(int)*/(ceil(this->xe) - 1.0f);
}

void triInterpIter::getZInv(float *z)
{
	*z = zs + dz * x_idx;
}

void triInterpIter::getCoords(int *x_out, int *y_out, float *z_inv_out)
{
	/**y_out = (this->vlist[0].y + this->y_idx);*/
	*y_out = (yc0 + this->y_idx);
}