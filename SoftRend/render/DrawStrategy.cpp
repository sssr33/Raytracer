// 🐚
#include "StdAfx.h"
#include "DrawStrategy.h"
#include "Math3DStructs.h"
#include <math.h>


DrawStrategy::DrawStrategy(void)
{
}


DrawStrategy::~DrawStrategy(void)
{
}

Draw32BitStrategy::Draw32BitStrategy(/*unsigned int *SSEInfo, */unsigned int graphicQuality)
{
	//if(SSEInfo[0] != 0 && SSEInfo[1] != 0){
	if(Sys::getSIMDInfo(SIMD::SSE1) && Sys::getSIMDInfo(SIMD::SSE2))
	{
		//_alphaBlender = new AlphaBlendSSE32BitStrategy();
		_alphaBlender = new AlphaBlendSlow32BitStrategy();
		if(graphicQuality != 0){
			_drawLine = new DrawSoftLineSSE32BitStrategy();
		}
		else{
			_drawLine = new DrawSimpleLine32BitStrategy();
		}
	}
	else{
		_alphaBlender = new AlphaBlendSlow32BitStrategy();
		if(graphicQuality != 0){
			_drawLine = new DrawSoftLine32BitStrategy();
		}
		else{
			_drawLine = new DrawSimpleLine32BitStrategy();
		}
	}

	//_alphaBlender = new AlphaBlendSSE32BitStrategy();
	//_drawLine = new DrawSimpleLine32BitStrategy();

	maxClipX = maxClipY = 0;
	this->ppObjMap = 0;
}


Draw32BitStrategy::~Draw32BitStrategy(void)
{
	delete _alphaBlender;
	_alphaBlender = 0;

	delete _drawLine;
	_drawLine = 0;
}

int Draw32BitStrategy::Release()
{
	delete _alphaBlender;
	_alphaBlender = 0;

	delete _drawLine;
	_drawLine = 0;

	zbuffer.release();

	return 1;
}

int Draw32BitStrategy::setScreenSize(int width, int height)
{
	this->scrWidth = width;
	this->scrHeight = height;

	zbuffer.release();
	zbuffer.initialize(width, height);

	return 1;
}

int Draw32BitStrategy::DrawLine(int x0, int y0, int x1, int y1, unsigned int color, void *videoMemory, int lpitch){

	_drawLine->DrawLine(x0,y0,x1,y1,color,videoMemory,lpitch,_alphaBlender);

	return 1;
}

int Draw32BitStrategy::DrawLine(VERTEX4DT_PTR v0, VERTEX4DT_PTR v1, unsigned int color, void *videoMemory, int lpitch)
{
	float dx = fabs(v1->x - v0->x);
	float dy = fabs(v1->y - v0->y);
	float dz_inv;// = 1.0f / (v1->z - v0->z);
	VERTEX4DT_PTR vTmp;

	lpitch >>= 2;

	unsigned int *vb = (unsigned int *)videoMemory;

	float zi, xi, yi;

	/*RECT r;
	r.left = this->minClipX + 2;
	r.top = this->minClipY + 2;
	r.right = this->maxClipX - 2;
	r.bottom = this->maxClipY - 2;*/

	//if(!this->clipLineCS(&r, &v0->v, &v1->v)) return 0;

	if(dx > dy)
	{
		if(v1->x < v0->x)
		{
			SWAP(v0, v1, vTmp);
		}

		//MessageBox(0,0,0,0);

		dy = (v1->y - v0->y) / dx;
		yi = v0->y;
		xi = v0->x;

		/*dz_inv =  ((1.0f / v1->z) - (1.0f / v0->z)) / dx;
		zi = 1.0f / v0->z;*/
		dz_inv =  (v1->z - v0->z) / dx;
		zi = v0->z;

		for(int i = 0; i < dx; i++)
		{

			if(zi > this->zbuffer.zbuffer[(int)xi + (int)yi * this->zbuffer.width])
			{
				vb[(int)xi + (int)yi * lpitch] = color;
				this->zbuffer.zbuffer[(int)xi + (int)yi * this->zbuffer.width] = zi;
			}

			yi += dy;
			xi++;
			zi += dz_inv;
		}
	}
	else if(dy > dx)
	{
		if(v1->y < v0->y)
		{
			SWAP(v0, v1, vTmp);
		}

		dx = (v1->x - v0->x) / dy;
		yi = v0->y;
		xi = v0->x;

		/*dz_inv =  ((1.0f / v1->z) - (1.0f / v0->z)) / dy;
		zi = 1.0f / v0->z;*/
		dz_inv =  (v1->z - v0->z) / dy;
		zi = v0->z;

		for(int i = 0; i < dy; i++)
		{
			if(zi > this->zbuffer.zbuffer[(int)xi + (int)yi * this->zbuffer.width])
			{
				vb[(int)xi + (int)yi * lpitch] = color;
				this->zbuffer.zbuffer[(int)xi + (int)yi * this->zbuffer.width] = zi;
			}

			yi++;
			xi += dx;
			zi += dz_inv;
		}
	}

	return 1;
}

int Draw32BitStrategy::DrawHLine(float x0, float x1, int y, unsigned int color, void *videoMemory, int lpitch)
{
	_drawLine->DrawHLine(x0, x1, y, color, videoMemory, lpitch, _alphaBlender);

	return 1;
}

int Draw32BitStrategy::DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color, void *videoMemory, int lpitch){

	if(!pts) return 0;
	if(numPts < 0) return 0;

	//int iDestIndex = x + (y * (lpitch >> 2));
	int *vb = (int *)videoMemory;
	int alpha = (color >> 24) & 0x000000FF;
	lpitch >>= 2;

	if(alpha == 255)
	{
		for(int i = 0; i < numPts; i++)
		{
			if(pts[i].x > this->minClipX && pts[i].x < this->maxClipX && pts[i].y > this->minClipY && pts[i].y < this->maxClipY)
				vb[pts[i].x + pts[i].y * lpitch] = color;
		}
	}
	else
	{
		for(int i = 0; i < numPts; i++)
		{
			if(pts[i].x > this->minClipX && pts[i].x < this->maxClipX && pts[i].y > this->minClipY && pts[i].y < this->maxClipY)
				_alphaBlender->AlphaBlend(&vb[pts[i].x + pts[i].y * lpitch], &color, 1);
		}
	}

	return 1;
}

int Draw32BitStrategy::DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color, void *videoMemory, int lpitch)
{
	if(!pts) return 0;
	if(numPts < 0) return 0;

	int *vb = (int *)videoMemory;
	float alpha = static_cast<float>((color >> 24) & 0x000000FF);
	int x, y;
	lpitch >>= 2;

	__declspec(align(16)) unsigned int colorSour[4];
	__declspec(align(16)) unsigned int colorDest[4];

	__declspec(align(16)) unsigned int alphas[4];              //alpha[0] alpha[1]   alpha[0] = rfPart(x)*rfPart(y)  alpha[1] = fPart(x)*rfPart(y)
	float ft = 0;											   //alpha[2] alpha[3]   alpha[2] = rfPart(x)*fPart(y)   alpha[3] = fPart(x) * fPart(y)
	//__declspec(align(16)) unsigned int k[4];				   // fPart = x - (int)x    rfPart = 1.0 - fPart

	for(int i = 0; i < numPts; i++)
	{
		if(pts[i].x > this->minClipX && pts[i].x < this->maxClipX && pts[i].y > this->minClipY && pts[i].y < this->maxClipY)
		{
			alphas[0] = static_cast<uint32_t>(alpha * (float)( (1.0f - (pts[i].x - (int)pts[i].x)) * (1.0f - (pts[i].y - (int)pts[i].y)) ));
			alphas[1] = static_cast<uint32_t>(alpha * (float)( (pts[i].x - (int)pts[i].x) * (1.0f - (pts[i].y - (int)pts[i].y)) ));
			alphas[2] = static_cast<uint32_t>(alpha * (float)( (1.0f - (pts[i].x - (int)pts[i].x)) * (pts[i].y - (int)pts[i].y) ));
			alphas[3] = static_cast<uint32_t>(alpha * (float)( (pts[i].x - (int)pts[i].x) * (pts[i].y - (int)pts[i].y) ));

			colorSour[0] = (color & 0x00FFFFFF) | (alphas[0] << 24);
			colorSour[1] = (color & 0x00FFFFFF) | (alphas[1] << 24);
			colorSour[2] = (color & 0x00FFFFFF) | (alphas[2] << 24);
			colorSour[3] = (color & 0x00FFFFFF) | (alphas[3] << 24);

			x = (int)pts[i].x;
			y = (int)pts[i].y;

			colorDest[0] = vb[x + (y * lpitch)];
			colorDest[1] = vb[(x + 1) + (y * lpitch)];
			colorDest[2] = vb[x + ((y + 1) * lpitch)];
			colorDest[3] = vb[(x + 1) + ((y + 1) * lpitch)];

			_alphaBlender->AlphaBlend(colorDest, colorSour, 4);

			vb[x + (y * lpitch)] = colorDest[0];
			vb[(x + 1) + (y * lpitch)] = colorDest[1];
			vb[x + ((y + 1) * lpitch)] = colorDest[2];
			vb[(x + 1) + ((y + 1) * lpitch)] = colorDest[3];
		}
	}

	return 1;
}

void Draw32BitStrategy::DrawTopTri(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch)
{
	float dx_right, dx_left, xs, xe, height;
	float temp_x, /*temp_y,*/ right, left;
	int iy1, iy3, loop_y;
	//unsigned int *vb = NULL;
	//int lpitch2 = lpitch >> 2;
	/*if (y1 == y3 || y2 == y3)
		return;*/

	if(x2 < x1){
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	}

	height = y3 - y1;

	dx_left = (x3 - x1) / height;
	dx_right = (x3 - x2) / height;

	xs = x1;
	xe = x2;

	if (y1 < minClipY)
	{
   // compute new xs and ys
		xs = xs + dx_left * (-y1 + minClipY);
		xe = xe + dx_right * (-y1 + minClipY);

   // reset y1
		y1 = minClipY;

		iy1 = static_cast<int>(y1);
   } // end if top is off screen
	else
	{
		iy1 = static_cast<int>(ceil(y1));

		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
	}

   if (y3 > maxClipY)
   {
	  y3 = maxClipY;
	  iy3 = static_cast<int>(y3 - 1);
   }
   else
   {
	  iy3 = static_cast<int>(ceil(y3) - 1);
   }

	//vb = videoMemory + (int)y1 * lpitch2;

	/*//new Пропуск одной линии для предотвращения вывода на уже нарисованый треугольник. Если этого не сделать будет видна разделяющая линия между нижним и верхним треуголиником.
	xs += dx_left;
	xe += dx_right;
	y1++;
	//new*/

	if (x1 >= minClipX && x1 <= maxClipX && x2 >= minClipX && x2 <= maxClipX && x3 >= minClipX && x3 <= maxClipX)
    {
		//for(temp_y = y1; temp_y <= y3; temp_y++/*, vb += lpitch2*/)
		for(loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			//DrawHLine(xs, xe, temp_y, color, videoMemory, lpitch);
		
			//_drawLine->DrawHGradientLine(xs, xe, temp_y, color, fabs(dx_left), fabs(dx_right), videoMemory, lpitch, _alphaBlender);
			_drawLine->DrawHGradientLine2(fabs(dx_left), fabs(dx_right), xs, xe, loop_y, color, videoMemory, lpitch, _alphaBlender);
			//_drawLine->DrawHLine(xs, xe, temp_y, color, videoMemory, lpitch, _alphaBlender);

			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		//for(temp_y = y1; temp_y <= y3; temp_y++/*, vb += lpitch2*/)
		for(loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			//DrawHLine(xs, xe, temp_y, color, videoMemory, lpitch);
		
			//_drawLine->DrawHGradientLine(xs, xe, temp_y, color, fabs(dx_left), fabs(dx_right), videoMemory, lpitch, _alphaBlender);
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			//if(right < 0.0f || left > maxClipX) continue;

			if (left < minClipX)
			{
				left = minClipX;

				/*if (right < 0)
					continue;*/
			}

			if (right > maxClipX)
			{
				right = maxClipX;

				/*if (left > maxClipX)
					continue;*/
			}

			_drawLine->DrawHGradientLine2(fabs(dx_left), fabs(dx_right), left, right, loop_y, color, videoMemory, lpitch, _alphaBlender);
			//_drawLine->DrawHLine(left, right, temp_y, color, videoMemory, lpitch, _alphaBlender);
		}
	}
}

void Draw32BitStrategy::DrawBottomTri(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch)
{
	float dx_right, dx_left, xs, xe, height;
	float temp_x, /*temp_y,*/ right, left;
	int iy1, iy3, loop_y;
	//unsigned int *vb = NULL;
	//int lpitch2 = lpitch >> 2;
	/*if (y1 == y2 || y1 == y3)
		return;*/

	if(x3 < x2){
		temp_x = x2;
		x2 = x3;
		x3 = temp_x;
	}

	height = y3 - y1;

	dx_left = (x2 - x1) / height;
	dx_right = (x3 - x1) / height;

	xs = x1;
	xe = x1;

	if (y1 < minClipY)
	{
		// compute new xs and ys
		xs = xs + dx_left * (-y1 + minClipY);
		xe = xe + dx_right * (-y1 + minClipY);

		// reset y1
		y1 = minClipY;
		iy1 = static_cast<int>(y1);
	} // end if top is off screen
	else
	{
		iy1 = static_cast<int>(ceil(y1));

		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
	}

	if (y3 > maxClipY)
	{
		y3 = maxClipY;
		iy3 = static_cast<int>(y3 - 1);
	}
	else
	{
		iy3 = static_cast<int>(ceil(y3) - 1);
	}

	/*TCHAR mas[128];
	swprintf(mas, L"%d %d", iy1, iy3);
	MessageBox(0,mas,0,0);*/
	//vb = videoMemory + (int)y1 * lpitch2;

	if (x1 >= minClipX && x1 <= maxClipX && x2 >= minClipX && x2 <= maxClipX && x3 >= minClipX && x3 <= maxClipX)
	{
		//for(temp_y = y1; temp_y <= y3; temp_y++/*, vb += lpitch2*/)
		for(loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			//DrawHLine(xs, xe, temp_y, color, videoMemory, lpitch);
			//_drawLine->DrawHGradientLine(xs, xe, temp_y, color, fabs(dx_left), fabs(dx_right), videoMemory, lpitch, _alphaBlender);
			_drawLine->DrawHGradientLine2(fabs(dx_left), fabs(dx_right), xs, xe, loop_y, color, videoMemory, lpitch, _alphaBlender);
			//_drawLine->DrawHLine(xs, xe, temp_y, color, videoMemory, lpitch, _alphaBlender);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		//for(temp_y = y1; temp_y <= y3; temp_y++/*, vb += lpitch2*/)
		for(loop_y = iy1; loop_y <= iy3; loop_y++)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

			if (left < minClipX)
			{
				left = minClipX;

				/*if (right < 0)
					continue;*/
			}

			if (right > maxClipX)
			{
				right = maxClipX;

				/*if (left > maxClipX)
					continue;*/
			}

			_drawLine->DrawHGradientLine2(fabs(dx_left), fabs(dx_right), left, right, loop_y, color, videoMemory, lpitch, _alphaBlender);
			//_drawLine->DrawHLine(left, right, temp_y, color, videoMemory, lpitch, _alphaBlender);
		}
	}
}

int Draw32BitStrategy::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int *videoMemory, int lpitch)
{
	float temp_x, temp_y, new_x;
	
	//if((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3)) return 0;
	if((math3D::mathFunc.FCMP(x1, x2) && math3D::mathFunc.FCMP(x2, x3)) || (math3D::mathFunc.FCMP(y1, y2) && math3D::mathFunc.FCMP(y2, y3))) return 0;

	if(y2 < y1){
		temp_x = x2;
		temp_y = y2;
		x2 = x1;
		y2 = y1;
		x1 = temp_x;
		y1 = temp_y;
	}
	if(y3 < y1){
		temp_x = x3;
		temp_y = y3;
		x3 = x1;
		y3 = y1;
		x1 = temp_x;
		y1 = temp_y;
	}
	if(y3 < y2){
		temp_x = x3;
		temp_y = y3;
		x3 = x2;
		y3 = y2;
		x2 = temp_x;
		y2 = temp_y;
	}

	if (y3 < minClipY || y1 > maxClipY ||
    (x1 < minClipX && x2 < minClipX && x3 < minClipX) ||
    (x1 > maxClipX && x2 > maxClipX && x3 > maxClipX))
		return 0;

	//if(y1 == y2){
	if(math3D::mathFunc.FCMP(y1, y2)){
		DrawTopTri(x1,y1,x2,y2,x3,y3,color,videoMemory,lpitch);
	}
	//else if(y3 == y2){
	else if(math3D::mathFunc.FCMP(y3, y2)){
		DrawBottomTri(x1,y1,x2,y2,x3,y3,color,videoMemory,lpitch);
	}
	else{
		//new_x = x1 + (int)(0.5f + (float)(y2 - y1) * (float)(x3 - x1) / (float)(y3 - y1));
		new_x = x1 + (y2 - y1) * (x3 - x1) / (y3 - y1);

		DrawBottomTri(x1,y1,new_x,y2,x2,y2,color,videoMemory,lpitch);
		//y2++;
		DrawTopTri(x2,y2,new_x,y2,x3,y3,color,videoMemory,lpitch);
	}

	return 1;
}

int Draw32BitStrategy::setClipBorders(float minClipX, float minClipY, float maxClipX, float maxClipY)
{
	if(maxClipX <= 0 || maxClipY <= 0)
	{
		maxClipX = maxClipY = 0;
		return 0;
	}
	
	if(minClipX <= 0 || minClipY <= 0 || minClipX >= maxClipX || minClipY >= maxClipY)
	{
		minClipX = minClipY = 0;
	}

	this->maxClipX = maxClipX - 1;
	this->maxClipY = maxClipY - 1;
	this->minClipX = minClipX;
	this->minClipY = minClipY;

	clipRt.left = static_cast<LONG>(this->minClipX);
	clipRt.top = static_cast<LONG>(this->minClipY);

	clipRt.right = static_cast<LONG>(this->maxClipX);
	clipRt.bottom = static_cast<LONG>(this->maxClipY);

	return 1;
}

