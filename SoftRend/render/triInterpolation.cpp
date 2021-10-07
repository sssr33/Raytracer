#include "triInterpolation.h"


triInterpolation::triInterpolation(void)
{
	//tri[0] = tri[1] = tri[2] = 0;
}


triInterpolation::~triInterpolation(void)
{
}

void triInterpolation::setTriangle(POLYF4D_PTR poly)
{
	int v0 = 0, v1 = 1, v2 = 2, tmp;
	//ZeroMemory(this, sizeof(triInterpolation));
	d_correction = 1;
	x_idx = y_idx = y_prev_idx = FLT_MIN;

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

	//Определение типа треугольника
	if(poly->tvlist[v1].y == poly->tvlist[v0].y)//FLAT_TOP
	{
		if(poly->tvlist[v1].x < poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);

		/*tri[0] = &poly->tvlist[v0];
		tri[1] = &poly->tvlist[v1];
		tri[2] = &poly->tvlist[v2];*/

		this->vlist[0].x = static_cast<float>((int)(poly->tvlist[v0].x + 0.5f));
		this->vlist[0].y = static_cast<float>((int)(poly->tvlist[v0].y + 0.5f));
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = static_cast<float>((int)(poly->tvlist[v1].x + 0.5f));
		this->vlist[1].y = static_cast<float>((int)(poly->tvlist[v1].y + 0.5f));
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = static_cast<float>((int)(poly->tvlist[v2].x + 0.5f));
		this->vlist[2].y = static_cast<float>((int)(poly->tvlist[v2].y + 0.5f));
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		RGBFROM32BIT(poly->lit_color[v0], rBase[0], gBase[0], bBase[0]);
		RGBFROM32BIT(poly->lit_color[v1], rBase[1], gBase[1], bBase[1]);
		RGBFROM32BIT(poly->lit_color[v2], rBase[2], gBase[2], bBase[2]);
		
		this->triType = TriType::FlatTop;
		this->intiForFlatTop();
	}
	else if(poly->tvlist[v2].y == poly->tvlist[v1].y)
	{
		if(poly->tvlist[v2].x < poly->tvlist[v1].x)
			SWAP(v2, v1, tmp);

		this->vlist[0].x = static_cast<float>((int)(poly->tvlist[v0].x + 0.5f));
		this->vlist[0].y = static_cast<float>((int)(poly->tvlist[v0].y + 0.5f));
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = static_cast<float>((int)(poly->tvlist[v1].x + 0.5f));
		this->vlist[1].y = static_cast<float>((int)(poly->tvlist[v1].y + 0.5f));
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = static_cast<float>((int)(poly->tvlist[v2].x + 0.5f));
		this->vlist[2].y = static_cast<float>((int)(poly->tvlist[v2].y + 0.5f));
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		RGBFROM32BIT(poly->lit_color[v0], rBase[0], gBase[0], bBase[0]);
		RGBFROM32BIT(poly->lit_color[v1], rBase[1], gBase[1], bBase[1]);
		RGBFROM32BIT(poly->lit_color[v2], rBase[2], gBase[2], bBase[2]);

		this->triType = TriType::FlatBottom;
		this->initForFlatBottom();
	}
	else
	{
		this->vlist[0].x = static_cast<float>((int)(poly->tvlist[v0].x + 0.5f));
		this->vlist[0].y = static_cast<float>((int)(poly->tvlist[v0].y + 0.5f));
		this->vlist[0].z = 1.0f / poly->tvlist[v0].z;

		this->vlist[1].x = static_cast<float>((int)(poly->tvlist[v1].x + 0.5f));
		this->vlist[1].y = static_cast<float>((int)(poly->tvlist[v1].y + 0.5f));
		this->vlist[1].z = 1.0f / poly->tvlist[v1].z;

		this->vlist[2].x = static_cast<float>((int)(poly->tvlist[v2].x + 0.5f));
		this->vlist[2].y = static_cast<float>((int)(poly->tvlist[v2].y + 0.5f));
		this->vlist[2].z = 1.0f / poly->tvlist[v2].z;

		RGBFROM32BIT(poly->lit_color[v0], rBase[0], gBase[0], bBase[0]);
		RGBFROM32BIT(poly->lit_color[v1], rBase[1], gBase[1], bBase[1]);
		RGBFROM32BIT(poly->lit_color[v2], rBase[2], gBase[2], bBase[2]);

		this->triType = TriType::General;
		this->intiForGeneral();
	}
}