int Draw32BitStrategy::DrawGouraudTriangle(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	if(!face) return 0;

	const int TRI_TYPE_NONE = 0;
	const int TRI_TYPE_FLAT_BOTTOM = 1;
	const int TRI_TYPE_FLAT_TOP = 2;
	const int TRI_TYPE_FLAT_MASK = 3;
	const int TRI_TYPE_GENERAL = 4;

	const int INTERP_LHS = 0;
	const int INTERP_RHS = 1;

	int v0 = 0, v1 = 1, v2 = 2, /*индексы вершин. ипользуються для того чтобы не менять местами вершины в массиве face->tvlist, а менять только индексы вершин*/
		temp = 0, /*переменная для обмена индексов вершин*/
		tri_type = TRI_TYPE_NONE, /*тип треугольника*/
		irestart = INTERP_LHS;
	float dx, dy, dyl, dyr;
	float tempf = 0;
	/*int u, v, w;*/
	float du, dv, dw;
	int xi, yi;
	float ui, vi, wi;
	/*int index_x, index_y*/;
	/*int x, y;*/
	int xstart, xend, ystart, yrestart, yend;
	float dxdyl, xr, xl, dxdyr, dudyl, ul, dvdyl, vl, dwdyl, wl, dudyr, ur, dvdyr, vr, dwdyr, wr;
	float x0, y0, tu0, tv0, tw0;
	float x1, y1, tu1, tv1, tw1;
	float x2, y2, tu2, tv2, tw2;
	
	float r_base0, g_base0, b_base0;
	float r_base1, g_base1, b_base1;
	float r_base2, g_base2, b_base2;

	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	/*сортируем индексы вершин в порядке v0.y <= v1.y <= v2.y, т.е. сверху вниз. положительная ось x направлена вправо, y - вниз*/
	if(face->tvlist[v1].y < face->tvlist[v0].y)
		SWAP(v0, v1, temp);

	if(face->tvlist[v2].y < face->tvlist[v0].y)
		SWAP(v0, v2, temp);

	if(face->tvlist[v2].y < face->tvlist[v1].y)
		SWAP(v1, v2, temp);

	if(face->tvlist[v0].y == face->tvlist[v1].y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		//Сортируем вершины слева направо, т.е. v0 будет слева
		if(face->tvlist[v1].x < face->tvlist[v0].x)
			SWAP(v1, v0, temp);
	}
	else if(face->tvlist[v1].y == face->tvlist[v2].y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		//Сортируем вершины слева направо, т.е. v1 будет слева
		if(face->tvlist[v2].x < face->tvlist[v1].x)
			SWAP(v2, v1, temp);
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	//Используеться 32 битный цвет

	RGBFROM32BIT(face->lit_color[v0], r_base0, g_base0, b_base0);
	RGBFROM32BIT(face->lit_color[v1], r_base1, g_base1, b_base1);
	RGBFROM32BIT(face->lit_color[v2], r_base2, g_base2, b_base2);

	x0 = static_cast<float>((int)(face->tvlist[v0].x + 0.5f));
	y0 = static_cast<float>((int)(face->tvlist[v0].y + 0.5f));

	tu0 = r_base0; tv0 = g_base0; tw0 = b_base0;

	x1 = static_cast<float>((int)(face->tvlist[v1].x + 0.5f));
	y1 = static_cast<float>((int)(face->tvlist[v1].y + 0.5f));

	tu1 = r_base1; tv1 = g_base1; tw1 = b_base1;

	x2 = static_cast<float>((int)(face->tvlist[v2].x + 0.5f));
	y2 = static_cast<float>((int)(face->tvlist[v2].y + 0.5f));

	tu2 = r_base2; tv2 = g_base2; tw2 = b_base2;

	yrestart = static_cast<int>(y1);

	if(tri_type & TRI_TYPE_FLAT_MASK)
	{
		if(tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = 1.0f / (y2 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x2 - x0) * dy;
			dudyl = (tu2 - tu0) * dy;
			dvdyl = (tv2 - tv0) * dy;
			dwdyl = (tw2 - tw0) * dy;

			dxdyr = (x2 - x1) * dy;
			dudyr = (tu2 - tu1) * dy;
			dvdyr = (tv2 - tv1) * dy;
			dwdyr = (tw2 - tw1) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				ul = dudyl * dy + tu0;
				vl = dvdyl * dy + tv0;
				wl = dwdyl * dy + tw0;

				xr = dxdyr * dy + x1;
				ur = dudyr * dy + tu1;
				vr = dvdyr * dy + tv1;
				wr = dwdyr * dy + tw1;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				ul = tu0;
				vl = tv0;
				wl = tw0;

				xr = x1;
				ur = tu1;
				vr = tv1;
				wr = tw1;

				ystart = static_cast<int>(y0);
			}
		}//if(tri_type == TRI_TYPE_FLAT_TOP)
		else //tri_type == TRI_TYPE_FLAT_BOTTOM
		{
			dy = 1.0f / (y1 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x1 - x0) * dy;
			dudyl = (tu1 - tu0) * dy;
			dvdyl = (tv1 - tv0) * dy;
			dwdyl = (tw1 - tw0) * dy;

			dxdyr = (x2 - x0) * dy;
			dudyr = (tu2 - tu0) * dy;
			dvdyr = (tv2 - tv0) * dy;
			dwdyr = (tw2 - tw0) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				ul = dudyl * dy + tu0;
				vl = dvdyl * dy + tv0;
				wl = dwdyl * dy + tw0;

				xr = dxdyr * dy + x0;
				ur = dudyr * dy + tu0;
				vr = dvdyr * dy + tv0;
				wr = dwdyr * dy + tw0;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				ul = tu0;
				vl = tv0;
				wl = tw0;

				xr = x0;
				ur = tu0;
				vr = tv0;
				wr = tw0;

				ystart = static_cast<int>(y0);
			}
		}//else //tri_type == TRI_TYPE_FLAT_BOTTOM

		if((yend = static_cast<int>(y2)) > this->maxClipY)
		{
			yend = static_cast<int>(this->maxClipY);
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;//(ul + 0.5f);
				vi = vl;//(vl + 0.5f);
				wi = wl;//(wl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = ur - ul;
					dv = vr - vl;
					dw = wr - wl;
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					ui += dx * du;
					vi += dx * dv;
					wi += dx * dw;

					xstart = static_cast<int>(this->minClipX);
				}

				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}//if с отсечением по x
		else
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;//(ul + 0.5f);
				vi = vl;//(vl + 0.5f);
				wi = wl;//(wl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = ur - ul;
					dv = vr - vl;
					dw = wr - wl;
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}
	}//if(tri_type & TRI_TYPE_FLAT_MASK)
	else if(tri_type == TRI_TYPE_GENERAL)
	{
		if((yend = static_cast<int>(y2)) > this->maxClipY)
			yend = static_cast<int>(this->maxClipY);

		if(y1 < this->minClipY)
		{
			
			dyl = 1.0f / (y2 - y1);
			dxdyl = (x2 - x1) * dyl;
			dudyl = (tu2 - tu1) * dyl;
			dvdyl = (tv2 - tv1) * dyl;
			dwdyl = (tw2 - tw1) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			dyr = (this->minClipY - y0);
			dyl = (this->minClipY - y1);

			xl = dxdyl * dyl + x1;
			ul = dudyl * dyl + tu1;
			vl = dvdyl * dyl + tv1;
			wl = dwdyl * dyl + tw1;

			xr = dxdyr * dyr + x0;
			ur = dudyr * dyr + tu0;
			vr = dvdyr * dyr + tv0;
			wr = dwdyr * dyr + tw0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}
		else if(y0 < this->minClipY)
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dudyl = (tu1 - tu0) * dyl;
			dvdyl = (tv1 - tv0) * dyl;
			dwdyl = (tw1 - tw0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			dy = (this->minClipY - y0);

			xl = dxdyl * dy + x0;
			ul = dudyl * dy + tu0;
			vl = dvdyl * dy + tv0;
			wl = dwdyl * dy + tw0;

			xr = dxdyr * dy + x0;
			ur = dudyr * dy + tu0;
			vr = dvdyr * dy + tv0;
			wr = dwdyr * dy + tw0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dudyl = (tu1 - tu0) * dyl;
			dvdyl = (tv1 - tv0) * dyl;
			dwdyl = (tw1 - tw0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			xl = xr = x0;
			ul = ur = tu0;
			vl = vr = tv0;
			wl = wr = tw0;

			ystart = static_cast<int>(y0);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;
				vi = vl;
				wi = wl;

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					ui += du * dx;
					vi += dv * dx;
					wi += dw * dx;

					xstart = static_cast<int>(this->minClipX);
				}
				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dudyl = (tu2 - tu1) * dyl;
						dvdyl = (tv2 - tv1) * dyl;
						dwdyl = (tw2 - tw1) * dyl;

						xl = x1;
						ul = tu1;
						vl = tv1;
						wl = tw1;

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dudyr = (tu1 - tu2) * dyr;
						dvdyr = (tv1 - tv2) * dyr;
						dwdyr = (tw1 - tw2) * dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;
						wr = tw2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;
				vi = vl;
				wi = wl;

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dudyl = (tu2 - tu1) * dyl;
						dvdyl = (tv2 - tv1) * dyl;
						dwdyl = (tw2 - tw1) * dyl;

						xl = x1;
						ul = tu1;
						vl = tv1;
						wl = tw1;

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dudyr = (tu1 - tu2) * dyr;
						dvdyr = (tv1 - tv2) * dyr;
						dwdyr = (tw1 - tw2) * dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;
						wr = tw2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
			}
		}
	}



	return 1;
}

int Draw32BitStrategy::DrawGouraudTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	triInterp.setTriangle(face);

	int dyist = 0;
	int dyiend = static_cast<int>((triInterp.vlist[2].y) - (triInterp.vlist[0].y));
	int dxist = 0;
	int dxiend = 0;
	float xend, xst;
	float z;

	if(triInterp.vlist[0].y < this->minClipY)
	{
		dyist = static_cast<int>(this->minClipY - (triInterp.vlist[0].y));
	}
	if(triInterp.vlist[2].y > this->maxClipY)
	{
		dyiend -= static_cast<int>((triInterp.vlist[2].y) - this->maxClipY);
	}

	if((face->tvlist[0].x < this->minClipX) || (face->tvlist[0].x > this->maxClipX) || (face->tvlist[1].x < this->minClipX) || (face->tvlist[1].x > this->maxClipX) || (face->tvlist[2].x < this->minClipX) || (face->tvlist[2].x > this->maxClipX))
	{
		for(int dyi = dyist; dyi <= dyiend; dyi++)
		{
			dxist = 0;
			triInterp.setIndexY(dyi);

			/*dxiend = (triInterp.getDX() + 0.5);

			xst = triInterp.getXL() + 0.5;
			xend = triInterp.getXR() + 0.5;*/

			dxiend = triInterp.getDX();

			xst = static_cast<float>(triInterp.getXL());
			xend = static_cast<float>(triInterp.getXR());
			//dxiend = xend - xst;

			if(xst > this->maxClipX)
			{
				dxist = static_cast<int>(xst - this->maxClipX);
			}
			else if(xst < this->minClipX)
			{
				dxist = static_cast<int>(this->minClipX - xst);
			}

			if(xend > this->maxClipX)
			{
				dxiend -= static_cast<int>(xend - this->maxClipX);
			}
			else if(xend < this->minClipX)
			{
				dxiend -= static_cast<int>(this->minClipX - xend);
			}
			for(int dxi = dxist; dxi <= dxiend; dxi++)
			{
				int x, y, color;

				triInterp.setIndexX(dxi);

				triInterp.getCoords(&x, &y, &z);

				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[x + y * this->zbuffer.width])
				{
				triInterp.getColor(&color);

				dest_buffer[x + y * lpitch] = color;//ARGB32BIT(255,255,0,0);//
				//color = ARGB32BIT(127,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[x + y * lpitch], &color, 1);
				this->zbuffer.zbuffer[x + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}
	else
	{
		for(int dyi = dyist; dyi <= dyiend; dyi++)
		{
			dxist = 0;
			triInterp.setIndexY(dyi);

			dxiend = (triInterp.getDX());
			for(int dxi = /*dxist*/0; dxi <= dxiend; dxi++)
			{
				int x, y, color;

				triInterp.setIndexX(dxi);

				triInterp.getCoords(&x, &y, &z);
				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[x + y * this->zbuffer.width])
				{
				triInterp.getColor(&color);

				dest_buffer[x + y * lpitch] = color;//ARGB32BIT(255,255,0,0);//
				//color = ARGB32BIT(127,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[x + y * lpitch], &color, 1);
				this->zbuffer.zbuffer[x + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}

	return 1;
}

int Draw32BitStrategy::DrawGouraudTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	//ZeroMemory(&triInterpI, sizeof(triInterpIter));

	/*face->tvlist[0].z = (face->tvlist[0].z - 10.0f) / (100000.0f - 10.0f);
	face->tvlist[1].z = (face->tvlist[1].z - 10.0f) / (100000.0f - 10.0f);
	face->tvlist[2].z = (face->tvlist[2].z - 10.0f) / (100000.0f - 10.0f);*/

	//float zNear = 10.0f;
	//float zFar = 100000.0f;

	//face->tvlist[0].z = /*((*/((zFar + zNear) / (zFar - zNear)) + ((-2.0f * zFar * zNear) / (face->tvlist[0].z * zFar - zNear));//) + 1.0f) / 2.0f;
	//face->tvlist[1].z = /*((*/((zFar + zNear) / (zFar - zNear)) + ((-2.0f * zFar * zNear) / (face->tvlist[1].z * zFar - zNear));//) + 1.0f) / 2.0f;
	//face->tvlist[2].z = /*((*/((zFar + zNear) / (zFar - zNear)) + ((-2.0f * zFar * zNear) / (face->tvlist[2].z * zFar - zNear));//) + 1.0f) / 2.0f;

	triInterpI.setTriangle(face);

	//if(triInterpI.triType == TriType2::GeneralRHS) return 0;

	int dxst;
	int dxend;
	int len;
	int y;
	int dyist = 0;
	int dyiend = static_cast<int>(triInterpI.height);
	int dxist = 0;
	int dxiend = 0;
	int color = 0;//ARGB32BIT(127,255,255,255);
	float z = 0;

	if(triInterpI.yc0 < this->minClipY)
	{
		dyist = static_cast<int>(this->minClipY - (triInterpI.yc0));
	}
	if(triInterpI.yc2 > this->maxClipY)
	{
		dyiend -= static_cast<int>((triInterpI.yc2) - this->maxClipY);
	}

	if((face->tvlist[0].x < this->minClipX) || (face->tvlist[0].x > this->maxClipX) || (face->tvlist[1].x < this->minClipX) || (face->tvlist[1].x > this->maxClipX) || (face->tvlist[2].x < this->minClipX) || (face->tvlist[2].x > this->maxClipX))
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			//len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			dxist = 0;
			dxiend = dxend - dxst;

			if(dxst > this->maxClipX)
			{
				dxist = static_cast<int>(dxst - this->maxClipX);
			}
			else if(dxst < this->minClipX)
			{
				dxist = static_cast<int>(this->minClipX - dxst);
			}

			if(dxend > this->maxClipX)
			{
				dxiend -= static_cast<int>(dxend - this->maxClipX);
			}
			else if(dxend < this->minClipX)
			{
				dxiend -= static_cast<int>(this->minClipX - dxend);
			}

			/*TCHAR mas[128];
			swprintf(mas, L"%d || %d || %f >> %f",y_idx, dxiend, triInterpI.xs, triInterpI.xe);
			MessageBox(0, mas, 0, 0);*/

			for(int x_idx = dxist; x_idx <= dxiend; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				//if((z /*+ this->zbuffer.z_add*/) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				//{
					triInterpI.getColor(&color);
					//dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z /*+ this->zbuffer.z_add*/);
				//}
			}
		}
	}
	else
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			for(int x_idx = 0; x_idx <= len; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				//if((z /*+ this->zbuffer.z_add*/) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				//{
					triInterpI.getColor(&color);
					//dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					
					/*if(len >= 0)
					{*/
						/*TCHAR mas[128];
						swprintf(mas, L"%d -> %d  H = %d type = %d restarted = %d yidx = %d", dxst, dxend, dyiend, (int)triInterpI.triType, (int)triInterpI.isRestarted, y_idx);
						MessageBox(0,mas,0,0);*/
					//}

					this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z /*+ this->zbuffer.z_add*/);
				//}
			}
		}
	}

	return 1;
}

int Draw32BitStrategy::DrawTriangle2(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	//ZeroMemory(&triInterpI, sizeof(triInterpIter));

	triInterpI.setTriangle(face);

	//if(triInterpI.triType != TriType2::GeneralLHS) return 0;

	int dxst;
	int dxend;
	int len;
	int y;
	int dyist = 0;
	int dyiend = static_cast<int>(triInterpI.height);
	int dxist = 0;
	int dxiend = 0;
	int color = face->lit_color[0];
	float z = 0;

	if(triInterpI.vlist[0].y < this->minClipY)
	{
		dyist = static_cast<int>(this->minClipY - (triInterpI.vlist[0].y));
	}
	if(triInterpI.vlist[2].y > this->maxClipY)
	{
		dyiend -= static_cast<int>((triInterpI.vlist[2].y) - this->maxClipY);
	}

	if((face->tvlist[0].x < this->minClipX) || (face->tvlist[0].x > this->maxClipX) || (face->tvlist[1].x < this->minClipX) || (face->tvlist[1].x > this->maxClipX) || (face->tvlist[2].x < this->minClipX) || (face->tvlist[2].x > this->maxClipX))
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			//len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			dxist = 0;
			dxiend = dxend - dxst;

			if(dxst > this->maxClipX)
			{
				dxist = static_cast<int>(dxst - this->maxClipX);
			}
			else if(dxst < this->minClipX)
			{
				dxist = static_cast<int>(this->minClipX - dxst);
			}

			if(dxend > this->maxClipX)
			{
				dxiend -= static_cast<int>(dxend - this->maxClipX);
			}
			else if(dxend < this->minClipX)
			{
				dxiend -= static_cast<int>(this->minClipX - dxend);
			}

			for(int x_idx = dxist; x_idx <= dxiend; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				{
					dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}
	else
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			for(int x_idx = 0; x_idx <= len; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				{
					dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}

	return 1;
}

int Draw32BitStrategy::DrawTriangleOriginal(struct3D::POLYF4D_PTR face, unsigned int* videoMemory, int lpitch)
{
	return DrawTriangle(
		face->tvlist[0].x, face->tvlist[0].y,
		face->tvlist[1].x, face->tvlist[1].y,
		face->tvlist[2].x, face->tvlist[2].y,
		ARGB32BIT(127, 0, 0, 0),
		videoMemory, lpitch);
}

int Draw32BitStrategy::DrawTriangle3(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	//ZeroMemory(&triInterpI, sizeof(triInterpIter));

	triInterpI.setTriangle(face);

	//if(triInterpI.triType != TriType2::GeneralLHS) return 0;

	int dxst;
	int dxend;
	int len;
	int y;
	int dyist = 0;
	int dyiend = static_cast<int>(triInterpI.height);
	int dxist = 0;
	int dxiend = 0;
	int color = 0;//ARGB32BIT(127,255,255,255);
	float z = 0;

	if(triInterpI.vlist[0].y < this->minClipY)
	{
		dyist = static_cast<int>(this->minClipY - (triInterpI.vlist[0].y));
	}
	if(triInterpI.vlist[2].y > this->maxClipY)
	{
		dyiend -= static_cast<int>((triInterpI.vlist[2].y) - this->maxClipY);
	}

	if((face->tvlist[0].x < this->minClipX) || (face->tvlist[0].x > this->maxClipX) || (face->tvlist[1].x < this->minClipX) || (face->tvlist[1].x > this->maxClipX) || (face->tvlist[2].x < this->minClipX) || (face->tvlist[2].x > this->maxClipX))
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			//len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			dxist = 0;
			dxiend = dxend - dxst;

			if(dxst > this->maxClipX)
			{
				dxist = static_cast<int>(dxst - this->maxClipX);
			}
			else if(dxst < this->minClipX)
			{
				dxist = static_cast<int>(this->minClipX - dxst);
			}

			if(dxend > this->maxClipX)
			{
				dxiend -= static_cast<int>(dxend - this->maxClipX);
			}
			else if(dxend < this->minClipX)
			{
				dxiend -= static_cast<int>(this->minClipX - dxend);
			}

			for(int x_idx = dxist; x_idx <= dxiend; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				{
					triInterpI.getColor(&color);
					dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}
	else
	{
		for(int y_idx = dyist; y_idx <= dyiend; y_idx++)
		{
			triInterpI.setIndexY(y_idx);

			dxst = triInterpI.getXL();
			dxend = triInterpI.getXR();
			len = dxend - dxst;
			triInterpI.getCoords(0, &y, 0);

			for(int x_idx = 0; x_idx <= len; x_idx++)
			{
				//dest_buffer[(x_idx + dxst) + y * lpitch] = ARGB32BIT(255,255,0,0);
				//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
				triInterpI.setIndexX(x_idx);
				triInterpI.getZInv(&z);

				if((z + this->zbuffer.z_add) > this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width])
				{
					triInterpI.getColor(&color);
					dest_buffer[(x_idx + dxst) + y * lpitch] = color;
					//this->_alphaBlender->AlphaBlend(&dest_buffer[(x_idx + dxst) + y * lpitch], &color, 1);
					this->zbuffer.zbuffer[(x_idx + dxst) + y * this->zbuffer.width] = (z + this->zbuffer.z_add);
				}
			}
		}
	}

	return 1;
}


int Draw32BitStrategy::DrawGouraudTriangle(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int V0color, unsigned int V1color, unsigned int V2color, unsigned int *videoMemory, int lpitch)
{
	const int TRI_TYPE_NONE = 0;
	const int TRI_TYPE_FLAT_BOTTOM = 1;
	const int TRI_TYPE_FLAT_TOP = 2;
	const int TRI_TYPE_FLAT_MASK = 3;
	const int TRI_TYPE_GENERAL = 4;

	const int INTERP_LHS = 0;
	const int INTERP_RHS = 1;

	int //v0 = 0, v1 = 1, v2 = 2, /*индексы вершин. ипользуються для того чтобы не менять местами вершины в массиве face->tvlist, а менять только индексы вершин*/
		//temp = 0, /*переменная для обмена индексов вершин*/
		tri_type = TRI_TYPE_NONE, /*тип треугольника*/
		irestart = INTERP_LHS;
	float dx, dy, dyl, dyr;
	float tempf = 0;
	unsigned int tempColor;
	/*int u, v, w;*/
	float du, dv, dw;
	int xi, yi;
	float ui, vi, wi;
	/*int index_x, index_y;*/
	/*int x, y;*/
	int xstart, xend, ystart, yrestart, yend;
	float dxdyl, xr, xl, dxdyr, dudyl, ul, dvdyl, vl, dwdyl, wl, dudyr, ur, dvdyr, vr, dwdyr, wr;
	float /*x0, y0, */tu0, tv0, tw0;
	float /*x1, y1, */tu1, tv1, tw1;
	float /*x2, y2, */tu2, tv2, tw2;
	
	float r_base0, g_base0, b_base0;
	float r_base1, g_base1, b_base1;
	float r_base2, g_base2, b_base2;

	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((y0 < this->minClipY) && (y1 < this->minClipY) && (y2 < this->minClipY)) ||
		(y0 > this->maxClipY) && (y1 > this->maxClipY) && (y2 > this->maxClipY) ||
		(x0 < this->minClipX) && (x1 < this->minClipX) && (x2 < this->minClipX) ||
		(x0 > this->maxClipX) && (x1 > this->maxClipX) && (x2 > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((x0 == x1) && (x1 == x2)) ||
		((y0 == y1) && (y1 == y2)))
		return 0;

	/*сортируем индексы вершин в порядке v0.y <= v1.y <= v2.y, т.е. сверху вниз. положительная ось x направлена вправо, y - вниз*/
	if(y1 < y0)
	{
		SWAP(y0, y1, tempf);
		SWAP(x0, x1, tempf);
		SWAP(V0color, V1color, tempColor);
	}

	if(y2 < y0)
	{
		SWAP(y0, y2, tempf);
		SWAP(x0, x2, tempf);
		SWAP(V0color, V2color, tempColor);
	}

	if(y2 < y1)
	{
		SWAP(y1, y2, tempf);
		SWAP(x1, x2, tempf);
		SWAP(V1color, V2color, tempColor);
	}

	if(y0 == y1)
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		//Сортируем вершины слева направо, т.е. v0 будет слева
		if(x1 < x0)
		{
			//SWAP(y1, y0, tempf);
			//SWAP(V1color, V0color, tempColor);
			SWAP(x1, x0, tempf);
		}
	}
	else if(y1 == y2)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		//Сортируем вершины слева направо, т.е. v1 будет слева
		if(x2 < x1)
		{
			//SWAP(y2, y1, tempf);
			//SWAP(V2color, V1color, tempColor);
			SWAP(x2, x1, tempf);
		}
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	//Используеться 32 битный цвет

	RGBFROM32BIT(V0color, r_base0, g_base0, b_base0);
	RGBFROM32BIT(V1color, r_base1, g_base1, b_base1);
	RGBFROM32BIT(V2color, r_base2, g_base2, b_base2);

	x0 = static_cast<float>((int)(x0 + 0.5f));
	y0 = static_cast<float>((int)(y0 + 0.5f));

	tu0 = r_base0; tv0 = g_base0; tw0 = b_base0;

	x1 = static_cast<float>((int)(x1 + 0.5f));
	y1 = static_cast<float>((int)(y1 + 0.5f));

	tu1 = r_base1; tv1 = g_base1; tw1 = b_base1;

	x2 = static_cast<float>((int)(x2 + 0.5f));
	y2 = static_cast<float>((int)(y2 + 0.5f));

	tu2 = r_base2; tv2 = g_base2; tw2 = b_base2;

	yrestart = static_cast<int>(y1);

	if(tri_type & TRI_TYPE_FLAT_MASK)
	{
		if(tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = 1.0f / (y2 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x2 - x0) * dy;
			dudyl = (tu2 - tu0) * dy;
			dvdyl = (tv2 - tv0) * dy;
			dwdyl = (tw2 - tw0) * dy;

			dxdyr = (x2 - x1) * dy;
			dudyr = (tu2 - tu1) * dy;
			dvdyr = (tv2 - tv1) * dy;
			dwdyr = (tw2 - tw1) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				ul = dudyl * dy + tu0;
				vl = dvdyl * dy + tv0;
				wl = dwdyl * dy + tw0;

				xr = dxdyr * dy + x1;
				ur = dudyr * dy + tu1;
				vr = dvdyr * dy + tv1;
				wr = dwdyr * dy + tw1;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				ul = tu0;
				vl = tv0;
				wl = tw0;

				xr = x1;
				ur = tu1;
				vr = tv1;
				wr = tw1;

				ystart = static_cast<int>(y0);
			}
		}//if(tri_type == TRI_TYPE_FLAT_TOP)
		else //tri_type == TRI_TYPE_FLAT_BOTTOM
		{
			dy = 1.0f / (y1 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x1 - x0) * dy;
			dudyl = (tu1 - tu0) * dy;
			dvdyl = (tv1 - tv0) * dy;
			dwdyl = (tw1 - tw0) * dy;

			dxdyr = (x2 - x0) * dy;
			dudyr = (tu2 - tu0) * dy;
			dvdyr = (tv2 - tv0) * dy;
			dwdyr = (tw2 - tw0) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				ul = dudyl * dy + tu0;
				vl = dvdyl * dy + tv0;
				wl = dwdyl * dy + tw0;

				xr = dxdyr * dy + x0;
				ur = dudyr * dy + tu0;
				vr = dvdyr * dy + tv0;
				wr = dwdyr * dy + tw0;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				ul = tu0;
				vl = tv0;
				wl = tw0;

				xr = x0;
				ur = tu0;
				vr = tv0;
				wr = tw0;

				ystart = static_cast<int>(y0);
			}
		}//else //tri_type == TRI_TYPE_FLAT_BOTTOM

		if((yend = static_cast<int>(y2)) > this->maxClipY)
		{
			yend = static_cast<int>(this->maxClipY);
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;//(ul + 0.5f);
				vi = vl;//(vl + 0.5f);
				wi = wl;//(wl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = ur - ul;
					dv = vr - vl;
					dw = wr - wl;
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					ui += dx * du;
					vi += dx * dv;
					wi += dx * dw;

					xstart = static_cast<int>(this->minClipX);
				}

				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}//if с отсечением по x
		else
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;//(ul + 0.5f);
				vi = vl;//(vl + 0.5f);
				wi = wl;//(wl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = ur - ul;
					dv = vr - vl;
					dw = wr - wl;
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}
	}//if(tri_type & TRI_TYPE_FLAT_MASK)
	else if(tri_type == TRI_TYPE_GENERAL)
	{
		if((yend = static_cast<int>(y2)) > this->maxClipY)
			yend = static_cast<int>(this->maxClipY);

		if(y1 < this->minClipY)
		{
			
			dyl = 1.0f / (y2 - y1);
			dxdyl = (x2 - x1) * dyl;
			dudyl = (tu2 - tu1) * dyl;
			dvdyl = (tv2 - tv1) * dyl;
			dwdyl = (tw2 - tw1) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			dyr = (this->minClipY - y0);
			dyl = (this->minClipY - y1);

			xl = dxdyl * dyl + x1;
			ul = dudyl * dyl + tu1;
			vl = dvdyl * dyl + tv1;
			wl = dwdyl * dyl + tw1;

			xr = dxdyr * dyr + x0;
			ur = dudyr * dyr + tu0;
			vr = dvdyr * dyr + tv0;
			wr = dwdyr * dyr + tw0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}
		else if(y0 < this->minClipY)
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dudyl = (tu1 - tu0) * dyl;
			dvdyl = (tv1 - tv0) * dyl;
			dwdyl = (tw1 - tw0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			dy = (this->minClipY - y0);

			xl = dxdyl * dy + x0;
			ul = dudyl * dy + tu0;
			vl = dvdyl * dy + tv0;
			wl = dwdyl * dy + tw0;

			xr = dxdyr * dy + x0;
			ur = dudyr * dy + tu0;
			vr = dvdyr * dy + tv0;
			wr = dwdyr * dy + tw0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dudyl = (tu1 - tu0) * dyl;
			dvdyl = (tv1 - tv0) * dyl;
			dwdyl = (tw1 - tw0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dudyr = (tu2 - tu0) * dyr;
			dvdyr = (tv2 - tv0) * dyr;
			dwdyr = (tw2 - tw0) * dyr;

			xl = xr = x0;
			ul = ur = tu0;
			vl = vr = tv0;
			wl = wr = tw0;

			ystart = static_cast<int>(y0);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dudyl,dudyr,tempf);
				SWAP(dvdyl,dvdyr,tempf);
				SWAP(dwdyl,dwdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(ul,ur,tempf);
				SWAP(vl,vr,tempf);
				SWAP(wl,wr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);
				SWAP(tw1,tw2,tempf);

				irestart = INTERP_RHS;
			}
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;
				vi = vl;
				wi = wl;

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					ui += du * dx;
					vi += dv * dx;
					wi += dw * dx;

					xstart = static_cast<int>(this->minClipX);
				}
				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dudyl = (tu2 - tu1) * dyl;
						dvdyl = (tv2 - tv1) * dyl;
						dwdyl = (tw2 - tw1) * dyl;

						xl = x1;
						ul = tu1;
						vl = tv1;
						wl = tw1;

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dudyr = (tu1 - tu2) * dyr;
						dvdyr = (tv1 - tv2) * dyr;
						dwdyr = (tw1 - tw2) * dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;
						wr = tw2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				ui = ul;
				vi = vl;
				wi = wl;

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					du = (ur - ul) * dx;
					dv = (vr - vl) * dx;
					dw = (wr - wl) * dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					screen_ptr[xi] = ARGB32BIT(255, (int)ui, (int)vi, (int)wi);

					ui += du;
					vi += dv;
					wi += dw;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;

				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dudyl = (tu2 - tu1) * dyl;
						dvdyl = (tv2 - tv1) * dyl;
						dwdyl = (tw2 - tw1) * dyl;

						xl = x1;
						ul = tu1;
						vl = tv1;
						wl = tw1;

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dudyr = (tu1 - tu2) * dyr;
						dvdyr = (tv1 - tv2) * dyr;
						dwdyr = (tw1 - tw2) * dyr;

						xr = x2;
						ur = tu2;
						vr = tv2;
						wr = tw2;

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
			}
		}
	}



	return 1;
}


int Draw32BitStrategy::DrawPhongTriangle(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	if(!face) return 0;

	/*
	!!!! НУЖНО ПРОВЕРИТЬ ВЫВОД ВЕРХНЕГО И НИЖНЕГО ТРЕУГОЛЬНИКА !!!!
	*/

	const int TRI_TYPE_NONE = 0;
	const int TRI_TYPE_FLAT_BOTTOM = 1;
	const int TRI_TYPE_FLAT_TOP = 2;
	const int TRI_TYPE_FLAT_MASK = 3;
	const int TRI_TYPE_GENERAL = 4;

	const int INTERP_LHS = 0;
	const int INTERP_RHS = 1;

	int v0 = 0, v1 = 1, v2 = 2, /*индексы вершин. ипользуються для того чтобы не менять местами вершины в массиве face->tvlist, а менять только индексы вершин*/
		temp = 0, /*переменная для обмена индексов вершин*/
		tri_type = TRI_TYPE_NONE, /*тип треугольника*/
		irestart = INTERP_LHS;
	float dx, dy, dyl, dyr;
	float tempf = 0;
	//int u, v, w;
	float dnx, dny, dnz;
	int xi, yi;
	float nxi, nyi, nzi;
	/*int index_x, index_y;*/
	/*int x, y;*/
	int xstart, xend, ystart, yrestart, yend;
	float dxdyl, xr, xl, dxdyr, dnxdyl, nxl, dnydyl, nyl, dnzdyl, nzl, dnxdyr, nxr, dnydyr, nyr, dnzdyr, nzr;
	float dzdyl, zr, zl, dzdyr, zi, dz;//Интерполяция Z для вычисление вектора от пикселя к точечному источнику света.
	/*float dxwdyl, xwr, xwl, dxwdyr, xwi, dxw;
	float dywdyl, ywr, ywl, dywdyr, ywi, dyw;
	float dzwdyl, zwr, zwl, dzwdyr, zwi, dzw;*/
	VECTOR4D wdyl, wdyr, wr, wl, wi, dw; //Интерполяция мировых координат
	float x0, y0, z0, tnx0, tny0, tnz0;
	float x1, y1, z1, tnx1, tny1, tnz1;
	float x2, y2, z2, tnx2, tny2, tnz2;
	
	int rBase, gBase, bBase;
	int rSum, gSum, bSum;
	float /*nl,*/ dp, dist = 0.f, atten, intensity;
	/*float r_base1, g_base1, b_base1;
	float r_base2, g_base2, b_base2;*/



	unsigned int shadedColor = 0;

	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	/*сортируем индексы вершин в порядке v0.y <= v1.y <= v2.y, т.е. сверху вниз. положительная ось x направлена вправо, y - вниз*/
	if(face->tvlist[v1].y < face->tvlist[v0].y)
		SWAP(v0, v1, temp);

	if(face->tvlist[v2].y < face->tvlist[v0].y)
		SWAP(v0, v2, temp);

	if(face->tvlist[v2].y < face->tvlist[v1].y)
		SWAP(v1, v2, temp);

	if(face->tvlist[v0].y == face->tvlist[v1].y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		//Сортируем вершины слева направо, т.е. v0 будет слева
		if(face->tvlist[v1].x < face->tvlist[v0].x)
			SWAP(v1, v0, temp);
	}
	else if(face->tvlist[v1].y == face->tvlist[v2].y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		//Сортируем вершины слева направо, т.е. v1 будет слева
		if(face->tvlist[v2].x < face->tvlist[v1].x)
			SWAP(v2, v1, temp);
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = static_cast<float>((int)(face->tvlist[v0].x + 0.5f));
	y0 = static_cast<float>((int)(face->tvlist[v0].y + 0.5f));
	z0 = face->tvlist[v0].z;

	tnx0 = face->vlist[v0].n.x; tny0 = face->vlist[v0].n.y; tnz0 = face->vlist[v0].n.z;

	x1 = static_cast<float>((int)(face->tvlist[v1].x + 0.5f));
	y1 = static_cast<float>((int)(face->tvlist[v1].y + 0.5f));
	z1 = face->tvlist[v1].z;

	tnx1 = face->vlist[v1].n.x; tny1 = face->vlist[v1].n.y; tnz1 = face->vlist[v1].n.z;

	x2 = static_cast<float>((int)(face->tvlist[v2].x + 0.5f));
	y2 = static_cast<float>((int)(face->tvlist[v2].y + 0.5f));
	z2 = face->tvlist[v2].z;

	tnx2 = face->vlist[v2].n.x; tny2 = face->vlist[v2].n.y; tnz2 = face->vlist[v2].n.z;

	yrestart = static_cast<int>(y1);

	if(tri_type & TRI_TYPE_FLAT_MASK)
	{
		if(tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = 1.0f / (y2 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x2 - x0) * dy;
			dzdyl = (z2 - z0) * dy;
			dnxdyl = (tnx2 - tnx0) * dy;
			dnydyl = (tny2 - tny0) * dy;
			dnzdyl = (tnz2 - tnz0) * dy;

			wdyl.x = (face->worldPos[v2].x - face->worldPos[v0].x) * dy;
			wdyl.y = (face->worldPos[v2].y - face->worldPos[v0].y) * dy;
			wdyl.z = (face->worldPos[v2].z - face->worldPos[v0].z) * dy;

			dxdyr = (x2 - x1) * dy;
			dzdyr = (z2 - z1) * dy;
			dnxdyr = (tnx2 - tnx1) * dy;
			dnydyr = (tny2 - tny1) * dy;
			dnzdyr = (tnz2 - tnz1) * dy;

			wdyr.x = (face->worldPos[v2].x - face->worldPos[v1].x) * dy;
			wdyr.y = (face->worldPos[v2].y - face->worldPos[v1].y) * dy;
			wdyr.z = (face->worldPos[v2].z - face->worldPos[v1].z) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				zl = dzdyl * dy + z0;
				nxl = dnxdyl * dy + tnx0;
				nyl = dnydyl * dy + tny0;
				nzl = dnzdyl * dy + tnz0;

				wl.x = wdyl.x * dy + face->worldPos[v0].x;
				wl.y = wdyl.y * dy + face->worldPos[v0].y;
				wl.z = wdyl.z * dy + face->worldPos[v0].z;

				xr = dxdyr * dy + x1;
				zr = dzdyr * dy + z1;
				nxr = dnxdyr * dy + tnx1;
				nyr = dnydyr * dy + tny1;
				nzr = dnzdyr * dy + tnz1;

				wr.x = wdyr.x * dy + face->worldPos[v1].x;
				wr.y = wdyr.y * dy + face->worldPos[v1].y;
				wr.z = wdyr.z * dy + face->worldPos[v1].z;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				zl = z0;
				nxl = tnx0;
				nyl = tny0;
				nzl = tnz0;

				wl.x = face->worldPos[v0].x;
				wl.y = face->worldPos[v0].y;
				wl.z = face->worldPos[v0].z;

				xr = x1;
				zr = z1;
				nxr = tnx1;
				nyr = tny1;
				nzr = tnz1;

				wr.x = face->worldPos[v1].x;
				wr.y = face->worldPos[v1].y;
				wr.z = face->worldPos[v1].z;

				ystart = static_cast<int>(y0);
			}
		}
		else// if(tri_type == TRI_TYPE_FLAT_BOTTOM)
		{
			dy = 1.0f / (y1 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x1 - x0) * dy;
			dzdyl = (z1 - z0) * dy;
			dnxdyl = (tnx1 - tnx0) * dy;
			dnydyl = (tny1 - tny0) * dy;
			dnzdyl = (tnz1 - tnz0) * dy;

			wdyl.x = (face->worldPos[v1].x - face->worldPos[v0].x) * dy;
			wdyl.y = (face->worldPos[v1].y - face->worldPos[v0].y) * dy;
			wdyl.z = (face->worldPos[v1].z - face->worldPos[v0].z) * dy;

			dxdyr = (x2 - x0) * dy;
			dzdyr = (z2 - z0) * dy;
			dnxdyr = (tnx2 - tnx0) * dy;
			dnydyr = (tny2 - tny0) * dy;
			dnzdyr = (tnz2 - tnz0) * dy;

			wdyr.x = (face->worldPos[v2].x - face->worldPos[v0].x) * dy;
			wdyr.y = (face->worldPos[v2].y - face->worldPos[v0].y) * dy;
			wdyr.z = (face->worldPos[v2].z - face->worldPos[v0].z) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				zl = dzdyl * dy + z0;
				nxl = dnxdyl * dy + tnx0;
				nyl = dnydyl * dy + tny0;
				nzl = dnzdyl * dy + tnz0;

				wl.x = wdyl.x * dy + face->worldPos[v0].x;
				wl.y = wdyl.y * dy + face->worldPos[v0].y;
				wl.z = wdyl.z * dy + face->worldPos[v0].z;

				xr = dxdyr * dy + x0;
				zr = dzdyr * dy + z0;
				nxr = dnxdyr * dy + tnx0;
				nyr = dnydyr * dy + tny0;
				nzr = dnzdyr * dy + tnz0;

				wr.x = wdyr.x * dy + face->worldPos[v0].x;
				wr.y = wdyr.y * dy + face->worldPos[v0].y;
				wr.z = wdyr.z * dy + face->worldPos[v0].z;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				zl = z0;
				nxl = tnx0;
				nyl = tny0;
				nzl = tnz0;

				wl.x = face->worldPos[v0].x;
				wl.y = face->worldPos[v0].y;
				wl.z = face->worldPos[v0].z;

				xr = x0;
				zr = z0;
				nxr = tnx0;
				nyr = tny0;
				nzr = tnz0;

				wr.x = face->worldPos[v0].x;
				wr.y = face->worldPos[v0].y;
				wr.z = face->worldPos[v0].z;

				ystart = static_cast<int>(y0);
			}
		}

		if((yend = static_cast<int>(y2)) > this->maxClipY)
		{
			yend = static_cast<int>(this->maxClipY);
		}

		/*if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{*/
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				nxi = nxl;
				nzi = nyl;
				nyi = nzl;
				zi = zl;

				wl.VECTOR4D_COPY(&wi);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					dnx = (nxr - nxl) * dx;
					dny = (nyr - nyl) * dx;
					dnz = (nzr - nzl) * dx;
					dz = (zr - zl) * dx;

					dw.x = (wr.x - wl.x) * dx;
					dw.y = (wr.y - wl.y) * dx;
					dw.z = (wr.z - wl.z) * dx;
				}
				else
				{
					dnx = (nxr - nxl);
					dny = (nyr - nyl);
					dnz = (nzr - nzl);
					dz = (zr - zl);

					dw.x = (wr.x - wl.x);
					dw.y = (wr.y - wl.y);
					dw.z = (wr.z - wl.z);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					nxi += dnx * dx;
					nyi += dny * dx;
					nzi += dnz * dx;
					zi += dz * dx;

					wi.x += dw.x * dx;
					wi.y += dw.y * dx;
					wi.z += dw.z * dx;

					xstart = static_cast<int>(this->minClipX);
				}
				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					shadedColor = 0;

					RGBFROM32BIT(face->lit_color[0], rBase, gBase, bBase);
					rSum = gSum = bSum = 0;

					for(int currLight = 0; currLight < lights->getNumLights(); currLight++)
					{
						if(lights->lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
						{
							rSum += (lights->lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
							gSum += (lights->lights[currLight].c_ambient.g * gBase) >> 8;
							bSum += (lights->lights[currLight].c_ambient.b * bBase) >> 8;
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
						{
							VECTOR4D n;

							n.x = nxi;
							n.y = nyi;
							n.z = nzi;
							n.w = 1.0f;
							n.VECTOR4D_Normalize();
							//float nl = n.VECTOR4D_Length_Fast();
							//float i = 128 * dp / nl;
							dp  = n.VECTOR4D_Dot(&lights->lights[currLight].dir);

							if(dp > 0)
							{
								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * dp) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * dp) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * dp) >> 8;
							}
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_POINT)
						{
							VECTOR4D n, l, p3D;

							n.x = nxi;
							n.y = nyi;
							n.z = nzi;
							n.w = 1.0f;

							p3D.x = wi.x;
							p3D.y = wi.y;
							p3D.z = wi.z;
							p3D.w = 1.0f;

							n.VECTOR4D_Normalize();
							l.VECTOR4D_Build(&p3D, &lights->lights[currLight].pos);
							l.VECTOR4D_Normalize();
							dp = n.VECTOR4D_Dot(&l);

							if(dp > 0)
							{
								atten = (lights->lights[currLight].kc + lights->lights[currLight].kl * dist + lights->lights[currLight].kq * dist * dist);
								intensity = dp / (atten);

								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * intensity) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * intensity) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * intensity) >> 8;
							}
						}
					}

					if(rSum > 255) rSum = 255;
					if(gSum > 255) gSum = 255;
					if(bSum > 255) bSum = 255;

					shadedColor = ARGB32BIT(255, rSum, gSum, bSum);

					screen_ptr[xi] = shadedColor;

					nxi += dnx;
					nyi += dny;
					nzi += dnz;
					zi += dz;

					wi.x += dw.x;
					wi.y += dw.y;
					wi.z += dw.z;
				}

				xl += dxdyl;
				zl += dzdyl;
				nxl += dnxdyl;
				nyl += dnydyl;
				nzl += dnzdyl;

				wl.x += wdyl.x;
				wl.y += wdyl.y;
				wl.z += wdyl.z;

				xr += dxdyr;
				zr += dzdyr;
				nxr += dnxdyr;
				nyr += dnydyr;
				nzr += dnzdyr;

				wr.x += wdyr.x;
				wr.y += wdyr.y;
				wr.z += wdyr.z;

				screen_ptr += lpitch;
			}
		//}
	}
	else if(tri_type == TRI_TYPE_GENERAL)
	{
		if((yend = static_cast<int>(y2)) > this->maxClipY)
			yend = static_cast<int>(this->maxClipY);

		if(y1 < this->minClipY)
		{
			dyl = 1.0f / (y2 - y1);
			dxdyl = (x2 - x1) * dyl;
			dzdyl = (z2 - z1) * dyl;
			dnxdyl = (tnx2 - tnx1) * dyl;
			dnydyl = (tny2 - tny1) * dyl;
			dnzdyl = (tnz2 - tnz1) * dyl;

			wdyl.x = (face->worldPos[v2].x - face->worldPos[v1].x) * dyl;
			wdyl.y = (face->worldPos[v2].y - face->worldPos[v1].y) * dyl;
			wdyl.z = (face->worldPos[v2].z - face->worldPos[v1].z) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dnxdyr = (tnx2 - tnx0) * dyr;
			dnydyr = (tny2 - tny0) * dyr;
			dnzdyr = (tnz2 - tnz0) * dyr;

			wdyr.x = (face->worldPos[v2].x - face->worldPos[v0].x) * dyr;
			wdyr.y = (face->worldPos[v2].y - face->worldPos[v0].y) * dyr;
			wdyr.z = (face->worldPos[v2].z - face->worldPos[v0].z) * dyr;

			dyr = (this->minClipY - y0);
			dyl = (this->minClipY - y1);

			xl = dxdyl * dyl + x1;
			zl = dzdyl * dyl + z1;
			nxl = dnxdyl * dyl + tnx1;
			nyl = dnydyl * dyl + tny1;
			nzl = dnzdyl * dyl + tnz1;

			wl.x = wdyl.x * dyl + face->worldPos[v1].x;
			wl.y = wdyl.y * dyl + face->worldPos[v1].y;
			wl.z = wdyl.z * dyl + face->worldPos[v1].z;

			xr = dxdyr * dyr + x0;
			zr = dzdyr * dyr + z0;
			nxr = dnxdyr * dyr + tnx0;
			nyr = dnydyr * dyr + tny0;
			nzr = dnzdyr * dyr + tnz0;

			wr.x = wdyr.x * dyr + face->worldPos[v0].x;
			wr.y = wdyr.y * dyr + face->worldPos[v0].y;
			wr.z = wdyr.z * dyr + face->worldPos[v0].z;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dzdyl,dzdyr,tempf);
				SWAP(dnxdyl,dnxdyr,tempf);
				SWAP(dnydyl,dnydyr,tempf);
				SWAP(dnzdyl,dnzdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(zl,zr,tempf);
				SWAP(nxl,nxr,tempf);
				SWAP(nyl,nyr,tempf);
				SWAP(nzl,nzr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(z1,z2,tempf);
				SWAP(tnx1,tnx2,tempf);
				SWAP(tny1,tny2,tempf);
				SWAP(tnz1,tnz2,tempf);

				wdyl.VECTOR4D_SWAP(&wdyr);
				wl.VECTOR4D_SWAP(&wr);
				face->worldPos[v1].VECTOR4D_SWAP(&face->worldPos[v2]);

				irestart = INTERP_RHS;
			}
		}
		else if(y0 < this->minClipY)
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dnxdyl = (tnx1 - tnx0) * dyl;
			dnydyl = (tny1 - tny0) * dyl;
			dnzdyl = (tnz1 - tnz0) * dyl;

			wdyl.x = (face->worldPos[v1].x - face->worldPos[v0].x) * dyl;
			wdyl.y = (face->worldPos[v1].y - face->worldPos[v0].y) * dyl;
			wdyl.z = (face->worldPos[v1].z - face->worldPos[v0].z) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dnxdyr = (tnx2 - tnx0) * dyr;
			dnydyr = (tny2 - tny0) * dyr;
			dnzdyr = (tnz2 - tnz0) * dyr;

			wdyr.x = (face->worldPos[v2].x - face->worldPos[v0].x) * dyr;
			wdyr.y = (face->worldPos[v2].y - face->worldPos[v0].y) * dyr;
			wdyr.z = (face->worldPos[v2].z - face->worldPos[v0].z) * dyr;

			dy = (this->minClipY - y0);

			xl = dxdyl * dy + x0;
			zl = dzdyl * dy + z0;
			nxl = dnxdyl * dy + tnx0;
			nyl = dnydyl * dy + tny0;
			nzl = dnzdyl * dy + tnz0;

			wl.x = wdyl.x * dy + face->worldPos[v0].x;
			wl.y = wdyl.y * dy + face->worldPos[v0].y;
			wl.z = wdyl.z * dy + face->worldPos[v0].z;

			xr = dxdyr * dy + x0;
			zr = dzdyr * dy + z0;
			nxr = dnxdyr * dy + tnx0;
			nyr = dnydyr * dy + tny0;
			nzr = dnzdyr * dy + tnz0;

			wr.x = wdyr.x * dy + face->worldPos[v0].x;
			wr.y = wdyr.y * dy + face->worldPos[v0].y;
			wr.z = wdyr.z * dy + face->worldPos[v0].z;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dzdyl,dzdyr,tempf);
				SWAP(dnxdyl,dnxdyr,tempf);
				SWAP(dnydyl,dnydyr,tempf);
				SWAP(dnzdyl,dnzdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(zl,zr,tempf);
				SWAP(nxl,nxr,tempf);
				SWAP(nyl,nyr,tempf);
				SWAP(nzl,nzr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(z1,z2,tempf);
				SWAP(tnx1,tnx2,tempf);
				SWAP(tny1,tny2,tempf);
				SWAP(tnz1,tnz2,tempf);

				wdyl.VECTOR4D_SWAP(&wdyr);
				wl.VECTOR4D_SWAP(&wr);
				face->worldPos[v1].VECTOR4D_SWAP(&face->worldPos[v2]);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			dzdyl = (z1 - z0) * dyl;
			dnxdyl = (tnx1 - tnx0) * dyl;
			dnydyl = (tny1 - tny0) * dyl;
			dnzdyl = (tnz1 - tnz0) * dyl;

			wdyl.x = (face->worldPos[v1].x - face->worldPos[v0].x) * dyl;
			wdyl.y = (face->worldPos[v1].y - face->worldPos[v0].y) * dyl;
			wdyl.z = (face->worldPos[v1].z - face->worldPos[v0].z) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			dzdyr = (z2 - z0) * dyr;
			dnxdyr = (tnx2 - tnx0) * dyr;
			dnydyr = (tny2 - tny0) * dyr;
			dnzdyr = (tnz2 - tnz0) * dyr;

			wdyr.x = (face->worldPos[v2].x - face->worldPos[v0].x) * dyr;
			wdyr.y = (face->worldPos[v2].y - face->worldPos[v0].y) * dyr;
			wdyr.z = (face->worldPos[v2].z - face->worldPos[v0].z) * dyr;

			xl = xr = x0;
			zl = zr = z0;
			nxl = nxr = tnx0;
			nyl = nyr = tny0;
			nzl = nzr = tnz0;

			wl.x = wr.x = face->worldPos[v0].x;
			wl.y = wr.y = face->worldPos[v0].y;
			wl.z = wr.z = face->worldPos[v0].z;

			ystart = static_cast<int>(y0);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(dzdyl,dzdyr,tempf);
				SWAP(dnxdyl,dnxdyr,tempf);
				SWAP(dnydyl,dnydyr,tempf);
				SWAP(dnzdyl,dnzdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(zl,zr,tempf);
				SWAP(nxl,nxr,tempf);
				SWAP(nyl,nyr,tempf);
				SWAP(nzl,nzr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(z1,z2,tempf);
				SWAP(tnx1,tnx2,tempf);
				SWAP(tny1,tny2,tempf);
				SWAP(tnz1,tnz2,tempf);

				wdyl.VECTOR4D_SWAP(&wdyr);
				wl.VECTOR4D_SWAP(&wr);
				face->worldPos[v1].VECTOR4D_SWAP(&face->worldPos[v2]);

				irestart = INTERP_RHS;
			}
		}

		/*if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{*/

			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				nxi = nxl;
				nyi = nyl;
				nzi = nzl;
				zi = zl;

				wl.VECTOR4D_COPY(&wi);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					dnx = (nxr - nxl) * dx;
					dny = (nyr - nyl) * dx;
					dnz = (nzr - nzl) * dx;
					dz = (zr - zl) * dx;

					dw.x = (wr.x - wl.x) * dx;
					dw.y = (wr.y - wl.y) * dx;
					dw.z = (wr.z - wl.z) * dx;
				}
				else
				{
					dnx = (nxr - nxl);
					dny = (nyr - nyl);
					dnz = (nzr - nzl);
					dz = (zr - zl);

					dw.x = (wr.x - wl.x);
					dw.y = (wr.y - wl.y);
					dw.z = (wr.z - wl.z);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					nxi += dnx * dx;
					nyi += dny * dx;
					nzi += dnz * dx;
					zi += dz * dx;

					wi.x += dw.x * dx;
					wi.y += dw.y * dx;
					wi.z += dw.z * dx;

					xstart = static_cast<int>(this->minClipX);
				}
				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					shadedColor = 0;

					RGBFROM32BIT(face->lit_color[0], rBase, gBase, bBase);
					rSum = gSum = bSum = 0;

					for(int currLight = 0; currLight < lights->getNumLights(); currLight++)
					{
						if(lights->lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
						{
							rSum += (lights->lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
							gSum += (lights->lights[currLight].c_ambient.g * gBase) >> 8;
							bSum += (lights->lights[currLight].c_ambient.b * bBase) >> 8;
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
						{
							VECTOR4D n;

							n.x = nxi;
							n.y = nyi;
							n.z = nzi;
							n.w = 1.0f;
							n.VECTOR4D_Normalize();
							//float nl = n.VECTOR4D_Length_Fast();
							dp  = n.VECTOR4D_Dot(&lights->lights[currLight].dir);

							if(dp > 0)
							{
								/*float i = 128 * dp / nl;

								rSum += (lights->lights[currLight].c_diffuse.r * rBase * i) / (256 * 128);
								gSum += (lights->lights[currLight].c_diffuse.g * gBase * i) / (256 * 128);
								bSum += (lights->lights[currLight].c_diffuse.b * bBase * i) / (256 * 128);*/

								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * dp) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * dp) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * dp) >> 8;
							}
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_POINT)
						{
							/*Работает неправильно!!! Нужно попробовать приобразовать p3D еще с помощью обратной матрицы камеры, чтобы перейти к мировым координатам
							(сейчас p3D в координатах камеры!!!). Или сохранять мировые координаты вершин и их потом интерполировать(скорее всего!!!!...РАБОТАЕТ!!!!). Или преобразовывать 
							всё(нормали к пикселям) в координаты камеры.*/
							VECTOR4D n, l, p3D;
							/*MATRIX4X4 mcamInv;*/
							/*VECTOR4D pR;*/
							/*float alpha, beta;*/

							n.x = nxi;
							n.y = nyi;
							n.z = nzi;
							n.w = 1.0f;

							//вычисляем положение пикселя в 3D пространстве
							/*alpha = (0.5f * cam->viewport_width - 0.5f);
							beta = (0.5f * cam->viewport_height - 0.5f);

							p3D.x = (((xi - alpha) / alpha) / cam->view_dist) * zi;
							p3D.y = (((yi + beta) / beta) / cam->aspect_ratio / cam->view_dist) * zi;
							p3D.z = zi;
							p3D.w = 1.0f;*/

							p3D.x = wi.x;
							p3D.y = wi.y;
							p3D.z = wi.z;
							p3D.w = 1.0f;

							/*cam->mcam.Mat_Inverse_4X4(&mcamInv);

							p3D.Mat_Mul_VECTOR4D_4X4(&mcamInv, &pR);
							pR.VECTOR4D_COPY(&p3D);*/

							/*nl = n.VECTOR4D_Length_Fast();
							//nl = 1.0f;
							n.VECTOR4D_Normalize();

							l.VECTOR4D_Build(&face->tvlist[v0].v, &lights->lights[currLight].pos);

							dist = l.VECTOR4D_Length_Fast();

							dp = n.VECTOR4D_Dot(&l);*/

							n.VECTOR4D_Normalize();
							l.VECTOR4D_Build(&p3D, &lights->lights[currLight].pos);
							l.VECTOR4D_Normalize();
							dp = n.VECTOR4D_Dot(&l);

							if(dp > 0)
							{
								atten = (lights->lights[currLight].kc + lights->lights[currLight].kl * dist + lights->lights[currLight].kq * dist * dist);

								//intensity = dp / (nl * dist * atten);
								intensity = dp / (atten);

								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * intensity) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * intensity) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * intensity) >> 8;
							}
						}
					}

					if(rSum > 255) rSum = 255;
					if(gSum > 255) gSum = 255;
					if(bSum > 255) bSum = 255;

					shadedColor = ARGB32BIT(255, rSum, gSum, bSum);

					screen_ptr[xi] = shadedColor;

					nxi += dnx;
					nyi += dny;
					nzi += dnz;
					zi += dz;

					wi.x += dw.x;
					wi.y += dw.y;
					wi.z += dw.z;
				}

				xl += dxdyl;
				zl += dzdyl;
				nxl += dnxdyl;
				nyl += dnydyl;
				nzl += dnzdyl;

				wl.x += wdyl.x;
				wl.y += wdyl.y;
				wl.z += wdyl.z;

				xr += dxdyr;
				zr += dzdyr;
				nxr += dnxdyr;
				nyr += dnydyr;
				nzr += dnzdyr;

				wr.x += wdyr.x;
				wr.y += wdyr.y;
				wr.z += wdyr.z;

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						dzdyl = (z2 - z1) * dyl;
						dnxdyl = (tnx2 - tnx1) * dyl;
						dnydyl = (tny2 - tny1) * dyl;
						dnzdyl = (tnz2 - tnz1) * dyl;

						wdyl.x = (face->worldPos[v2].x - face->worldPos[v1].x) * dyl;
						wdyl.y = (face->worldPos[v2].y - face->worldPos[v1].y) * dyl;
						wdyl.z = (face->worldPos[v2].z - face->worldPos[v1].z) * dyl;

						xl = x1;
						zl = z1;
						nxl = tnx1;
						nyl = tny1;
						nzl = tnz1;

						wl.x = face->worldPos[v1].x;
						wl.y = face->worldPos[v1].y;
						wl.z = face->worldPos[v1].z;

						xl += dxdyl;
						zl += dzdyl;
						nxl += dnxdyl;
						nyl += dnydyl;
						nzl += dnzdyl;

						wl.x += wdyl.x;
						wl.y += wdyl.y;
						wl.z += wdyl.z;
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						dzdyr = (z1 - z2) * dyr;
						dnxdyr = (tnx1 - tnx2) * dyr;
						dnydyr = (tny1 - tny2) * dyr;
						dnzdyr = (tnz1 - tnz2) * dyr;

						wdyr.x = (face->worldPos[v1].x - face->worldPos[v2].x) * dyr;
						wdyr.y = (face->worldPos[v1].y - face->worldPos[v2].y) * dyr;
						wdyr.z = (face->worldPos[v1].z - face->worldPos[v2].z) * dyr;

						xr = x2;
						zr = z2;
						nxr = tnx2;
						nyr = tny2;
						nzr = tnz2;

						wr.x = face->worldPos[v2].x;
						wr.y = face->worldPos[v2].y;
						wr.z = face->worldPos[v2].z;

						xr += dxdyr;
						zr += dzdyr;
						nxr += dnxdyr;
						nyr += dnydyr;
						nzr += dnzdyr;

						wr.x += wdyr.x;
						wr.y += wdyr.y;
						wr.z += wdyr.z;
					}
				}
			}

		//}
	}

	return 1;
}