void triInterpolation::initForFlatBottom()
{
	this->y_restart = FLT_MAX;

	/*dy_inv1 = 1.0f / (tri[1]->y - tri[0]->y);

	dxdyl = (tri[1]->x - tri[0]->x) * dy_inv1;
	dxdyr = (tri[2]->x - tri[0]->x) * dy_inv1;*/
	dy_inv1 = 1.0f / (vlist[1].y - vlist[0].y);

	dxdyl = (vlist[1].x - vlist[0].x) * dy_inv1;
	dxdyr = (vlist[2].x - vlist[0].x) * dy_inv1;

	dzdyl = (vlist[1].z - vlist[0].z) * dy_inv1;
	dzdyr = (vlist[2].z - vlist[0].z) * dy_inv1;

	dcdyl.M[0] = (rBase[1] - rBase[0]) * dy_inv1;//r
	dcdyl.M[1] = (gBase[1] - gBase[0]) * dy_inv1;//g
	dcdyl.M[2] = (bBase[1] - bBase[0]) * dy_inv1;//b

	dcdyr.M[0] = (rBase[2] - rBase[0]) * dy_inv1;
	dcdyr.M[1] = (gBase[2] - gBase[0]) * dy_inv1;
	dcdyr.M[2] = (bBase[2] - bBase[0]) * dy_inv1;
}
void triInterpolation::intiForFlatTop()
{
	/*this->y_restart = tri[1]->y - 1;

	dy_inv1 = 1.0f / (tri[2]->y - tri[0]->y);

	dxdyl2 = (tri[2]->x - tri[1]->x) * dy_inv1;
	dxdyr = (tri[2]->x - tri[0]->x) * dy_inv1;*/
	this->y_restart = vlist[1].y - 1;

	dy_inv1 = 1.0f / (vlist[2].y - vlist[0].y);

	dxdyl2 = (vlist[2].x - vlist[1].x) * dy_inv1;
	dxdyr = (vlist[2].x - vlist[0].x) * dy_inv1;

	dzdyl2 = (vlist[2].z - vlist[1].z) * dy_inv1;
	dzdyr = (vlist[2].z - vlist[0].z) * dy_inv1;

	dcdyl2.M[0] = (rBase[2] - rBase[1]) * dy_inv1;
	dcdyl2.M[1] = (gBase[2] - gBase[1]) * dy_inv1;
	dcdyl2.M[2] = (bBase[2] - bBase[1]) * dy_inv1;

	dcdyr.M[0] = (rBase[2] - rBase[0]) * dy_inv1;
	dcdyr.M[1] = (gBase[2] - gBase[0]) * dy_inv1;
	dcdyr.M[2] = (bBase[2] - bBase[0]) * dy_inv1;

	d_correction = -1;
}
void triInterpolation::intiForGeneral()
{
	/*this->y_restart = tri[1]->y;

	dy_inv1 = 1.0f / (tri[1]->y - tri[0]->y);
	dy_inv2 = 1.0f / (tri[2]->y - tri[0]->y);
	dy_inv3 = 1.0f / (tri[2]->y - tri[1]->y);

	dxdyl = (tri[1]->x - tri[0]->x) * dy_inv1;
	dxdyr = (tri[2]->x - tri[0]->x) * dy_inv2;
	dxdyl2 = (tri[2]->x - tri[1]->x) * dy_inv3;*/
	this->y_restart = vlist[1].y;

	dy_inv1 = 1.0f / (vlist[1].y - vlist[0].y);
	dy_inv2 = 1.0f / (vlist[2].y - vlist[0].y);
	dy_inv3 = 1.0f / (vlist[2].y - vlist[1].y);

	dxdyl = (vlist[1].x - vlist[0].x) * dy_inv1;
	dxdyr = (vlist[2].x - vlist[0].x) * dy_inv2;
	dxdyl2 = (vlist[2].x - vlist[1].x) * dy_inv3;

	dzdyl = (vlist[1].z - vlist[0].z) * dy_inv1;
	dzdyr = (vlist[2].z - vlist[0].z) * dy_inv2;
	dzdyl2 = (vlist[2].z - vlist[1].z) * dy_inv3;

	dcdyl.M[0] = (rBase[1] - rBase[0]) * dy_inv1;
	dcdyl.M[1] = (gBase[1] - gBase[0]) * dy_inv1;
	dcdyl.M[2] = (bBase[1] - bBase[0]) * dy_inv1;

	dcdyr.M[0] = (rBase[2] - rBase[0]) * dy_inv2;
	dcdyr.M[1] = (gBase[2] - gBase[0]) * dy_inv2;
	dcdyr.M[2] = (bBase[2] - bBase[0]) * dy_inv2;

	dcdyl2.M[0] = (rBase[2] - rBase[1]) * dy_inv3;
	dcdyl2.M[1] = (gBase[2] - gBase[1]) * dy_inv3;
	dcdyl2.M[2] = (bBase[2] - bBase[1]) * dy_inv3;

	if(dxdyl > dxdyr)
	{
		d_correction = -1;//в этом случае координаты выдаються с права на лево, что бы не выполнять перестановку
	}
}