int Draw32BitStrategy::DrawPhongTriangle2(struct3D::CAM4D_PTR cam, mat::LightSystem *lights, struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	if(!face) return 0;

	const int TRI_TYPE_NONE = 0;
	const int TRI_TYPE_FLAT_BOTTOM = 1;
	const int TRI_TYPE_FLAT_TOP = 2;
	const int TRI_TYPE_FLAT_MASK = 3;
	const int TRI_TYPE_GENERAL = 4;

	const int INTERP_LHS = 0;
	const int INTERP_RHS = 1;

	int v0 = 0, v1 = 1, v2 = 2,
	temp = 0,
	tri_type = TRI_TYPE_NONE,
	irestart = INTERP_LHS;

	float dx, dy, dyl, dyr;
	float tempf = 0;

	int xi, yi;

	/*int index_x, index_y;*/
	/*int x, y;*/
	int xstart, xend, ystart, yrestart, yend;

	math3D::VECTOR4D vt;

	math3D::VECTOR4D dn, dw;
	math3D::VECTOR4D ni, wi;

	//Интерполяция по левой стороне
	float dxdyl, xl;//интерпояция значений по оси X
	math3D::VECTOR4D dndyl, nl;//значений нормали к каждому пикселю
	math3D::VECTOR4D dwdyl, wl;//положения каждого пикселя в мировых координитах

	//Интерполяция по правой стороне
	float dxdyr, xr;//интерпояция значений по оси X
	math3D::VECTOR4D dndyr, nr;//и значений нормали к каждому пикселю
	math3D::VECTOR4D dwdyr, wr;//положения каждого пикселя в мировых координитах

	float x0, y0, x1, y1, x2, y2;
	math3D::VECTOR4D tn0, tn1, tn2;

	int rBase, gBase, bBase;
	int rSum, gSum, bSum;
	float dp, dpSpec, dist, atten, intensity;

	unsigned int shadedColor = 0;

	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = 0,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	if(face->tvlist[v1].y < face->tvlist[v0].y)
		SWAP(v0, v1, temp);

	if(face->tvlist[v2].y < face->tvlist[v0].y)
		SWAP(v0, v2, temp);

	if(face->tvlist[v2].y < face->tvlist[v1].y)
		SWAP(v1, v2, temp);


	if(face->tvlist[v0].y == face->tvlist[v1].y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		//Сортируем вершины слева направо, т.е. v0 будет слева
		if(face->tvlist[v1].x < face->tvlist[v0].x)
			SWAP(v1, v0, temp);
	}
	else if(face->tvlist[v1].y == face->tvlist[v2].y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		//Сортируем вершины слева направо, т.е. v1 будет слева
		if(face->tvlist[v2].x < face->tvlist[v1].x)
			SWAP(v2, v1, temp);
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	x0 = static_cast<float>((int)(face->tvlist[v0].x + 0.5f));
	y0 = static_cast<float>((int)(face->tvlist[v0].y + 0.5f));
	vecCopy(&face->vlist[v0].n, &tn0);

	x1 = static_cast<float>((int)(face->tvlist[v1].x + 0.5f));
	y1 = static_cast<float>((int)(face->tvlist[v1].y + 0.5f));
	vecCopy(&face->vlist[v1].n, &tn1);

	x2 = static_cast<float>((int)(face->tvlist[v2].x + 0.5f));
	y2 = static_cast<float>((int)(face->tvlist[v2].y + 0.5f));
	vecCopy(&face->vlist[v2].n, &tn2);

	yrestart = static_cast<int>(y1);

	if(tri_type & TRI_TYPE_FLAT_MASK)
	{
		if(tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = 1.0f / (y2 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x2 - x0) * dy;
			vecSub(&tn2, &tn0, &dndyl);
			vecScale(dy, &dndyl);
			vecSub(&face->worldPos[v2], &face->worldPos[v0], &dwdyl);
			vecScale(dy, &dwdyl);

			dxdyr = (x2 - x1) * dy;
			vecSub(&tn2, &tn1, &dndyr);
			vecScale(dy, &dndyr);
			vecSub(&face->worldPos[v2], &face->worldPos[v1], &dwdyr);
			vecScale(dy, &dwdyr);

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				vecScale(dy, &dndyl, &nl);
				vecAdd(&nl, &tn0, &nl);
				vecScale(dy, &dwdyl, &wl);
				vecAdd(&wl, &face->worldPos[v0], &wl);

				xr = dxdyr * dy + x1;
				vecScale(dy, &dndyr, &nr);
				vecAdd(&nr, &tn1, &nr);
				vecScale(dy, &dwdyr, &wr);
				vecAdd(&wr, &face->worldPos[v1], &wr);

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				vecCopy(&tn0, &nl);
				vecCopy(&face->worldPos[v0], &wl);

				xr = x1;
				vecCopy(&tn1, &nr);
				vecCopy(&face->worldPos[v1], &wr);

				ystart = static_cast<int>(y0);
			}
		}
		else
		{
			dy = 1.0f / (y1 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x1 - x0) * dy;
			vecSub(&tn1, &tn0, &dndyl);
			vecScale(dy, &dndyl);
			vecSub(&face->worldPos[v1], &face->worldPos[v0], &dwdyl);
			vecScale(dy, &dwdyl);

			dxdyr = (x2 - x0) * dy;
			vecSub(&tn2, &tn0, &dndyr);
			vecScale(dy, &dndyr);
			vecSub(&face->worldPos[v2], &face->worldPos[v0], &dwdyr);
			vecScale(dy, &dwdyr);

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				vecScale(dy, &dndyl, &nl);
				vecAdd(&nl, &tn0, &nl);
				vecScale(dy, &dwdyl, &wl);
				vecAdd(&wl, &face->worldPos[v0], &wl);

				xr = dxdyr * dy + x0;
				vecScale(dy, &dndyr, &nr);
				vecAdd(&nr, &tn0, &nr);
				vecScale(dy, &dwdyr, &wr);
				vecAdd(&wr, &face->worldPos[v0], &wr);

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				vecCopy(&tn0, &nl);
				vecCopy(&face->worldPos[v0], &wl);

				xr = x0;
				vecCopy(&tn0, &nr);
				vecCopy(&face->worldPos[v0], &wr);

				ystart = static_cast<int>(y0);
			}
		}

		if((yend = static_cast<int>(y2)) > this->maxClipY)
		{
			yend = static_cast<int>(this->maxClipY);
		}

		//if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		//{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecCopy(&wl, &wi);
				vecCopy(&nl, &ni);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					vecSub(&nr, &nl, &dn);
					vecScale(dx, &dn);

					vecSub(&wr, &wl, &dw);
					vecScale(dx, &dw);
				}
				else
				{
					vecSub(&nr, &nl, &dn);
					vecSub(&wr, &wl, &dw);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					vecScale(dx, &dn, &vt);
					vecAdd(&ni, &vt, &ni);

					vecScale(dx, &dw, &vt);
					vecAdd(&wi, &vt, &wi);

					xstart = static_cast<int>(this->minClipX);
				}

				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					shadedColor = 0;

					RGBFROM32BIT(face->lit_color[0], rBase, gBase, bBase);
					rSum = gSum = bSum = 0;
					
					for(int currLight = 0; currLight < lights->getNumLights(); currLight++)
					{
						if(lights->lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
						{
							rSum += (lights->lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
							gSum += (lights->lights[currLight].c_ambient.g * gBase) >> 8;
							bSum += (lights->lights[currLight].c_ambient.b * bBase) >> 8;
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
						{
							VECTOR4D n, /*p3D, l,*/ c, r;

							vecCopy(&ni, &n);
							vecNormalize(&n);

							//float nl = n.VECTOR4D_Length_Fast();
							//float i = 128 * dp / nl;
							dp  = n.VECTOR4D_Dot(&lights->lights[currLight].dir);

							if(dp > 0)
							{
								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * dp) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * dp) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * dp) >> 8;

								vecReflect(&lights->lights[currLight].tdir, &n, &r);
								c.w = 0;
								c.x = wi.x - cam->pos.x;
								c.y = wi.y - cam->pos.y;
								c.z = wi.z - cam->pos.z;
								vecNormalize(&r);
								vecNormalize(&c);
								dpSpec = vecDot(&r, &c);

								if(dpSpec > 0)
								{
									dpSpec = pow(dpSpec, lights->lights[currLight].pf);
									intensity = dpSpec * dp;

									rSum += static_cast<int>((lights->lights[currLight].c_specular.r * rBase * intensity) / 256);
									gSum += static_cast<int>((lights->lights[currLight].c_specular.g * gBase * intensity) / 256);
									bSum += static_cast<int>((lights->lights[currLight].c_specular.b * bBase * intensity) / 256);
								}
							}
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_POINT)
						{
							VECTOR4D n, l, p3D, c, r;

							vecCopy(&ni, &n);
							vecCopy(&wi, &p3D);
							vecBuild(&p3D, &lights->lights[currLight].pos, &l);

							dist = vecLengthFast(&l);

							vecNormalize(&n);
							vecNormalize(&l);
							dp = vecDot(&n, &l);

							if(dp > 0)
							{
								atten = (lights->lights[currLight].kc + lights->lights[currLight].kl * dist + lights->lights[currLight].kq * dist * dist);
								intensity = dp / (atten);

								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * intensity) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * intensity) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * intensity) >> 8;

								vecReflect(&l, &n, &r);
								c.w = 0;
								c.x = wi.x - cam->pos.x;
								c.y = wi.y - cam->pos.y;
								c.z = wi.z - cam->pos.z;
								vecNormalize(&r);
								vecNormalize(&c);
								dpSpec = vecDot(&r, &c);

								if(dpSpec > 0)
								{
									dpSpec = pow(dpSpec, lights->lights[currLight].pf);
									intensity = dpSpec * dp / atten;

									rSum += static_cast<int>((lights->lights[currLight].c_specular.r * rBase * intensity) / 256);
									gSum += static_cast<int>((lights->lights[currLight].c_specular.g * gBase * intensity) / 256);
									bSum += static_cast<int>((lights->lights[currLight].c_specular.b * bBase * intensity) / 256);
								}
							}
						}
					}
					
					if(rSum > 255) rSum = 255;
					if(gSum > 255) gSum = 255;
					if(bSum > 255) bSum = 255;

					shadedColor = ARGB32BIT(255, rSum, gSum, bSum);

					screen_ptr[xi] = shadedColor;

					vecAdd(&ni, &dn, &ni);
					vecAdd(&wi, &dw, &wi);
				}

				xl += dxdyl;
				vecAdd(&nl, &dndyl, &nl);
				vecAdd(&wl, &dwdyl, &wl);

				xr += dxdyr;
				vecAdd(&nr, &dndyr, &nr);
				vecAdd(&wr, &dwdyr, &wr);

				screen_ptr += lpitch;
			}
		//}
			
	}
	else if(tri_type == TRI_TYPE_GENERAL)
	{
		if((yend = static_cast<int>(y2)) > this->maxClipY)
			yend = static_cast<int>(this->maxClipY);

		if(y1 < this->minClipY)
		{
			dyl = 1.0f / (y2 - y1);
			dxdyl = (x2 - x1) * dyl;
			vecSub(&tn2, &tn1, &dndyl);
			vecScale(dyl, &dndyl);
			vecSub(&face->worldPos[v2], &face->worldPos[v1], &dwdyl);
			vecScale(dyl, &dwdyl);

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecSub(&tn2, &tn0, &dndyr);
			vecScale(dyr, &dndyr);
			vecSub(&face->worldPos[v2], &face->worldPos[v0], &dwdyr);
			vecScale(dyr, &dwdyr);

			dyr = (this->minClipY - y0);
			dyl = (this->minClipY - y1);

			xl = dxdyl * dyl + x1;
			vecScale(dyl, &dndyl, &nl);
			vecAdd(&nl, &tn1, &nl);
			vecScale(dyl, &dwdyl, &wl);
			vecAdd(&wl, &face->worldPos[v1], &wl);

			xr = dxdyr * dyr + x0;
			vecScale(dyr, &dndyr, &nr);
			vecAdd(&nr, &tn0, &nr);
			vecScale(dyr, &dwdyr, &wr);
			vecAdd(&wr, &face->worldPos[v0], &wr);

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);

				SWAP(v1, v2, temp);

				vecSwap(&dndyl, &dndyr);
				vecSwap(&nl, &nr);
				vecSwap(&tn1, &tn2);
				vecSwap(&dwdyl, &dwdyr);
				vecSwap(&wl, &wr);

				irestart = INTERP_RHS;
			}
		}
		else if(y0 < this->minClipY)
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			vecSub(&tn1, &tn0, &dndyl);
			vecScale(dyl, &dndyl);
			vecSub(&face->worldPos[v1], &face->worldPos[v0], &dwdyl);
			vecScale(dyl, &dwdyl);

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecSub(&tn2, &tn0, &dndyr);
			vecScale(dyr, &dndyr);
			vecSub(&face->worldPos[v2], &face->worldPos[v0], &dwdyr);
			vecScale(dyr, &dwdyr);

			dy = (this->minClipY - y0);

			xl = dxdyl * dy + x0;
			vecScale(dy, &dndyl, &nl);
			vecAdd(&nl, &tn0, &nl);
			vecScale(dy, &dwdyl, &wl);
			vecAdd(&wl, &face->worldPos[v0], &wl);

			xr = dxdyr * dy + x0;
			vecScale(dy, &dndyr, &nr);
			vecAdd(&nr, &tn0, &nr);
			vecScale(dy, &dwdyr, &wr);
			vecAdd(&wr, &face->worldPos[v0], &wr);

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);

				SWAP(v1, v2, temp);

				vecSwap(&dndyl, &dndyr);
				vecSwap(&nl, &nr);
				vecSwap(&tn1, &tn2);
				vecSwap(&dwdyl, &dwdyr);
				vecSwap(&wl, &wr);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			vecSub(&tn1, &tn0, &dndyl);
			vecScale(dyl, &dndyl);
			vecSub(&face->worldPos[v1], &face->worldPos[v0], &dwdyl);
			vecScale(dyl, &dwdyl);

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecSub(&tn2, &tn0, &dndyr);
			vecScale(dyr, &dndyr);
			vecSub(&face->worldPos[v2], &face->worldPos[v0], &dwdyr);
			vecScale(dyr, &dwdyr);

			xl = xr = x0;
			vecCopy(&tn0, &nl);
			vecCopy(&tn0, &nr);
			vecCopy(&face->worldPos[v0], &wl);
			vecCopy(&face->worldPos[v0], &wr);

			ystart = static_cast<int>(y0);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);

				SWAP(v1, v2, temp);

				vecSwap(&dndyl, &dndyr);
				vecSwap(&nl, &nr);
				vecSwap(&tn1, &tn2);
				vecSwap(&dwdyl, &dwdyr);
				vecSwap(&wl, &wr);

				irestart = INTERP_RHS;
			}
		}

		/*if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{*/

		screen_ptr = dest_buffer + (ystart * lpitch);

		for(yi = ystart; yi <= yend; yi++)
		{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecCopy(&nl, &ni);
				vecCopy(&wl, &wi);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					vecSub(&nr, &nl, &dn);
					vecScale(dx, &dn);
					vecSub(&wr, &wl, &dw);
					vecScale(dx, &dw);
				}
				else
				{
					vecSub(&nr, &nl, &dn);
					vecSub(&wr, &wl, &dw);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					vecScale(dx, &dn, &vt);
					vecAdd(&ni, &vt, &ni);
					vecScale(dx, &dw, &vt);
					vecAdd(&wi, &vt, &wi);

					xstart = static_cast<int>(this->minClipX);
				}

				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					shadedColor = 0;

					RGBFROM32BIT(face->lit_color[0], rBase, gBase, bBase);
					rSum = gSum = bSum = 0;
					float dpSpec;
					float i;

					for(int currLight = 0; currLight < lights->getNumLights(); currLight++)
					{
						if(lights->lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
						{
							rSum += (lights->lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
							gSum += (lights->lights[currLight].c_ambient.g * gBase) >> 8;
							bSum += (lights->lights[currLight].c_ambient.b * bBase) >> 8;
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
						{
							VECTOR4D n, /*p3D, l,*/ c, r;

							vecCopy(&ni, &n);
							vecNormalize(&n);

							//float nl = n.VECTOR4D_Length_Fast();
							//float i = 128 * dp / nl;
							dp  = n.VECTOR4D_Dot(&lights->lights[currLight].dir);

							if(dp > 0)
							{
								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * dp) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * dp) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * dp) >> 8;

								vecReflect(&lights->lights[currLight].tdir, &n, &r);
								c.w = 0;
								c.x = wi.x - cam->pos.x;
								c.y = wi.y - cam->pos.y;
								c.z = wi.z - cam->pos.z;
								vecNormalize(&r);
								vecNormalize(&c);
								dpSpec = vecDot(&r, &c);

								if(dpSpec > 0)
								{
									dpSpec = pow(dpSpec, lights->lights[currLight].pf);
									i = dpSpec * dp;

									rSum += static_cast<int>((lights->lights[currLight].c_specular.r * rBase * i) / 256);
									gSum += static_cast<int>((lights->lights[currLight].c_specular.g * gBase * i) / 256);
									bSum += static_cast<int>((lights->lights[currLight].c_specular.b * bBase * i) / 256);
								}
							}
						}
						else if(lights->lights[currLight].attr & mat::LIGHT_ATTR_POINT)
						{
							VECTOR4D n, l, p3D, c, r;

							vecCopy(&ni, &n);
							vecCopy(&wi, &p3D);
							vecBuild(&p3D, &lights->lights[currLight].pos, &l);

							dist = vecLengthFast(&l);

							vecNormalize(&n);
							vecNormalize(&l);
							dp = vecDot(&n, &l);

							if(dp > 0)
							{
								atten = (lights->lights[currLight].kc + lights->lights[currLight].kl * dist + lights->lights[currLight].kq * dist * dist);
								intensity = dp / (atten);

								rSum += (int)(lights->lights[currLight].c_diffuse.r * rBase * intensity) >> 8;
								gSum += (int)(lights->lights[currLight].c_diffuse.g * gBase * intensity) >> 8;
								bSum += (int)(lights->lights[currLight].c_diffuse.b * bBase * intensity) >> 8;

								vecReflect(&l, &n, &r);
								c.w = 0;
								c.x = wi.x - cam->pos.x;
								c.y = wi.y - cam->pos.y;
								c.z = wi.z - cam->pos.z;
								vecNormalize(&r);
								vecNormalize(&c);
								dpSpec = vecDot(&r, &c);

								if(dpSpec > 0)
								{
									dpSpec = pow(dpSpec, lights->lights[currLight].pf);
									i = dpSpec * dp / atten;

									rSum += static_cast<int>((lights->lights[currLight].c_specular.r * rBase * i) / 256);
									gSum += static_cast<int>((lights->lights[currLight].c_specular.g * gBase * i) / 256);
									bSum += static_cast<int>((lights->lights[currLight].c_specular.b * bBase * i) / 256);
								}
							}
						}
					}

					if(rSum > 255) rSum = 255;
					if(gSum > 255) gSum = 255;
					if(bSum > 255) bSum = 255;

					shadedColor = ARGB32BIT(255, rSum, gSum, bSum);

					screen_ptr[xi] = shadedColor;

					vecAdd(&ni, &dn, &ni);
					vecAdd(&wi, &dw, &wi);
				}

				xl += dxdyl;
				vecAdd(&nl, &dndyl, &nl);
				vecAdd(&wl, &dwdyl, &wl);

				xr += dxdyr;
				vecAdd(&nr, &dndyr, &nr);
				vecAdd(&wr, &dwdyr, &wr);

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						vecSub(&tn2, &tn1, &dndyl);
						vecScale(dyl, &dndyl);
						vecSub(&face->worldPos[v2], &face->worldPos[v1], &dwdyl);
						vecScale(dyl, &dwdyl);

						xl = x1;
						vecCopy(&tn1, &nl);
						vecCopy(&face->worldPos[v1], &wl);

						xl += dxdyl;
						vecAdd(&nl, &dndyl, &nl);
						vecAdd(&wl, &dwdyl, &wl);
					}
					else
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						vecSub(&tn1, &tn2, &dndyr);
						vecScale(dyr, &dndyr);
						vecSub(&face->worldPos[v1], &face->worldPos[v2], &dwdyr);
						vecScale(dyr, &dwdyr);

						xr = x2;
						vecCopy(&tn2, &nr);
						vecCopy(&face->worldPos[v2], &wr);

						xr += dxdyr;
						vecAdd(&nr, &dndyr, &nr);
						vecAdd(&wr, &dwdyr, &wr);
					}
				}
		}

		//}
	}

	return 1;
}