void triInterpolation::setIndexY(int y_idx)
{
	this->y_idx = static_cast<float>(y_idx);

	if(this->y_idx != this->y_prev_idx)
	{
		//if((tri[0]->y + y_idx) > y_restart)//В случае общего и с плоским верхом треугольника
		if((vlist[0].y + y_idx) > y_restart)//В случае общего и с плоским верхом треугольника
		{
			/*float idx_for_l = (y_idx - (tri[1]->y - tri[0]->y));
			xl = idx_for_l * dxdyl2 + tri[1]->x;
			xr = y_idx * dxdyr + tri[0]->x;*/
			float idx_for_l = ((float)y_idx - (vlist[1].y - vlist[0].y));
			xl = idx_for_l * dxdyl2 + vlist[1].x;
			xr = y_idx * dxdyr + vlist[0].x;

			zl = idx_for_l * dzdyl2 + vlist[1].z;
			zr = y_idx * dzdyr + vlist[0].z;

			cl.M[0] = idx_for_l * dcdyl2.M[0] + rBase[1];//r
			cl.M[1] = idx_for_l * dcdyl2.M[1] + gBase[1];//g
			cl.M[2] = idx_for_l * dcdyl2.M[2] + bBase[1];//b

			cr.M[0] = y_idx * dcdyr.M[0] + rBase[0];//r
			cr.M[1] = y_idx * dcdyr.M[1] + gBase[0];//g
			cr.M[2] = y_idx * dcdyr.M[2] + bBase[0];//b

			x_start = static_cast<float>((int)(xl + 0.5f));
			x_end = static_cast<float>((int)(xr + 0.5f));

			/*if((dx = (xr * d_correction) - (xl * d_correction)) > 0)*/
			/*if((dx = ((int)(xr + 0.5f) * d_correction) - ((int)(xl + 0.5f) * d_correction)) > 0)*/
			if((dx = (x_end * d_correction - x_start * d_correction)) > 0)
			{
				dx_inv = 1.0f / dx;

				dz = (zr - zl) * dx_inv;

				dc.M[0] = ((cr.M[0] /** d_correction*/) - (cl.M[0] /** d_correction*/)) * dx_inv;
				dc.M[1] = ((cr.M[1] /** d_correction*/) - (cl.M[1] /** d_correction*/)) * dx_inv;
				dc.M[2] = ((cr.M[2] /** d_correction*/) - (cl.M[2] /** d_correction*/)) * dx_inv;
			}
			else
			{
				//dx = 0;
				//dc.M[0] = 0;//((cr.M[0] /** d_correction*/) - (cl.M[0] /** d_correction*/));
				//dc.M[1] = 0;//((cr.M[1] /** d_correction*/) - (cl.M[1] /** d_correction*/));
				//dc.M[2] = 0;//((cr.M[2] /** d_correction*/) - (cl.M[2] /** d_correction*/));
				dz = (zr * d_correction - zl * d_correction);
				dc.M[0] = ((cr.M[0] * d_correction) - (cl.M[0] * d_correction));
				dc.M[1] = ((cr.M[1] * d_correction) - (cl.M[1] * d_correction));
				dc.M[2] = ((cr.M[2] * d_correction) - (cl.M[2] * d_correction));
			}
		}
		else//В случае общего и с плоским низом треугольника
		{
			/*xl = y_idx * dxdyl + tri[0]->x;
			xr = y_idx * dxdyr + tri[0]->x;*/
			xl = y_idx * dxdyl + vlist[0].x;
			xr = y_idx * dxdyr + vlist[0].x;
			/*dx = ((xr * d_correction) - (xl * d_correction));
			dx_inv = 1.0f / dx;*/

			zl = y_idx * dzdyl + vlist[0].z;
			zr = y_idx * dzdyr + vlist[0].z;

			cl.M[0] = y_idx * dcdyl.M[0] + rBase[0];//r
			cl.M[1] = y_idx * dcdyl.M[1] + gBase[0];//g
			cl.M[2] = y_idx * dcdyl.M[2] + bBase[0];//b

			cr.M[0] = y_idx * dcdyr.M[0] + rBase[0];//r
			cr.M[1] = y_idx * dcdyr.M[1] + gBase[0];//g
			cr.M[2] = y_idx * dcdyr.M[2] + bBase[0];//b

			x_start = static_cast<float>((int)(xl + 0.5f));
			x_end = static_cast<float>((int)(xr + 0.5f));

			/*if((dx = (xr * d_correction) - (xl * d_correction)) > 0)*/
			/*if((dx = ((int)(xr + 0.5f) * d_correction) - ((int)(xl + 0.5f) * d_correction)) > 0)*/
			if((dx = (x_end * d_correction - x_start * d_correction)) > 0)
			{
				dx_inv = 1.0f / dx;

				dz = (zr - zl) * dx_inv;

				dc.M[0] = ((cr.M[0] /** d_correction*/) - (cl.M[0] /** d_correction*/)) * dx_inv;
				dc.M[1] = ((cr.M[1] /** d_correction*/) - (cl.M[1] /** d_correction*/)) * dx_inv;
				dc.M[2] = ((cr.M[2] /** d_correction*/) - (cl.M[2] /** d_correction*/)) * dx_inv;
			}
			else
			{
				//dx = 0;

				dz = (zr * d_correction - zl * d_correction);

				dc.M[0] = ((cr.M[0] * d_correction) - (cl.M[0] * d_correction));
				dc.M[1] = ((cr.M[1] * d_correction) - (cl.M[1] * d_correction));
				dc.M[2] = ((cr.M[2] * d_correction) - (cl.M[2] * d_correction));
			}
		}

		this->y_prev_idx = this->y_idx;
	}
}