int Draw32BitStrategy::FullScreenShader(unsigned int *videoMemory, int lpitch)
{
	/*lpitch >>= 2;

	int start = this->minClipX + this->minClipY * lpitch;
	int end = this->maxClipX + this->maxClipY * lpitch; 
	unsigned int rSum = 0, gSum = 0, bSum = 0;
	const int numOffsets = 5;
	POINT offsets[numOffsets] = {{-2,0}, {-1,0}, {0,0}, {1,0}, {2,0}};//{{0,0}, {1,0}, {1,-1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};

	for(int y = this->minClipY; y < this->maxClipY; y++)
	{
		for(int x = this->minClipX; x < this->maxClipX; x++)
		{
			for(int i = 0; i < numOffsets; i++)
			{
				if((offsets[i].x + x) >= this->minClipX && (offsets[i].y + y) >= this->minClipY && (offsets[i].x + x) < this->maxClipX && (offsets[i].y + y) < this->maxClipY)
				{
					rSum += ((videoMemory[(offsets[i].x + x) + (offsets[i].y + y) * lpitch]) >> 16) & 0x000000FF;
					gSum += ((videoMemory[(offsets[i].x + x) + (offsets[i].y + y) * lpitch]) >> 8) & 0x000000FF;
					bSum += (videoMemory[(offsets[i].x + x) + (offsets[i].y + y) * lpitch]) & 0x000000FF;
				}
			}

			rSum /= numOffsets;
			gSum /= numOffsets;
			bSum /= numOffsets;

			if(rSum > 255) rSum = 255;
			if(gSum > 255) gSum = 255;
			if(bSum > 255) bSum = 255;

			videoMemory[x + y * lpitch] = ARGB32BIT(0,rSum, gSum, bSum);
		}
	}*/

	// TODO rewrite with intrinsics and plain C/C++
	/*__declspec(align(16)) unsigned int iMask[4] = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};

	int iMasSize = static_cast<int>((this->maxClipY - 2) * this->maxClipX);
	int iMasWidth = static_cast<int>(this->maxClipX);
	int iRedColor = 0x00FF0000;
	int *vb = (int *)videoMemory;
	int lpitch2 = lpitch;*/

	// TODO rewrite with intrinsics and plain C/C++
//	__asm{
//		pushad
//
//
//		mov eax, lpitch2 //Загружаем значение y + 1 для видеопамяти
//		mov ecx, iMasSize //Загружаем количество эл. в виделпамяти y - 1
//
//		mov edi, vb;//_videoBuffer //Загружаем начало видеопамяти
//		mov esi, iMasWidth //Загружаем кол. эл. по вертикали 
//
//		add edi, eax //К началу видеопамяти прибавляем y + 1
//		dec ecx //Уменшаем кол. эл. на x - 1
//
//		shl esi, 2 //Узнаем кол. эл. по вертикали в байтах
//		add edi, 4 //Смещаем на x + 1, т.к. цвет имеет размер 4 байта
//		
//		mov edx, edi //Копируем аддрес первого эл. в видеопамяти
//		sub esi, 12 //Уменшаем кол. эл. по вертикали так чтобы при прибавлении к x+1 получался конечный аддресс - (iWidth - 1)
//
//		add edx, esi //Получаем конечный аддрес по вертикали - (iWidth - 1)
//		push esi
//
//L1:
//		//main loop===============
//		mov esi, edi
//		mov ebx, edi
//
//		sub esi, 4
//		sub ebx, 4
//
//		sub esi, eax
//		add ebx, eax
//
//		movups xmm0, [esi]
//		movups xmm4, [ebx]
//
//		pslldq xmm0, 4
//		add esi, eax
//
//		pslldq xmm4, 4
//		sub ebx, eax
//
//		movd xmm1, [esi]
//		add ebx, 8//12
//
//		movd xmm5, [ebx]
//
//		por xmm0, xmm1
//		por xmm4, xmm5
//
//		//new alg
//
//		movaps xmm1, xmm0
//		movaps xmm5, xmm4
//
//		movaps xmm2, xmm0
//		movaps xmm6, xmm4
//
//		movaps xmm3, xmm0
//		movaps xmm7, xmm4
//
//		psrld xmm0, 24
//		psrld xmm4, 24
//
//		psrld xmm1, 16
//		psrld xmm5, 16
//
//		psrld xmm2, 8
//		psrld xmm6, 8
//
//		pand xmm0, iMask
//		pand xmm4, iMask
//
//		pand xmm1, iMask
//		pand xmm5, iMask
//
//		pand xmm2, iMask
//		pand xmm6, iMask
//
//		pand xmm3, iMask
//		pand xmm7, iMask
//
//		paddd xmm0, xmm4 //xmm0 4 alpha components
//		paddd xmm1, xmm5 //xmm1 4 red components
//
//		paddd xmm2, xmm6 //xmm2 4 green components
//		paddd xmm3, xmm7 //xmm3 4 blue components
//
//		movhlps xmm4, xmm0
//		movhlps xmm5, xmm1
//
//		movhlps xmm6, xmm2
//		movhlps xmm7, xmm3
//
//		paddd xmm0, xmm4 //xmm0 2 alpha components
//		paddd xmm1, xmm5 //xmm1 2 red components
//
//		paddd xmm2, xmm6 //xmm2 2 green components
//		paddd xmm3, xmm7 //xmm3 2 blue components
//
//		movaps xmm4, xmm0
//		movaps xmm5, xmm1
//
//		movaps xmm6, xmm2
//		movaps xmm7, xmm3
//
//		psrldq xmm4, 4
//		psrldq xmm5, 4
//
//		psrldq xmm6, 4
//		psrldq xmm7, 4
//
//		paddd xmm0, xmm4
//		paddd xmm1, xmm5
//
//		paddd xmm2, xmm6
//		psrld xmm0, 3
//
//		paddd xmm3, xmm7
//		psrld xmm1, 3
//
//		psrld xmm2, 3
//		pslld xmm0, 24
//
//		psrld xmm3, 3
//		pslld xmm1, 16
//
//		por xmm0, xmm3
//		pslld xmm2, 8
//
//		por xmm0, xmm1
//
//		por xmm0, xmm2
//
//		//new alg
//
//		movd [edi], xmm0
//
//		//main loop===============
//		cmp edi, edx
//		jnz L_no_next_line
//
//		pop esi
//		add edx, eax
//
//		sub edi, esi
//		add edi, eax
//
//		push esi
//		sub ecx, 3
//		
//		jc L_end
//		jnz L1
//		
//		jz L_end
//L_no_next_line:
//		
//		add edi, 4
//
//		dec ecx 
//		jnz L1
//L_end:
//		pop esi
//		popad
//	}

	return 1;
}

int Draw32BitStrategy::DrawTriangleTex(struct3D::POLYF4D_PTR face, unsigned int *videoMemory, int lpitch)
{
	if(!face) return 0;

	const int TRI_TYPE_NONE = 0;
	const int TRI_TYPE_FLAT_BOTTOM = 1;
	const int TRI_TYPE_FLAT_TOP = 2;
	const int TRI_TYPE_FLAT_MASK = 3;
	const int TRI_TYPE_GENERAL = 4;

	const int INTERP_LHS = 0;
	const int INTERP_RHS = 1;

	int v0 = 0, v1 = 1, v2 = 2, /*индексы вершин. ипользуються для того чтобы не менять местами вершины в массиве face->tvlist, а менять только индексы вершин*/
		temp = 0, /*переменная для обмена индексов вершин*/
		tri_type = TRI_TYPE_NONE, /*тип треугольника*/
		irestart = INTERP_LHS;
	float dx, dy, dyl, dyr;
	float tempf = 0;

	int xi, yi;
	/*int index_x, index_y;*/
	/*int x, y;*/
	int xstart, xend, ystart, yrestart, yend;
	float dxdyl, xr, xl, dxdyr;
	float x0, y0, tu0, tv0;
	float x1, y1, tu1, tv1;
	float x2, y2, tu2, tv2;
	
	float r_base0, g_base0, b_base0;
	float r_texel, g_texel, b_texel;

	/*math3D::VECTOR2D vecUV;*/
	math3D::VECTOR2D vecUVi;
	math3D::VECTOR2D vecDUV;
	math3D::VECTOR2D vecUVL;
	math3D::VECTOR2D vecUVR;
	math3D::VECTOR2D vecDUVDYL;
	math3D::VECTOR2D vecDUVDYR;

	UINT *screen_ptr = 0,
		*screen_line = 0,
		*textmap = (UINT *)face->texture->buffer,
		*dest_buffer = (UINT *) videoMemory;
	
	lpitch >>= 2;

	//TCHAR mas[128];
	//		swprintf(mas, L"%f %f", face->tvlist[0].x, face->tvlist[0].y);
	//		MessageBox(0,mas,0,0);

	if(((face->tvlist[0].y < this->minClipY) && (face->tvlist[1].y < this->minClipY) && (face->tvlist[2].y < this->minClipY)) ||
		(face->tvlist[0].y > this->maxClipY) && (face->tvlist[1].y > this->maxClipY) && (face->tvlist[2].y > this->maxClipY) ||
		(face->tvlist[0].x < this->minClipX) && (face->tvlist[1].x < this->minClipX) && (face->tvlist[2].x < this->minClipX) ||
		(face->tvlist[0].x > this->maxClipX) && (face->tvlist[1].x > this->maxClipX) && (face->tvlist[2].x > this->maxClipX))
		return 0; //Треугольник вне экрана

	if(((face->tvlist[0].x == face->tvlist[1].x) && (face->tvlist[1].x == face->tvlist[2].x)) ||
		((face->tvlist[0].y == face->tvlist[1].y) && (face->tvlist[1].y == face->tvlist[2].y)))
		return 0;

	/*сортируем индексы вершин в порядке v0.y <= v1.y <= v2.y, т.е. сверху вниз. положительная ось x направлена вправо, y - вниз*/
	if(face->tvlist[v1].y < face->tvlist[v0].y)
		SWAP(v0, v1, temp);

	if(face->tvlist[v2].y < face->tvlist[v0].y)
		SWAP(v0, v2, temp);

	if(face->tvlist[v2].y < face->tvlist[v1].y)
		SWAP(v1, v2, temp);

	if(face->tvlist[v0].y == face->tvlist[v1].y)
	{
		tri_type = TRI_TYPE_FLAT_TOP;

		//Сортируем вершины слева направо, т.е. v0 будет слева
		if(face->tvlist[v1].x < face->tvlist[v0].x)
			SWAP(v1, v0, temp);
	}
	else if(face->tvlist[v1].y == face->tvlist[v2].y)
	{
		tri_type = TRI_TYPE_FLAT_BOTTOM;

		//Сортируем вершины слева направо, т.е. v1 будет слева
		if(face->tvlist[v2].x < face->tvlist[v1].x)
			SWAP(v2, v1, temp);
	}
	else
	{
		tri_type = TRI_TYPE_GENERAL;
	}

	//Используеться 32 битный цвет

	RGBFROM32BIT(face->lit_color[0], r_base0, g_base0, b_base0);

	x0 = static_cast<float>((int)(face->tvlist[v0].x + 0.5f));
	y0 = static_cast<float>((int)(face->tvlist[v0].y + 0.5f));
	tu0 = face->tvlist[v0].u0;//(int)(face->tvlist[v0].u0 + 0.5f); 
	tv0 = face->tvlist[v0].v0;//(int)(face->tvlist[v0].v0 + 0.5f);

	x1 = static_cast<float>((int)(face->tvlist[v1].x + 0.5f));
	y1 = static_cast<float>((int)(face->tvlist[v1].y + 0.5f));
	tu1 = face->tvlist[v1].u0;//(int)(face->tvlist[v1].u0 + 0.5f); 
	tv1 = face->tvlist[v1].v0;//(int)(face->tvlist[v1].v0 + 0.5f);

	x2 = static_cast<float>((int)(face->tvlist[v2].x + 0.5f));
	y2 = static_cast<float>((int)(face->tvlist[v2].y + 0.5f));
	tu2 = face->tvlist[v2].u0;//(int)(face->tvlist[v2].u0 + 0.5f); 
	tv2 = face->tvlist[v2].v0;//(int)(face->tvlist[v2].v0 + 0.5f);

	yrestart = static_cast<int>(y1);

	if(tri_type & TRI_TYPE_FLAT_MASK)
	{
		if(tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = 1.0f / (y2 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x2 - x0) * dy;
			vecDUVDYL.x = (tu2 - tu0) * dy;
			vecDUVDYL.y = (tv2 - tv0) * dy;

			dxdyr = (x2 - x1) * dy;
			vecDUVDYR.x = (tu2 - tu1) * dy;
			vecDUVDYR.y = (tv2 - tv1) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				vecUVL.x = vecDUVDYL.x * dy + tu0;
				vecUVL.y = vecDUVDYL.y * dy + tv0;

				xr = dxdyr * dy + x1;
				vecUVR.x = vecDUVDYR.x * dy + tu1;
				vecUVR.y = vecDUVDYR.y * dy + tv1;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				vecUVL.x = tu0;
				vecUVL.y = tv0;

				xr = x1;
				vecUVR.x = tu1;
				vecUVR.y = tv1;

				ystart = static_cast<int>(y0);
			}
		}//if(tri_type == TRI_TYPE_FLAT_TOP)
		else //tri_type == TRI_TYPE_FLAT_BOTTOM
		{
			dy = 1.0f / (y1 - y0);//Используем обратную величину чтобы не использовать деление
			dxdyl = (x1 - x0) * dy;
			vecDUVDYL.x = (tu1 - tu0) * dy;
			vecDUVDYL.y = (tv1 - tv0) * dy;

			dxdyr = (x2 - x0) * dy;
			vecDUVDYR.x = (tu2 - tu0) * dy;
			vecDUVDYR.y = (tv2 - tv0) * dy;

			if(y0 < this->minClipY)
			{
				dy = this->minClipY - y0;

				xl = dxdyl * dy + x0;
				vecUVL.x = vecDUVDYL.x * dy + tu0;
				vecUVL.y = vecDUVDYL.y * dy + tv0;

				xr = dxdyr * dy + x0;
				vecUVR.x = vecDUVDYR.x * dy + tu0;
				vecUVR.y = vecDUVDYR.y * dy + tv0;

				ystart = static_cast<int>(this->minClipY);
			}
			else
			{
				xl = x0;
				vecUVL.x = tu0;
				vecUVL.y = tv0;

				xr = x0;
				vecUVR.x = tu0;
				vecUVR.y = tv0;

				ystart = static_cast<int>(y0);
			}
		}//else //tri_type == TRI_TYPE_FLAT_BOTTOM

		if((yend = static_cast<int>(y2)) > this->maxClipY)
		{
			yend = static_cast<int>(this->maxClipY);
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecUVi.x = vecUVL.x;//(ul + 0.5f);
				vecUVi.y = vecUVL.y;//(vl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					vecDUV.x = (vecUVR.x - vecUVL.x) * dx;
					vecDUV.y = (vecUVR.y - vecUVL.y) * dx;
				}
				else
				{
					vecDUV.x = vecUVR.x - vecUVL.x;
					vecDUV.y = vecUVR.y - vecUVL.y;
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					vecUVi.x += dx * vecDUV.x;
					vecUVi.y += dx * vecDUV.y;

					xstart = static_cast<int>(this->minClipX);
				}

				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					float ftx = static_cast<float>(fmod(vecUVi.x, 1) * face->texture->width);
					float fty = static_cast<float>(fmod(vecUVi.y, 1) * face->texture->height);

					RGBFROM32BIT(textmap[(int)ftx + (int)fty * face->texture->width], r_texel, g_texel, b_texel);

					r_texel *= r_base0;
					g_texel *= g_base0;
					b_texel *= b_base0;

					int rFinal = static_cast<int>(r_texel);
					int gFinal = static_cast<int>(g_texel);
					int bFinal = static_cast<int>(b_texel);

					rFinal >>= 8;
					gFinal >>= 8;
					bFinal >>= 8;

					screen_ptr[xi] = ARGB32BIT(255,rFinal,gFinal,bFinal);
					if(this->ppObjMap)
						this->ppObjMap[xi + yi * this->mapWidth] = face;

					vecUVi.x += vecDUV.x;
					vecUVi.y += vecDUV.y;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				vecUVL.x += vecDUVDYL.x;
				vecUVL.y += vecDUVDYL.y;

				xr += dxdyr;
				vecUVR.x += vecDUVDYR.x;
				vecUVR.y += vecDUVDYR.y;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}//if с отсечением по x
		else
		{

			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecUVi.x = vecUVL.x;//(ul + 0.5f);
				vecUVi.y = vecUVL.y;//(vl + 0.5f);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					vecDUV.x = (vecUVR.x - vecUVL.x) * dx;
					vecDUV.y = (vecUVR.y - vecUVL.y) * dx;
				}
				else
				{
					vecDUV.x = vecUVR.x - vecUVL.x;
					vecDUV.y = vecUVR.y - vecUVL.y;
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					float ftx = static_cast<float>(fmod(vecUVi.x, 1) * face->texture->width);
					float fty = static_cast<float>(fmod(vecUVi.y, 1) * face->texture->height);

					RGBFROM32BIT(textmap[(int)ftx + (int)fty * face->texture->width], r_texel, g_texel, b_texel);

					r_texel *= r_base0;
					g_texel *= g_base0;
					b_texel *= b_base0;

					int rFinal = static_cast<int>(r_texel);
					int gFinal = static_cast<int>(g_texel);
					int bFinal = static_cast<int>(b_texel);

					rFinal >>= 8;
					gFinal >>= 8;
					bFinal >>= 8;

					screen_ptr[xi] = ARGB32BIT(255,rFinal,gFinal,bFinal);
					if(this->ppObjMap)
						this->ppObjMap[xi + yi * this->mapWidth] = face;

					vecUVi.x += vecDUV.x;
					vecUVi.y += vecDUV.y;
				}//for(xi = xstart; xi <= xend; xi++)

				xl += dxdyl;
				vecUVL.x += vecDUVDYL.x;
				vecUVL.y += vecDUVDYL.y;

				xr += dxdyr;
				vecUVR.x += vecDUVDYR.x;
				vecUVR.y += vecDUVDYR.y;

				screen_ptr += lpitch;
			}//for(yi = ystart; yi <= yend; yi++)
		}
	}//if(tri_type & TRI_TYPE_FLAT_MASK)
	else if(tri_type == TRI_TYPE_GENERAL)
	{
		if((yend = static_cast<int>(y2)) > this->maxClipY)
			yend = static_cast<int>(this->maxClipY);

		if(y1 < this->minClipY)
		{
			
			dyl = 1.0f / (y2 - y1);
			dxdyl = (x2 - x1) * dyl;
			vecDUVDYL.x = (tu2 - tu1) * dyl;
			vecDUVDYL.y = (tv2 - tv1) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecDUVDYR.x = (tu2 - tu0) * dyr;
			vecDUVDYR.y = (tv2 - tv0) * dyr;

			dyr = (this->minClipY - y0);
			dyl = (this->minClipY - y1);

			xl = dxdyl * dyl + x1;
			vecUVL.x = vecDUVDYL.x * dyl + tu1;
			vecUVL.y = vecDUVDYL.y * dyl + tv1;

			xr = dxdyr * dyr + x0;
			vecUVR.x = vecDUVDYR.x * dyr + tu0;
			vecUVR.y = vecDUVDYR.y * dyr + tv0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);

				vecSwap(&vecDUVDYL, &vecDUVDYR);
				vecSwap(&vecUVL, &vecUVR);

				irestart = INTERP_RHS;
			}
		}
		else if(y0 < this->minClipY)
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			vecDUVDYL.x = (tu1 - tu0) * dyl;
			vecDUVDYL.y = (tv1 - tv0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecDUVDYR.x = (tu2 - tu0) * dyr;
			vecDUVDYR.y = (tv2 - tv0) * dyr;

			dy = (this->minClipY - y0);

			xl = dxdyl * dy + x0;
			vecUVL.x = vecDUVDYL.x * dy + tu0;
			vecUVL.y = vecDUVDYL.y * dy + tv0;

			xr = dxdyr * dy + x0;
			vecUVR.x = vecDUVDYR.x * dy + tu0;
			vecUVR.y = vecDUVDYR.y * dy + tv0;

			ystart = static_cast<int>(this->minClipY);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);

				vecSwap(&vecDUVDYL, &vecDUVDYR);
				vecSwap(&vecUVL, &vecUVR);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			dyl = 1.0f / (y1 - y0);
			dxdyl = (x1 - x0) * dyl;
			vecDUVDYL.x = (tu1 - tu0) * dyl;
			vecDUVDYL.y = (tv1 - tv0) * dyl;

			dyr = 1.0f / (y2 - y0);
			dxdyr = (x2 - x0) * dyr;
			vecDUVDYR.x = (tu2 - tu0) * dyr;
			vecDUVDYR.y = (tv2 - tv0) * dyr;

			xl = xr = x0;
			vecUVL.x = vecUVR.x = tu0;
			vecUVL.y = vecUVR.y = tv0;

			ystart = static_cast<int>(y0);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl,dxdyr,tempf);
				SWAP(xl,xr,tempf);
				SWAP(x1,x2,tempf);
				SWAP(y1,y2,tempf);
				SWAP(tu1,tu2,tempf);
				SWAP(tv1,tv2,tempf);

				vecSwap(&vecDUVDYL, &vecDUVDYR);
				vecSwap(&vecUVL, &vecUVR);

				irestart = INTERP_RHS;
			}
		}

		if((x0 < this->minClipX) || (x0 > this->maxClipX) || (x1 < this->minClipX) || (x1 > this->maxClipX) || (x2 < this->minClipX) || (x2 > this->maxClipX))
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecCopy(&vecUVL, &vecUVi);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					/*du = (ur - ul) * dx;
					dv = (vr - vl) * dx;*/
					vecSub(&vecUVR, &vecUVL, &vecDUV);
					vecScale(dx, &vecDUV);
				}
				else
				{
					vecSub(&vecUVR, &vecUVL, &vecDUV);
				}

				if(xstart < this->minClipX)
				{
					dx = this->minClipX - xstart;

					/*ui += du * dx;
					vi += dv * dx;*/

					vecScale(dx, &vecDUV);
					vecAdd(&vecUVi, &vecDUV, &vecUVi);

					xstart = static_cast<int>(this->minClipX);
				}
				if(xend > this->maxClipX)
					xend = static_cast<int>(this->maxClipX);

				for(xi = xstart; xi <= xend; xi++)
				{
					float ftx = static_cast<float>(fmod(vecUVi.x, 1) * face->texture->width);
					float fty = static_cast<float>(fmod(vecUVi.y, 1) * face->texture->height);

					RGBFROM32BIT(textmap[(int)ftx + (int)fty * face->texture->width], r_texel, g_texel, b_texel);

					r_texel *= r_base0;
					g_texel *= g_base0;
					b_texel *= b_base0;

					int rFinal = static_cast<int>(r_texel);
					int gFinal = static_cast<int>(g_texel);
					int bFinal = static_cast<int>(b_texel);

					rFinal >>= 8;
					gFinal >>= 8;
					bFinal >>= 8;

					screen_ptr[xi] = ARGB32BIT(255,rFinal,gFinal,bFinal);
					if(this->ppObjMap)
						this->ppObjMap[xi + yi * this->mapWidth] = face;

					/*ui += du;
					vi += dv;*/
					vecAdd(&vecUVi, &vecDUV, &vecUVi);
				}

				xl += dxdyl;
				/*ul += dudyl;
				vl += dvdyl;*/
				vecAdd(&vecUVL, &vecDUVDYL, &vecUVL);

				xr += dxdyr;
				/*ur += dudyr;
				vr += dvdyr;*/
				vecAdd(&vecUVR, &vecDUVDYR, &vecUVR);

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						vecDUVDYL.x = (tu2 - tu1) * dyl;
						vecDUVDYL.y = (tv2 - tv1) * dyl;

						xl = x1;
						vecUVL.x = tu1;
						vecUVL.y = tv1;

						xl += dxdyl;
						/*ul += dudyl;
						vl += dvdyl;*/
						vecAdd(&vecUVL, &vecDUVDYL, &vecUVL);
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						vecDUVDYR.x = (tu1 - tu2) * dyr;
						vecDUVDYR.y = (tv1 - tv2) * dyr;

						xr = x2;
						vecUVR.x = tu2;
						vecUVR.y = tv2;

						xr += dxdyr;
						/*ur += dudyr;
						vr += dvdyr;*/
						vecAdd(&vecUVR, &vecDUVDYR, &vecUVR);
					}
				}
			}
		}
		else
		{
			screen_ptr = dest_buffer + (ystart * lpitch);

			for(yi = ystart; yi <= yend; yi++)
			{
				xstart = static_cast<int>(xl + 0.5f);
				xend = static_cast<int>(xr + 0.5f);

				vecCopy(&vecUVL, &vecUVi);

				if((dx = static_cast<float>(xend - xstart)) > 0)
				{
					dx = 1.0f / dx;

					vecSub(&vecUVR, &vecUVL, &vecDUV);
					vecScale(dx, &vecDUV);
				}
				else
				{
					vecSub(&vecUVR, &vecUVL, &vecDUV);
				}

				for(xi = xstart; xi <= xend; xi++)
				{
					float ftx = static_cast<float>(fmod(vecUVi.x, 1) * face->texture->width);
					float fty = static_cast<float>(fmod(vecUVi.y, 1) * face->texture->height);

					RGBFROM32BIT(textmap[(int)ftx + (int)fty * face->texture->width], r_texel, g_texel, b_texel);

					r_texel *= r_base0;
					g_texel *= g_base0;
					b_texel *= b_base0;

					int rFinal = static_cast<int>(r_texel);
					int gFinal = static_cast<int>(g_texel);
					int bFinal = static_cast<int>(b_texel);

					rFinal >>= 8;
					gFinal >>= 8;
					bFinal >>= 8;

					screen_ptr[xi] = ARGB32BIT(255,rFinal,gFinal,bFinal);

					if(this->ppObjMap)
						this->ppObjMap[xi + yi * this->mapWidth] = face;
					//screen_ptr[xi] = 255;

					vecAdd(&vecUVi, &vecDUV, &vecUVi);
				}

				xl += dxdyl;
				vecAdd(&vecUVL, &vecDUVDYL, &vecUVL);

				xr += dxdyr;
				vecAdd(&vecUVR, &vecDUVDYR, &vecUVR);

				screen_ptr += lpitch;

				if(yi == yrestart)
				{
					if(irestart == INTERP_LHS)
					{
						dyl = 1.0f / (y2 - y1);

						dxdyl = (x2 - x1) * dyl;
						vecDUVDYL.x = (tu2 - tu1) * dyl;
						vecDUVDYL.y = (tv2 - tv1) * dyl;

						xl = x1;
						vecUVL.x = tu1;
						vecUVL.y = tv1;

						xl += dxdyl;
						vecAdd(&vecUVL, &vecDUVDYL, &vecUVL);
					}
					else //INTERP_RHS
					{
						dyr = 1.0f / (y1 - y2);

						dxdyr = (x1 - x2) * dyr;
						vecDUVDYR.x = (tu1 - tu2) * dyr;
						vecDUVDYR.y = (tv1 - tv2) * dyr;

						xr = x2;
						vecUVR.x = tu2;
						vecUVR.y = tv2;

						xr += dxdyr;
						vecAdd(&vecUVR, &vecDUVDYR, &vecUVR);
					}
				}
			}
		}
	}

	return 1;
}

int Draw32BitStrategy::setObj3DMap(struct3D::POLYF4D_PTR *ppMap, unsigned int width, unsigned int height)
{
	if(!ppMap) return 0;

	this->ppObjMap = ppMap;
	this->mapWidth = width;
	this->mapHeight = height;

	return 1;
}

int Draw32BitStrategy::DrawRect(RECT *r, unsigned int color, void *videoMemory, int lpitch)
{
	int temp = 0;
	int lpitch2 = lpitch >> 2;

	if(r->left > r->right) SWAP(r->left, r->right, temp);
	if(r->top > r->bottom) SWAP(r->top, r->bottom, temp);

	if(r->left > this->maxClipX || r->right < this->minClipX || r->top > this->maxClipY || r->bottom < this->minClipY)
		return 0;

	if(r->left < this->minClipX) r->left = static_cast<LONG>(this->minClipX);
	if(r->top < this->minClipY) r->top = static_cast<LONG>(this->minClipY);

	if(r->bottom > this->maxClipY) r->bottom = static_cast<LONG>(this->maxClipY);
	if(r->right > this->maxClipX) r->right = static_cast<LONG>(this->maxClipX);



	int width = r->right - r->left;
	int height = r->bottom - r->top;
	unsigned int *vb = (unsigned int *)videoMemory + r->left + (r->top * lpitch2);

	if((color >> 24) > 250)
	{
		for(int i = 0; i < height; i++)
		{
			std::fill(vb, vb + width, color);
			vb += lpitch2;
		}
	}
	else
	{
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				this->_alphaBlender->AlphaBlend(&vb[j + i * lpitch2], &color, 1);
			}
		}
	}

	return 1;
}

int Draw32BitStrategy::clearZBuffer()
{
	this->zbuffer.clear();

	return 1;
}

int Draw32BitStrategy::clipLineCS(RECT *clippingRect, POINT4D *pt1, POINT4D *pt2)
{
	const int CLIP_CODE_C = 0x0000;
	const int CLIP_CODE_N = 0x0008;
	const int CLIP_CODE_S = 0x0004;
	const int CLIP_CODE_E = 0x0002;
	const int CLIP_CODE_W = 0x0001;

	const int CLIP_CODE_NE = 0x000A;
	const int CLIP_CODE_SE = 0x0006;
	const int CLIP_CODE_NW = 0x0009;
	const int CLIP_CODE_SW = 0x0005;

	float minX = static_cast<float>(clippingRect->left);
	float minY = static_cast<float>(clippingRect->top);

	float maxX = static_cast<float>(clippingRect->right);
	float maxY = static_cast<float>(clippingRect->bottom);

	float t = 0;

	POINT4D ptc1;
	POINT4D ptc2;

	vecZero(&ptc1);
	vecZero(&ptc2);

	int pcode1 = 0, pcode2 = 0;

	if(pt1->x < minX) pcode1 |= CLIP_CODE_W;
	else if(pt1->x > maxX) pcode1 |= CLIP_CODE_E;

	if(pt1->y < minY) pcode1 |= CLIP_CODE_N;
	else if(pt1->y > maxY) pcode1 |= CLIP_CODE_S;

	if(pt2->x < minX) pcode2 |= CLIP_CODE_W;
	else if(pt2->x > maxX) pcode2 |= CLIP_CODE_E;

	if(pt2->y < minY) pcode2 |= CLIP_CODE_N;
	else if(pt2->y > maxY) pcode2 |= CLIP_CODE_S;

	if((pcode1 & pcode2)) return 0;
	
	if(pcode1 == 0 && pcode2 == 0) return 1;

	switch(pcode1)
	{
	case CLIP_CODE_C: 
		{
			/*ptc1.x = pt1->x;
			ptc1.y = pt1->y;
			ptc1.z = pt1->z;*/
			vecCopy(pt1, &ptc1);
		}break;
	case CLIP_CODE_N:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
		}break;
	case CLIP_CODE_S:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
		}break;
	case CLIP_CODE_E:
		{
			t = (maxX - pt1->x) / (pt2->x - pt1->x);
			ptc1.x = maxX;
			ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
		}break;
	case CLIP_CODE_W:
		{
			t = (minX - pt1->x) / (pt2->x - pt1->x);
			ptc1.x = minX;
			ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
		}break;
	case CLIP_CODE_NE:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (maxX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = maxX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
				ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			}
		}break;
	case CLIP_CODE_SE:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (maxX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = maxX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
				ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			}
		}break;
	case CLIP_CODE_NW:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (minX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = minX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
				ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			}
		}break;
	case CLIP_CODE_SW:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (minX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = minX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
				ptc1.z = pt1->z + (pt2->z - pt1->z) * t;
			}
		}break;
	default: break;
	}

	switch(pcode2)
	{
	case CLIP_CODE_C: 
		{
			/*ptc2.x = pt2->x;
			ptc2.y = pt2->y;
			ptc2.z = pt2->z;*/
			vecCopy(pt2, &ptc2);
		}break;
	case CLIP_CODE_N:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
		}break;
	case CLIP_CODE_S:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
		}break;
	case CLIP_CODE_E:
		{
			t = (maxX - pt2->x) / (pt1->x - pt2->x);
			ptc2.x = maxX;
			ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
		}break;
	case CLIP_CODE_W:
		{
			t = (minX - pt2->x) / (pt1->x - pt2->x);
			ptc2.x = minX;
			ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
		}break;
	case CLIP_CODE_NE:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (maxX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = maxX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
				ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			}
		}break;
	case CLIP_CODE_SE:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (maxX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = maxX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
				ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			}
		}break;
	case CLIP_CODE_NW:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (minX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = minX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
				ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			}
		}break;
	case CLIP_CODE_SW:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (minX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = minX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
				ptc2.z = pt2->z + (pt1->z - pt2->z) * t;
			}
		}break;
	default: break;
	}

	if((ptc1.x < minX) || (ptc1.x > maxX) || (ptc1.y  < minY) || (ptc1.y > maxY) || (ptc2.x  < minX) || (ptc2.x > maxX) || (ptc2.y < minY) || (ptc2.y > maxY)) 
	{
		return 0;
	}

	vecCopy(&ptc1, pt1);
	vecCopy(&ptc2, pt2);

	return 1;
}