void triInterpolation::setIndexX(int x_idx)
{
	this->x_idx = static_cast<float>(x_idx);
}

void triInterpolation::getCoords(int *x_out, int *y_out, float *z_inv_out)
{
	/**x_out = (xl  + 0.5 + (x_idx * d_correction));*/
	*x_out = static_cast<int>(x_start + (x_idx * d_correction));
	/**y_out = y_idx + tri[0]->y;*/
	*y_out = static_cast<int>(y_idx + (int)vlist[0].y);
	if(z_inv_out)
		*z_inv_out = zl + (dz * x_idx);
}
void triInterpolation::getColor(int *c_out)
{
	*c_out = 0;
	/*int r = cl.M[0] + (dc.M[0] * (x_idx * d_correction));
	int g = cl.M[1] + (dc.M[1] * (x_idx * d_correction));
	int b = cl.M[2] + (dc.M[2] * (x_idx * d_correction));*/
	int r = static_cast<int>(cl.M[0] + (dc.M[0] * x_idx));
	int g = static_cast<int>(cl.M[1] + (dc.M[1] * x_idx));
	int b = static_cast<int>(cl.M[2] + (dc.M[2] * x_idx));

	*c_out = ARGB32BIT(255, r, g, b);
}

TriType triInterpolation::getTriType()
{
	return this->triType;
}

int triInterpolation::getDX()
{
	return static_cast<int>(this->dx /*+ 1.0f*/);
}

int triInterpolation::getXL()
{
	//return (this->xl /*+ 0.5f*/);	
	return static_cast<int>(this->x_start /*+ 0.5f*/);
}
int triInterpolation::getXR()
{
	//return (this->xr /*+ 0.5f*/);
	return static_cast<int>(this->x_end /*+ 0.5f*/);
}