int Draw32BitStrategy::DrawTriangle4(struct3D::POLYF4D_PTR poly, unsigned int *videoMemory, int lpitch)
{

	int v0 = 0, v1 = 1, v2 = 2, tmp;
	UINT lp = lpitch >> 2;
	UINT *vb;

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

	//сортировка сверху по Х против часовой стрелки
	if(poly->tvlist[v1].y == poly->tvlist[v0].y)//FLAT_TOP
	{
		if(poly->tvlist[v1].x > poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);
	}
	else if(poly->tvlist[v1].y == poly->tvlist[v2].y)//FLAT_BOTTOM
	{
		if(poly->tvlist[v1].x > poly->tvlist[v2].x)
			SWAP(v1, v2, tmp);
	}
	else
	{
		float dxdyr = (poly->tvlist[v1].x - poly->tvlist[v0].x) / (poly->tvlist[v1].y - poly->tvlist[v0].y);
		float dxdyl = (poly->tvlist[v2].x - poly->tvlist[v0].x) / (poly->tvlist[v2].y - poly->tvlist[v0].y);

		if(dxdyr > dxdyl)//RHS
		{
			SWAP(v1, v2, tmp);
		}
	}

	// 28.4 fixed-point coordinates
	#define fixpres 4
	#define pres 4
	#define corr (1 << fixpres) - 1

	// 28.4 fixed-point coordinates
	const int no_edge12 = 0x1;
	const int no_edge23 = 0x2;
	const int no_edge31 = 0x4;
	const int no_edge_all = 0x7;
	int edge_code = 0;

	int Y1, Y2, Y3, Y4 ,Y5 ,Y6;
	int X1, X2, X3, X4, X5, X6;

	int DX12, DX23, DX31;
	int DY12, DY23, DY31;

	int FDX12, FDX23, FDX31;
	int FDY12, FDY23, FDY31;

	POINT4D p1, p2;

	vecCopy(&poly->tvlist[v0].v, &p1);
	vecCopy(&poly->tvlist[v1].v, &p2);

	if(!this->clipLineCS(&this->clipRt, &p1, &p2))
	{
		edge_code |= no_edge12;
	}
	else
	{
		X1 = int(16.0f * p1.x);
		Y1 = int(16.0f * p1.y);

		X2 = int(16.0f * p2.x);
		Y2 = int(16.0f * p2.y);

		DX12 = X1 - X2;
		DY12 = Y1 - Y2;

		FDX12 = DX12 << fixpres;
		FDY12 = DY12 << fixpres;
	}

	vecCopy(&poly->tvlist[v1].v, &p1);
	vecCopy(&poly->tvlist[v2].v, &p2);

	if(!this->clipLineCS(&this->clipRt, &p1, &p2))
	{
		edge_code |= no_edge23;
	}
	else
	{
		X3 = int(16.0f * p1.x);
		Y3 = int(16.0f * p1.y);

		X4 = int(16.0f * p2.x);
		Y4 = int(16.0f * p2.y);

		DX23 = X3 - X4;
		DY23 = Y3 - Y4;

		FDX23 = DX23 << fixpres;
		FDY23 = DY23 << fixpres;
	}

	vecCopy(&poly->tvlist[v2].v, &p1);
	vecCopy(&poly->tvlist[v0].v, &p2);

	if(!this->clipLineCS(&this->clipRt, &p1, &p2))
	{
		edge_code |= no_edge31;
	}
	else
	{
		X5 = int(16.0f * p1.x);
		Y5 = int(16.0f * p1.y);

		X6 = int(16.0f * p2.x);
		Y6 = int(16.0f * p2.y);

		DX31 = X5 - X6;
		DY31 = Y5 - Y6;

		FDX31 = DX31 << fixpres;
		FDY31 = DY31 << fixpres;
	}

    int YO1 = int(16.0f * poly->tvlist[v0].y);
    int YO2 = int(16.0f * poly->tvlist[v1].y);
    int YO3 = int(16.0f * poly->tvlist[v2].y);

    int XO1 = int(16.0f * poly->tvlist[v0].x);
    int XO2 = int(16.0f * poly->tvlist[v1].x);
    int XO3 = int(16.0f * poly->tvlist[v2].x);

    // Bounding rectangle
    int minx = (min(XO1, min(XO2, XO3)) + corr) >> fixpres;
    int maxx = (max(XO1, max(XO2, XO3)) + corr) >> fixpres;
    int miny = (min(YO1, min(YO2, YO3)) + corr) >> fixpres;
    int maxy = (max(YO1, max(YO2, YO3)) + corr) >> fixpres;

	minx = max(minx, (int)this->minClipX);
	miny = max(miny, (int)this->minClipY);

	maxx = min(maxx, (int)this->maxClipX);
	maxy = min(maxy, (int)this->maxClipY);

    vb = videoMemory + miny * lp;

    // Half-edge constants
	int C1, C2, C3;
	int CY1, CY2, CY3;

	if(edge_code & no_edge12)
	{
		CY1 = 1;
		FDX12 = FDY12 = 0;
	}
	else
	{
		C1 = DY12 * X1 - DX12 * Y1;
		if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
		CY1 = C1 + DX12 * (miny << fixpres) - DY12 * (minx << fixpres);
	}

	if(edge_code & no_edge23)
	{
		CY2 = 1;
		FDX23 = FDY23 = 0;
	}
	else
	{
		C2 = DY23 * X3 - DX23 * Y3;
		if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
		CY2 = C2 + DX23 * (miny << fixpres) - DY23 * (minx << fixpres);
	}

	if(edge_code & no_edge31)
	{
		CY3 = 1;
		FDX31 = FDY31 = 0;
	}
	else
	{
		C3 = DY31 * X5 - DX31 * Y5;
		if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;
		CY3 = C3 + DX31 * (miny << fixpres) - DY31 * (minx << fixpres);
	}

    for(int y = miny; y < maxy; y++)
    {
        int CX1 = CY1;
        int CX2 = CY2;
        int CX3 = CY3;

        for(int x = minx; x < maxx; x++)
        {
            if(CX1 > 0 && CX2 > 0 && CX3 > 0)
            {
                UINT color = ARGB32BIT(127,0,0,0);
   		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1); 
            }

            CX1 -= FDY12;
            CX2 -= FDY23;
            CX3 -= FDY31;
        }

        CY1 += FDX12;
        CY2 += FDX23;
        CY3 += FDX31;

        vb += lp;
    }

	return 1;
}

inline int iround(float x)
{
	// original code
	// std::round is very close to fistp instruction but there are some differences in rounding near 0.5
	//__asm
	//{
	//  fld  x
	//  fistp t
	//}
	return static_cast<int>(std::round(x));
}

int Draw32BitStrategy::DrawTriangle5(struct3D::POLYF4D_PTR poly, unsigned int *videoMemory, int lpitch)
{
	int v0 = 0, v1 = 1, v2 = 2, tmp;
	UINT lp = lpitch >> 2;
	UINT *vb;

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

	//сортировка сверху по Х против часовой стрелки
	if(poly->tvlist[v1].y == poly->tvlist[v0].y)//FLAT_TOP
	{
		if(poly->tvlist[v1].x > poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);
	}
	else if(poly->tvlist[v1].y == poly->tvlist[v2].y)//FLAT_BOTTOM
	{
		if(poly->tvlist[v1].x > poly->tvlist[v2].x)
			SWAP(v1, v2, tmp);
	}
	else
	{
		float dxdyr = (poly->tvlist[v1].x - poly->tvlist[v0].x) / (poly->tvlist[v1].y - poly->tvlist[v0].y);
		float dxdyl = (poly->tvlist[v2].x - poly->tvlist[v0].x) / (poly->tvlist[v2].y - poly->tvlist[v0].y);

		if(dxdyr > dxdyl)//RHS
		{
			SWAP(v1, v2, tmp);
		}
	}

	const __int64 fpres = 32;
	const __int64 fcorr = ((__int64)1 << fpres) - (__int64)1;

	//// 28.4 fixed-point coordinates
	//fixp64 Y1, Y2, Y3;
	//fixp64 X1, X2, X3;

 //   Y1 = poly->tvlist[v0].y;
 //   Y2 = poly->tvlist[v1].y;
 //   Y3 = poly->tvlist[v2].y;

 //   X1 = poly->tvlist[v0].x;
 //   X2 = poly->tvlist[v1].x;
 //   X3 = poly->tvlist[v2].x;

 //   // Deltas
	//fixp64 DY12, DY23, DY31;
	//fixp64 DX12, DX23, DX31;

 //   DX12 = X1 - X2;
 //   DX23 = X2 - X3;
 //   DX31 = X3 - X1;

 //   DY12 = Y1 - Y2;
 //   DY23 = Y2 - Y3;
 //   DY31 = Y3 - Y1;

 //   // Bounding rectangle
	//int minx = (min(X1.value, min(X2.value, X3.value)) + fcorr) >> fpres;
	//int maxx = (max(X1.value, max(X2.value, X3.value)) + fcorr) >> fpres;
	//int miny = (min(Y1.value, min(Y2.value, Y3.value)) + fcorr) >> fpres;
	//int maxy = (max(Y1.value, max(Y2.value, Y3.value)) + fcorr) >> fpres;

	//minx = max(minx, (int)this->minClipX);
	//miny = max(miny, (int)this->minClipY);

	//maxx = min(maxx, (int)this->maxClipX);
	//maxy = min(maxy, (int)this->maxClipY);

 //   vb = videoMemory + miny * lp;

 //   // Half-edge constants
	//fixp64 C1, C2, C3;
	//fixp64 CY1, CY2, CY3;

 //   C1 = DY12 * X1 - DX12 * Y1;
 //   C2 = DY23 * X2 - DX23 * Y2;
 //   C3 = DY31 * X3 - DX31 * Y3;

 //   // Correct for fill convention
	//if(DY12.value < 0 || (DY12.value == 0 && DX12.value > 0)) C1.value++;//C1+=fixed_point<fpres>(1);
	//if(DY23.value < 0 || (DY23.value == 0 && DX23.value > 0)) C2.value++;//C2+=fixed_point<fpres>(1);
	//if(DY31.value < 0 || (DY31.value == 0 && DX31.value > 0)) C3.value++;//C3+=fixed_point<fpres>(1);

 //   CY1 = C1 + DX12 * miny - DY12 * minx;
 //   CY2 = C2 + DX23 * miny - DY23 * minx;
 //   CY3 = C3 + DX31 * miny - DY31 * minx;

 //   for(int y = miny; y < maxy; y++)
 //   {
 //       fixp64 CX1 = CY1;
 //       fixp64 CX2 = CY2;
 //       fixp64 CX3 = CY3;

 //       for(int x = minx; x < maxx; x++)
 //       {
	//		if(CX1.value > 0 && CX2.value > 0 && CX3.value > 0)
 //           {
 //               UINT color = ARGB32BIT(127,0,0,0);
 //  		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
 //           }

 //           CX1 -= DY12;
 //           CX2 -= DY23;
 //           CX3 -= DY31;
 //       }

 //       CY1 += DX12;
 //       CY2 += DX23;
 //       CY3 += DX31;

 //       vb += lp;
 //   }


	// 48.16 fixed-point coordinates
    const __int64 Y1 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v0].y);
    const __int64 Y2 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v1].y);
    const __int64 Y3 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v2].y);

    const __int64 X1 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v0].x);
    const __int64 X2 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v1].x);
    const __int64 X3 = static_cast<__int64>(65536.0f * (double)poly->tvlist[v2].x);

    // Deltas
    const __int64 DX12 = X1 - X2;
    const __int64 DX23 = X2 - X3;
    const __int64 DX31 = X3 - X1;

    const __int64 DY12 = Y1 - Y2;
    const __int64 DY23 = Y2 - Y3;
    const __int64 DY31 = Y3 - Y1;

    // Fixed-point deltas. extending to 32.32
    const __int64 FDX12 = DX12 << 16;
    const __int64 FDX23 = DX23 << 16;
    const __int64 FDX31 = DX31 << 16;

    const __int64 FDY12 = DY12 << 16;
    const __int64 FDY23 = DY23 << 16;
    const __int64 FDY31 = DY31 << 16;

    // Bounding rectangle
    int minx = static_cast<int>((min(X1, min(X2, X3)) + 0xFFFF) >> 16);
    int maxx = static_cast<int>((max(X1, max(X2, X3)) + 0xFFFF) >> 16);
    int miny = static_cast<int>((min(Y1, min(Y2, Y3)) + 0xFFFF) >> 16);
    int maxy = static_cast<int>((max(Y1, max(Y2, Y3)) + 0xFFFF) >> 16);

	minx = max(minx, (int)this->minClipX);
	miny = max(miny, (int)this->minClipY);

	maxx = min(maxx, (int)this->maxClipX);
	maxy = min(maxy, (int)this->maxClipY);

    vb = videoMemory + miny * lp;

    // Half-edge constants in 32.32 fixp
    __int64 C1 = DY12 * X1 - DX12 * Y1;
    __int64 C2 = DY23 * X2 - DX23 * Y2;
    __int64 C3 = DY31 * X3 - DX31 * Y3;

    // Correct for fill convention
    if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
    if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	//DX# in 48.16! After calculations CY# is in 32.32 fixp
    __int64 CY1 = C1 + DX12 * (miny << 16) - DY12 * (minx << 16);
    __int64 CY2 = C2 + DX23 * (miny << 16) - DY23 * (minx << 16);
    __int64 CY3 = C3 + DX31 * (miny << 16) - DY31 * (minx << 16);

    for(int y = miny; y < maxy; y++)
    {
        __int64 CX1 = CY1;
        __int64 CX2 = CY2;
        __int64 CX3 = CY3;

        for(int x = minx; x < maxx; x++)
        {
            if(CX1 > 0 && CX2 > 0 && CX3 > 0)
            {

				/*if(vb[x] != ARGB32BIT(0,255,255,255))
				{
					MessageBox(0,L"ERROR",0,0);
				}*/

                UINT color = ARGB32BIT(127,0,0,0);
   		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
            }

            CX1 -= FDY12;
            CX2 -= FDY23;
            CX3 -= FDY31;
        }

        CY1 += FDX12;
        CY2 += FDX23;
        CY3 += FDX31;

        vb += lp;
    }


 //    // 28.4 fixed-point coordinates
 //   const int Y1 = iround(16.0f * poly->tvlist[v0].y);
 //   const int Y2 = iround(16.0f * poly->tvlist[v1].y);
 //   const int Y3 = iround(16.0f * poly->tvlist[v2].y);

 //   const int X1 = iround(16.0f * poly->tvlist[v0].x);
 //   const int X2 = iround(16.0f * poly->tvlist[v1].x);
 //   const int X3 = iround(16.0f * poly->tvlist[v2].x);

 //   // Deltas
 //   const int DX12 = X1 - X2;
 //   const int DX23 = X2 - X3;
 //   const int DX31 = X3 - X1;

 //   const int DY12 = Y1 - Y2;
 //   const int DY23 = Y2 - Y3;
 //   const int DY31 = Y3 - Y1;

 //   // Fixed-point deltas
 //   const int FDX12 = DX12 << 4;
 //   const int FDX23 = DX23 << 4;
 //   const int FDX31 = DX31 << 4;

 //   const int FDY12 = DY12 << 4;
 //   const int FDY23 = DY23 << 4;
 //   const int FDY31 = DY31 << 4;

 //   // Bounding rectangle
 //   int minx = (min(X1, min(X2, X3)) + 0xF) >> 4;
 //   int maxx = (max(X1, max(X2, X3)) + 0xF) >> 4;
 //   int miny = (min(Y1, min(Y2, Y3)) + 0xF) >> 4;
 //   int maxy = (max(Y1, max(Y2, Y3)) + 0xF) >> 4;

	//minx = max(minx, (int)this->minClipX);
	//miny = max(miny, (int)this->minClipY);

	//maxx = min(maxx, (int)this->maxClipX);
	//maxy = min(maxy, (int)this->maxClipY);

 //   vb = videoMemory + miny * lp;

 //   // Half-edge constants
 //   int C1 = DY12 * X1 - DX12 * Y1;
 //   int C2 = DY23 * X2 - DX23 * Y2;
 //   int C3 = DY31 * X3 - DX31 * Y3;

 //   // Correct for fill convention
 //   if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
 //   if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
 //   if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

 //   int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
 //   int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
 //   int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

 //   for(int y = miny; y < maxy; y++)
 //   {
 //       int CX1 = CY1;
 //       int CX2 = CY2;
 //       int CX3 = CY3;

 //       for(int x = minx; x < maxx; x++)
 //       {
 //           if(CX1 > 0 && CX2 > 0 && CX3 > 0)
 //           {
 //               UINT color = ARGB32BIT(127,0,0,0);
 //  		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
 //           }

 //           CX1 -= FDY12;
 //           CX2 -= FDY23;
 //           CX3 -= FDY31;
 //       }

 //       CY1 += FDX12;
 //       CY2 += FDX23;
 //       CY3 += FDX31;

 //       vb += lp;
 //   }

	return 1;
}

int Draw32BitStrategy::DrawTriangle6(struct3D::POLYF4D_PTR poly, unsigned int *videoMemory, int lpitch)
{
	int v0 = 0, v1 = 1, v2 = 2, tmp;
	UINT lp = lpitch >> 2;
	UINT *vb;

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

	//сортировка сверху по Х против часовой стрелки
	if(poly->tvlist[v1].y == poly->tvlist[v0].y)//FLAT_TOP
	{
		if(poly->tvlist[v1].x > poly->tvlist[v0].x)
			SWAP(v1, v0, tmp);
	}
	else if(poly->tvlist[v1].y == poly->tvlist[v2].y)//FLAT_BOTTOM
	{
		if(poly->tvlist[v1].x > poly->tvlist[v2].x)
			SWAP(v1, v2, tmp);
	}
	else
	{
		float dxdyr = (poly->tvlist[v1].x - poly->tvlist[v0].x) / (poly->tvlist[v1].y - poly->tvlist[v0].y);
		float dxdyl = (poly->tvlist[v2].x - poly->tvlist[v0].x) / (poly->tvlist[v2].y - poly->tvlist[v0].y);

		if(dxdyr > dxdyl)//RHS
		{
			SWAP(v1, v2, tmp);
		}
	}

		// 48.16 fixed-point coordinates
    /*const __int64 Y1 = (65536.0f * (double)poly->tvlist[v0].y);
    const __int64 Y2 = (65536.0f * (double)poly->tvlist[v1].y);
    const __int64 Y3 = (65536.0f * (double)poly->tvlist[v2].y);

    const __int64 X1 = (65536.0f * (double)poly->tvlist[v0].x);
    const __int64 X2 = (65536.0f * (double)poly->tvlist[v1].x);
    const __int64 X3 = (65536.0f * (double)poly->tvlist[v2].x);*/
	const float flt_fix = 65536.0f;
	float fx[3] = {poly->tvlist[v0].x, poly->tvlist[v1].x, poly->tvlist[v2].x};
	float fy[3] = {poly->tvlist[v0].y, poly->tvlist[v1].y, poly->tvlist[v2].y};
	__declspec(align(16)) __int64 X123[3], Y123[3]/*, X3Y3[2]*//*x3 - low part y3 - high part*//*, X1Y1[2]*//*x1 - low part y1 - high part*/;

	X123[0] = __int64(fx[0] * flt_fix);
	X123[1] = __int64(fx[1] * flt_fix);
	X123[2] = __int64(fx[2] * flt_fix);

	Y123[0] = __int64(fy[0] * flt_fix);
	Y123[1] = __int64(fy[1] * flt_fix);
	Y123[2] = __int64(fy[2] * flt_fix);

    // Deltas
    /*const __int64 DX12 = X1 - X2;
    const __int64 DX23 = X2 - X3;
    const __int64 DX31 = X3 - X1;

    const __int64 DY12 = Y1 - Y2;
    const __int64 DY23 = Y2 - Y3;
    const __int64 DY31 = Y3 - Y1;*/
	const __int64 DX12 = X123[0] - X123[1];
    const __int64 DX23 = X123[1] - X123[2];
    const __int64 DX31 = X123[2] - X123[0];

    const __int64 DY12 = Y123[0] - Y123[1];
    const __int64 DY23 = Y123[1] - Y123[2];
    const __int64 DY31 = Y123[2] - Y123[0];

    // Fixed-point deltas. extending to 32.32
    const __int64 FDX12 = DX12 << 16;
    const __int64 FDX23 = DX23 << 16;
    const __int64 FDX31 = DX31 << 16;

    const __int64 FDY12 = DY12 << 16;
    const __int64 FDY23 = DY23 << 16;
    const __int64 FDY31 = DY31 << 16;

    // Bounding rectangle
    int minx = static_cast<int>((min(X123[0], min(X123[1], X123[2])) + 0xFFFF) >> 16);
    int maxx = static_cast<int>((max(X123[0], max(X123[1], X123[2])) + 0xFFFF) >> 16);
    int miny = static_cast<int>((min(Y123[0], min(Y123[1], Y123[2])) + 0xFFFF) >> 16);
    int maxy = static_cast<int>((max(Y123[0], max(Y123[1], Y123[2])) + 0xFFFF) >> 16);

	minx = max(minx, (int)this->minClipX);
	miny = max(miny, (int)this->minClipY);

	maxx = min(maxx, (int)this->maxClipX);
	maxy = min(maxy, (int)this->maxClipY);

    vb = videoMemory + miny * lp;

    // Half-edge constants in 32.32 fixp
    /*__int64 C1 = DY12 * X1 - DX12 * Y1;
    __int64 C2 = DY23 * X2 - DX23 * Y2;
    __int64 C3 = DY31 * X3 - DX31 * Y3;*/
	__int64 C1 = DY12 * X123[0] - DX12 * Y123[0];
    __int64 C2 = DY23 * X123[1] - DX23 * Y123[1];
    __int64 C3 = DY31 * X123[2] - DX31 * Y123[2];

    // Correct for fill convention
    if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
    if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

	//DX# in 48.16! After calculations CY# is in 32.32 fixp
    __int64 CY1 = C1 + DX12 * (miny << 16) - DY12 * (minx << 16);
    __int64 CY2 = C2 + DX23 * (miny << 16) - DY23 * (minx << 16);
    __int64 CY3 = C3 + DX31 * (miny << 16) - DY31 * (minx << 16);

    for(int y = miny; y < maxy; y++)
    {
        __int64 CX1 = CY1;
        __int64 CX2 = CY2;
        __int64 CX3 = CY3;

        for(int x = minx; x < maxx; x++)
        {
            if(CX1 > 0 && CX2 > 0 && CX3 > 0)
			{
                UINT color = ARGB32BIT(127,0,0,0);
   		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
            }

            CX1 -= FDY12;
            CX2 -= FDY23;
            CX3 -= FDY31;
        }

        CY1 += FDX12;
        CY2 += FDX23;
        CY3 += FDX31;

        vb += lp;
    }

	return 1;
}

// TODO rewrite with intrinsics and plain C/C++
int Draw32BitStrategy::DrawTriangle7_sse(struct3D::POLYF4D_PTR poly, unsigned int *videoMemory, int lpitch)
{
	//int v0 = 0, v1 = 1, v2 = 2, tmp;
	//UINT lp = lpitch >> 2;
	//UINT *vb;

	////сортировка сверху вниз
	//if(poly->tvlist[v1].y < poly->tvlist[v0].y)
	//{
	//	SWAP(v1, v0, tmp);
	//}
	//if(poly->tvlist[v2].y < poly->tvlist[v0].y)
	//{
	//	SWAP(v2, v0, tmp);
	//}
	//if(poly->tvlist[v2].y < poly->tvlist[v1].y)
	//{
	//	SWAP(v2, v1, tmp);
	//}

	////сортировка сверху по Х против часовой стрелки
	//if(poly->tvlist[v1].y == poly->tvlist[v0].y)//FLAT_TOP
	//{
	//	if(poly->tvlist[v1].x > poly->tvlist[v0].x)
	//		SWAP(v1, v0, tmp);
	//}
	//else if(poly->tvlist[v1].y == poly->tvlist[v2].y)//FLAT_BOTTOM
	//{
	//	if(poly->tvlist[v1].x > poly->tvlist[v2].x)
	//		SWAP(v1, v2, tmp);
	//}
	//else
	//{
	//	float dxdyr = (poly->tvlist[v1].x - poly->tvlist[v0].x) / (poly->tvlist[v1].y - poly->tvlist[v0].y);
	//	float dxdyl = (poly->tvlist[v2].x - poly->tvlist[v0].x) / (poly->tvlist[v2].y - poly->tvlist[v0].y);

	//	if(dxdyr > dxdyl)//RHS
	//	{
	//		SWAP(v1, v2, tmp);
	//	}
	//}

	//// TODO rewrite with intrinsics and plain C/C++
	//	// 48.16 fixed-point coordinates
	//const float flt_fix = 65536.0f;
	//float fx[3] = {poly->tvlist[v0].x, poly->tvlist[v1].x, poly->tvlist[v2].x};
	//float fy[3] = {poly->tvlist[v0].y, poly->tvlist[v1].y, poly->tvlist[v2].y};
	//__declspec(align(16)) __int64 X123[3], Y123[3], X3Y3[2]/*x3 - low part y3 - high part*/, X1Y1[2]/*x1 - low part y1 - high part*/;

	//// TODO rewrite with intrinsics and plain C/C++
	//__asm
	//{
	//	fld [fy + 8]	//st 6 = y2
	//	fld [fy + 4]	//st 5 = y1
	//	fld [fy + 0]	//st 4 = y0
	//	fld [fx + 8]	//st 3 = x2
	//	fld [fx + 4]	//st 2 = x1
	//	fld [fx + 0]	//st 1 = x0
	//	fld flt_fix		//st 0

	//	fmul st(5), st(0)
	//	fmul st(6), st(0)

	//	fmul st(4), st(0)
	//	fmul st(3), st(0)

	//	fmul st(2), st(0)
	//	fmulp st(1), st(0) //mul and pop flt_fix
	//			
	//	fisttp [X123]		//x0_fix
	//	fisttp [X123 + 8]	//x1_fix

	//	fisttp [X123 + 16]	//x2_fix
	//	fisttp [Y123]		//y0_fix

	//	movlps xmm0, [X123 + 16] //x3 to low part
	//	movlps xmm1, [X123]		 //x1 to low part

	//	fisttp [Y123 + 8]	//y1_fix
	//	fisttp [Y123 + 16]	//y2_fix

	//	movhps xmm1, [Y123]		 //y1 to high part
	//	movhps xmm0, [Y123 + 16] //y3 to high part

	//	movaps X1Y1, xmm1
	//	movaps X3Y3, xmm0
	//}

 //   // Deltas
	//__declspec(align(16)) __int64 DX12_23[2], FDX12_23[2];
	//__declspec(align(16)) __int64 DY12_23[2], FDY12_23[2];
	//__declspec(align(16)) __int64 DX31_DY31[2], FDX31_FDY31[2];

	//__asm
	//{
	//	movaps xmm2, X123 //xmm2 = x1, x2
	//	movaps xmm4, Y123 //xmm4 = y1, y2

	//	movups xmm3, [X123 + 8] // xmm3 = x2, x3
	//	movups xmm5, [Y123 + 8] // xmm5 = y2, y3

	//	psubq xmm0, xmm1 // xmm0 = dx31, dy31
	//	psubq xmm2, xmm3 // xmm2 = dx12, dx23

	//	movaps DX31_DY31, xmm0
	//	psubq xmm4, xmm5 // xmm4 = dy12, dy23

	//	movaps DX12_23, xmm2
	//	movaps DY12_23, xmm4

	//	// Fixed-point deltas. extending to 32.32

	//	psllq xmm0, 16 // xmm0 = fdx31, fdy31
	//	psllq xmm2, 16 // xmm2 = fdx12, fdx23

	//	movaps FDX31_FDY31, xmm0
	//	psllq xmm4, 16 // xmm4 = fdy12, fdy23

	//	movaps FDX12_23, xmm2
	//	movaps FDY12_23, xmm4
	//}

 //   // Bounding rectangle
	//int min_xy[2];
 //   /*int */min_xy[0] = (min(X123[0], min(X123[1], X123[2])) + 0xFFFF) >> 16;
 //   int maxx = (max(X123[0], max(X123[1], X123[2])) + 0xFFFF) >> 16;
 //   /*int */min_xy[1] = (min(Y123[0], min(Y123[1], Y123[2])) + 0xFFFF) >> 16;
 //   int maxy = (max(Y123[0], max(Y123[1], Y123[2])) + 0xFFFF) >> 16;

	//min_xy[0] = max(min_xy[0], (int)this->minClipX);
	//min_xy[1] = max(min_xy[1], (int)this->minClipY);

	//maxx = min(maxx, (int)this->maxClipX);
	//maxy = min(maxy, (int)this->maxClipY);

 //   vb = videoMemory + min_xy[1] * lp;

 //   // Half-edge constants in 32.32 fixp
	//__declspec(align(16)) __int64 C12[2];
	//__int64 C3;
	///*__int64 C1 = DY12_23[0] * X123[0] - DX12_23[0] * Y123[0];
 //   __int64 C2 = DY12_23[1] * X123[1] - DX12_23[1] * Y123[1];
 //   __int64 C3 = DX31_DY31[1] * X123[2] - DX31_DY31[0] * Y123[2];*/

	//__asm
	//{
	//	//DY12_23[0] * X123[0]  and   DY12_23[1] * X123[1]
	//	movaps xmm7, DY12_23 //low part a
	//	movaps xmm1, X123 //low part b

	//	movaps xmm2, DY12_23 // low part a
	//	movaps xmm3, X123 // low part b

	//	movaps xmm4, X123 // high part, after shift b
	//	movaps xmm5, DY12_23 // high part, after shift a

	//	psrlq xmm4, 32 // high part b
	//	psrlq xmm5, 32 // high part a

	//	pmuludq xmm7, xmm1 // lp_a * lp_b
	//	pmuludq xmm2, xmm4 // lp_a * hp_b

	//	pmuludq xmm3, xmm5 // lp_b * hp_a
	//	psllq xmm2, 32 // shift low parts of 2 64bit int's to high parts

	//	paddq xmm7, xmm2 
	//	psllq xmm3, 32 // shift low parts of 2 64bit int's to high parts

	//	paddq xmm7, xmm3
	//	//DX12_23[0] * Y123[0]   and   DX12_23[1] * Y123[1]
	//	movaps xmm6, DX12_23 //low part a

	//	movaps xmm1, Y123 //low part b
	//	movaps xmm2, DX12_23 // low part a

	//	movaps xmm3, Y123 // low part b
	//	movaps xmm4, Y123 // high part, after shift b

	//	movaps xmm5, DX12_23 // high part, after shift a
	//	psrlq xmm4, 32 // high part b

	//	psrlq xmm5, 32 // high part a
	//	pmuludq xmm6, xmm1 // lp_a * lp_b

	//	pmuludq xmm2, xmm4 // lp_a * hp_b
	//	pmuludq xmm3, xmm5 // lp_b * hp_a

	//	psllq xmm2, 32 // shift low parts of 2 64bit int's to high parts
	//	movaps xmm0, DX31_DY31

	//	paddq xmm6, xmm2
	//	movlps xmm1, [Y123 + 16]

	//	psllq xmm3, 32 // shift low parts of 2 64bit int's to high parts
	//	movhps xmm1, [X123 + 16]

	//	paddq xmm6, xmm3
	//	movaps xmm2, DX31_DY31

	//	psubq xmm7, xmm6
	//	movaps xmm3, xmm1

	//	movaps C12, xmm7
	//	movaps xmm4, xmm1

	//	movaps xmm5, DX31_DY31
	//	psrlq xmm4, 32 // high part b

	//	psrlq xmm5, 32 // high part a
	//	pmuludq xmm0, xmm1 // lp_a * lp_b
	//	
	//	pmuludq xmm2, xmm4 // lp_a * hp_b
	//	pmuludq xmm3, xmm5 // lp_b * hp_a

	//	psllq xmm2, 32 // shift low parts of 2 64bit int's to high parts
	//	paddq xmm0, xmm2 

	//	psllq xmm3, 32 // shift low parts of 2 64bit int's to high parts
	//	paddq xmm0, xmm3

	//	movhlps xmm1, xmm0
	//	psubq xmm1, xmm0

	//	movq C3, xmm1
	//}

 //   // Correct for fill convention
 //   if(DY12_23[0] < 0 || (DY12_23[0] == 0 && DX12_23[0] > 0)) C12[0]++;
 //   if(DY12_23[1] < 0 || (DY12_23[1] == 0 && DX12_23[1] > 0)) C12[1]++;
 //   if(DX31_DY31[1] < 0 || (DX31_DY31[1] == 0 && DX31_DY31[0] > 0)) C3++;

	////DX# in 48.16! After calculations CY# is in 32.32 fixp
 //  /* __int64 CY1 = C12[0] + DX12_23[0] * (miny << 16) - DY12_23[0] * (minx << 16);
 //   __int64 CY2 = C12[1] + DX12_23[1] * (miny << 16) - DY12_23[1] * (minx << 16);
 //   __int64 CY3 = C3 + DX31_DY31[0] * (miny << 16) - DX31_DY31[1] * (minx << 16);*/
	//__declspec(align(16)) __int64 CY_12[2];
	//__int64 CY3;

	//__asm
	//{
	//	movaps xmm0, DX12_23 // low part a
	//	movaps xmm4, DY12_23 // low part a2

	//	movq xmm1, qword ptr[min_xy]
	//	movq xmm5, qword ptr[min_xy]

	//	movaps xmm2, DX12_23 // high part, after shift a
	//	movaps xmm6, DY12_23 // high part, after shift a2

	//	pshufd xmm1, xmm1, 01010101b //y,y,y,y
	//	pshufd xmm5, xmm5, 00000000b //x,x,x,x

	//	psrlq xmm2, 32 // high part a
	//	psrlq xmm6, 32 // high part a2

	//	pslld xmm1, 16
	//	pslld xmm5, 16

	//	pmuludq xmm0, xmm1
	//	pmuludq xmm4, xmm5

	//	pmuludq xmm2, xmm1
	//	pmuludq xmm6, xmm5

	//	psllq xmm2, 32
	//	psllq xmm6, 32

	//	paddq xmm0, xmm2
	//	paddq xmm4, xmm6

	//	psubq xmm0, xmm4
	//	movaps xmm3, DX31_DY31 // low part a

	//	movaps xmm7, C12
	//	movlhps xmm1, xmm5 //x,x,y,y

	//	paddq xmm7, xmm0
	//	movaps xmm2, DX31_DY31 // high part, after shift a

	//	movaps CY_12, xmm7
	//	psrlq xmm2, 32 // high part a

	//	pmuludq xmm2, xmm1
	//	movq xmm5, C3

	//	pmuludq xmm3, xmm1
	//	psllq xmm2, 32

	//	paddq xmm3, xmm2
	//	movhlps xmm6, xmm3

	//	paddq xmm5, xmm3
	//	psubq xmm5, xmm6

	//	movq CY3, xmm5
	//}

	//__declspec(align(16)) __int64 CX_12[2];
	//__int64 CX3;

 //   for(int y = min_xy[1]; y < maxy; y++)
 //   {
 //      /* CX_12[0] = CY_12[0];
 //       CX_12[1] = CY_12[1];
 //       CX3 = CY3;*/
	//	__asm
	//	{
	//		movaps xmm0, CY_12
	//		movq xmm1, CY3

	//		movaps CX_12, xmm0
	//		movq CX3, xmm1
	//	}

 //       for(int x = min_xy[0]; x < maxx; x++)
 //       {
 //           if(CX_12[0] > 0 && CX_12[1] > 0 && CX3 > 0)
	//		{
 //               UINT color = ARGB32BIT(127,0,0,0);
 //  		        this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
 //           }

 //           /*CX_12[0] -= FDY12_23[0];
 //           CX_12[1] -= FDY12_23[1];
 //           CX3 -= FDX31_FDY31[1];*/
	//		__asm
	//		{
	//			movaps xmm0, CX_12
	//			movaps xmm1, FDY12_23

	//			movq xmm2, CX3
	//			movq xmm3, [FDX31_FDY31 + 8]

	//			psubq xmm0, xmm1
	//			psubq xmm2, xmm3

	//			movaps CX_12, xmm0
	//			movq CX3, xmm2
	//		}
 //       }

 //       /*CY_12[0] += FDX12_23[0];
 //       CY_12[1] += FDX12_23[1];
 //       CY3 += FDX31_FDY31[0];*/
	//	__asm
	//	{
	//		movaps xmm0, CY_12
	//		movaps xmm1, FDX12_23

	//		movq xmm2, CY3
	//		movq xmm3, [FDX31_FDY31 + 0]

	//		paddq xmm0, xmm1
	//		paddq xmm2, xmm3

	//		movaps CY_12, xmm0
	//		movq CY3, xmm2
	//	}

 //       vb += lp;
 //   }

	return 1;
}

void Draw32BitStrategy::DrawTriangleDefault(struct3D::POLYF4D_PTR face, unsigned int* videoMemory, int lpitch, int polyIdx)
{
	float x1 = face->tvlist[0].x;
	float y1 = face->tvlist[0].y;
	float x2 = face->tvlist[1].x;
	float y2 = face->tvlist[1].y;
	float x3 = face->tvlist[2].x;
	float y3 = face->tvlist[2].y;

	if ((x1 == x2 && x2 == x3)
		|| (y1 == y2 && y2 == y3)
		)
	{
		// degenerate triangle
		return;
	}

	if (y2 < y1) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	if (y3 < y1) {
		std::swap(x1, x3);
		std::swap(y1, y3);
	}
	if (y3 < y2) {
		std::swap(x2, x3);
		std::swap(y2, y3);
	}

	if (y3 < minClipY || y1 > maxClipY
		|| (x1 < minClipX && x2 < minClipX && x3 < minClipX)
		|| (x1 > maxClipX && x2 > maxClipX && x3 > maxClipX)
		)
	{
		// out of screen
		return;
	}

	UINT color = face->color;// ARGB32BIT(127, 0, 0, 0);

	ARGB32BIT(127, 0, 0, 0);

	int b = color & 255;
	int g = (color >> 8) & 255;
	int r = (color >> 16) & 255;
	int a = (color >> 24) & 255;

	if (y1 == y2)
	{
		//DrawTopTriDefault(x3, y3, x1, x2, y1, color, videoMemory, lpitch, polyIdx);
		DrawTopTriDefault2(x1, y1, x2, y2, x3, y3, color, videoMemory, lpitch, polyIdx);
	}
	else if (y3 == y2)
	{
		//DrawBottomTriDefault(x1, y1, x2, x3, y2, color, videoMemory, lpitch, polyIdx);
		DrawBottomTriDefault2(x1, y1, x2, y2, x3, y3, color, videoMemory, lpitch, polyIdx);
	}
	else
	{
		// y1 - bottom (screen coords)
		// y2 - middle (screen coords)
		// y3 - top (screen coords)
		// find where y2 divides edge y1 --> y3 for x
		float t = (y2 - y1) / (y3 - y1);
		float new_x = x1 + t * (x3 - x1);

		if (polyIdx == 53)
		{
			int stop = 234;
		}
		if (polyIdx == 54)
		{
			int stop = 234;
		}
		if (polyIdx == 56)
		{
			int stop = 234;
		}

		/*DrawBottomTriDefault(x1, y1, x3, y3, x2, y2, color, videoMemory, lpitch, polyIdx);
		DrawTopTriDefault(x3, y3, x1, y1, x2, y2, color, videoMemory, lpitch, polyIdx);*/

		DrawBottomTriDefault2(x1, y1, x2, y2, x3, y3, color, videoMemory, lpitch, polyIdx);
		DrawTopTriDefault2(x1, y1, x2, y2, x3, y3, color, videoMemory, lpitch, polyIdx);
	}
}

void Draw32BitStrategy::DrawTopTriDefault(float xTop, float yTop, float xBottom1, float yBottom1, float xBottom2, float yBottom, unsigned int color, unsigned int* vb, int lpitch, int polyIdx)
{
	float yBottomClipped = Draw32BitStrategy::clamp(yBottom, this->minClipY, this->maxClipY);
	float yTopClipped = Draw32BitStrategy::clamp(yTop, this->minClipY, this->maxClipY);

	for (float y = yBottomClipped; y < yTopClipped; y++)
	{
		float t = (y - yBottom) / (yTop - yBottom);
		float t1 = (y - yBottom1) / (yTop - yBottom1);
		float leftX = Draw32BitStrategy::lerp(t1, xBottom1, xTop);
		float rightX = Draw32BitStrategy::lerp(t, xBottom2, xTop);
		DrawHLineDefault(leftX, rightX, y, (std::min)(y + 1.f, yTop), color, vb, lpitch, polyIdx);
	}
}

void Draw32BitStrategy::DrawBottomTriDefault(float xBottom, float yBottom, float xTop1, float yTop1, float xTop2, float yTop, unsigned int color, unsigned int* vb, int lpitch, int polyIdx)
{
	float yBottomClipped = Draw32BitStrategy::clamp(yBottom, this->minClipY, this->maxClipY);
	float yTopClipped = Draw32BitStrategy::clamp(yTop, this->minClipY, this->maxClipY);

	for (float y = yBottomClipped; y < yTopClipped; y++)
	{
		float t = (y - yBottom) / (yTop - yBottom);
		float t1 = (y - yBottom) / (yTop1 - yBottom);
		float leftX = Draw32BitStrategy::lerp(t1, xBottom, xTop1);
		float rightX = Draw32BitStrategy::lerp(t, xBottom, xTop2);
		DrawHLineDefault(leftX, rightX, y, (std::min)(y + 1.f, yTop), color, vb, lpitch, polyIdx);
	}
}

void Draw32BitStrategy::DrawTopTriDefault2(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, int lpitch, int polyIdx)
{
	float yStart = Draw32BitStrategy::clamp(y2, this->minClipY, this->maxClipY);
	float yEnd = Draw32BitStrategy::clamp(y3, this->minClipY, this->maxClipY);

	float istart = std::floor(yStart);
	float iend = std::floor(yEnd);

	float startCenter = istart + 0.5f;
	float endCenter = iend + 0.5f;

	if (startCenter < yStart)
	{
		startCenter++;
	}

	if (endCenter < yEnd)
	{
		endCenter++;
	}

	for (float y = startCenter; y < endCenter; y++)
	{
		float t13 = (y - y1) / (y3 - y1);
		float t23 = (y - y2) / (y3 - y2);

		float x13 = Draw32BitStrategy::lerp(t13, x1, x3);
		float x23 = Draw32BitStrategy::lerp(t23, x2, x3);
		DrawHLineDefault(x13, x23, y, (std::min)(y + 1.f, y3), color, vb, lpitch, polyIdx);
	}

	/*for (float y = yStart; y < yEnd; y++)
	{
		float t13 = (y - y1) / (y3 - y1);
		float t23 = (y - y2) / (y3 - y2);

		float x13 = Draw32BitStrategy::lerp(t13, x1, x3);
		float x23 = Draw32BitStrategy::lerp(t23, x2, x3);
		DrawHLineDefault(x13, x23, y, (std::min)(y + 1.f, y3), color, vb, lpitch, polyIdx);
	}*/
}

void Draw32BitStrategy::DrawBottomTriDefault2(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color, unsigned int* vb, int lpitch, int polyIdx)
{
	float yStart = Draw32BitStrategy::clamp(y1, this->minClipY, this->maxClipY);
	float yEnd = Draw32BitStrategy::clamp(y2, this->minClipY, this->maxClipY);

	float istart = std::floor(yStart);
	float iend = std::floor(yEnd);

	float startCenter = istart + 0.5f;
	float endCenter = iend + 0.5f;

	if (startCenter < yStart)
	{
		startCenter++;
	}

	if (endCenter < yEnd)
	{
		endCenter++;
	}

	for (float y = startCenter; y < endCenter; y++)
	{
		float t13 = (y - y1) / (y3 - y1);
		float t12 = (y - y1) / (y2 - y1);

		float x13 = Draw32BitStrategy::lerp(t13, x1, x3);
		float x12 = Draw32BitStrategy::lerp(t12, x1, x2);
		DrawHLineDefault(x13, x12, y, (std::min)(y + 1.f, y2), color, vb, lpitch, polyIdx);
	}

	/*for (float y = yStart; y < yEnd; y++)
	{
		float t13 = (y - y1) / (y3 - y1);
		float t12 = (y - y1) / (y2 - y1);

		float x13 = Draw32BitStrategy::lerp(t13, x1, x3);
		float x12 = Draw32BitStrategy::lerp(t12, x1, x2);
		DrawHLineDefault(x13, x12, y, (std::min)(y + 1.f, y2), color, vb, lpitch, polyIdx);
	}*/
}

void Draw32BitStrategy::DrawHLineDefault(float leftX, float rightX, float topY, float bottomY, unsigned int color, unsigned int* vb, int lpitch, int polyIdx)
{
	if (leftX > rightX)
	{
		std::swap(leftX, rightX);
	}

	leftX = Draw32BitStrategy::clamp(leftX, this->minClipX, this->maxClipX);
	rightX = Draw32BitStrategy::clamp(rightX, this->minClipX, this->maxClipX);

	if (topY >= 217 && topY < 218)
	{
		if (polyIdx == 53)
		{
			// top
			int stop = 234;
		}
		if (polyIdx == 54)
		{
			// bottom
			int stop = 234;
		}
		if (polyIdx == 56)
		{
			// top
			int stop = 234;
		}

		int stop = 234;
	}

	if (leftX >= rightX)
	{
		return;
	}

	RoundedRange yRange = RoundRange(topY, bottomY);

	if (yRange.start >= 215 && yRange.end <= 218)
	{
		if (polyIdx == 53)
		{
			int stop = 234;
		}
		if (polyIdx == 54)
		{
			int stop = 234;
		}
		if (polyIdx == 56)
		{
			int stop = 234;
		}

		int stop = 234;
	}

	if (yRange.start >= yRange.end)
	{
		return;
	}

	RoundedRange xRange = RoundRange(leftX, rightX);

	vb = (uint32_t*)((uint8_t*)vb + yRange.start * lpitch);

	for (int x = xRange.start; x < xRange.end; x++)
	{
		this->_alphaBlender->AlphaBlend(&vb[x], &color, 1);
	}
}

Draw32BitStrategy::RoundedRange Draw32BitStrategy::RoundRange(float start, float end)
{
	float istart = std::floor(start);
	float iend = std::floor(end);

	float startCenter = istart + 0.5f;
	float endCenter = iend + 0.5f;

	RoundedRange res;

	if (start <= startCenter)
	{
		res.start = static_cast<int>(istart);
	}
	else
	{
		res.start = static_cast<int>(istart) + 1;
	}

	if (end <= endCenter)
	{
		res.end = static_cast<int>(iend);
	}
	else
	{
		res.end = static_cast<int>(iend) + 1;
	}

	return res;
}

float Draw32BitStrategy::lerp(float t, float start, float end)
{
	float res = start + t * (end - start);
	return res;
}

float Draw32BitStrategy::clamp(float x, float min, float max)
{
	float res = (std::max)((std::min)(x, max), min);
	return res;
}
