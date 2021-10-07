#include "StdAfx.h"
#include "GraphicImp.h"


GraphicImpDirectDraw::GraphicImpDirectDraw(){
	_lpdd7 = 0;
	_lpddsurf = 0;
	_lpddbacksurf = 0;
	_lpddclip = 0;
}

int GraphicImpDirectDraw::Initialize(_screenParams *params, HWND hWnd)
{
	//if(FAILED()) return 0;

	memcpy(&_sp, params, sizeof(_screenParams));

	state.reset();

	this->ppObj3DMap = 0;

	_hWnd = hWnd;
	isBufferLocked = 0;

	if(FAILED(DirectDrawCreateEx(NULL, (void **) &_lpdd7, IID_IDirectDraw7, NULL))) return 0;

	if(params->iFullScreen){
		if(FAILED(_lpdd7->SetCooperativeLevel(_hWnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))) return 0;
		if(FAILED(_lpdd7->SetDisplayMode(params->iWidth, params->iHeight, params->iBitsPerPixel, 0, 0))) return 0;
	}
	else{
		/*RECT wndRect, clientRect;

		GetWindowRect(_hWnd, &wndRect);
		GetClientRect(_hWnd, &clientRect);

		int iWinWidth = params->iWidth + ((wndRect.right - wndRect.left) - (clientRect.right - clientRect.left));
		int iWinHeight = params->iHeight + ((wndRect.bottom - wndRect.top) - (clientRect.bottom - clientRect.top));

		MoveWindow(_hWnd, wndRect.left, wndRect.top, iWinWidth, iWinHeight, 1);*/

		if(FAILED(_lpdd7->SetCooperativeLevel(_hWnd, DDSCL_NORMAL))) return 0;
	}

	ZeroMemory(&_ddsd, sizeof(_ddsd));
	_ddsd.dwSize = sizeof(_ddsd);

	if(params->iFullScreen){
		_ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_SYSTEMMEMORY;
		_ddsd.dwBackBufferCount = 1;
	}
	else{
		_ddsd.dwFlags = DDSD_CAPS;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	if(FAILED(_lpdd7->CreateSurface(&_ddsd,&_lpddsurf,NULL))) return 0;

	if(params->iFullScreen){
		DDSCAPS2 ddsc;
		ZeroMemory(&ddsc, sizeof(ddsc));
		ddsc.dwCaps = DDSCAPS_BACKBUFFER;
		if(FAILED(_lpddsurf->GetAttachedSurface(&ddsc, &_lpddbacksurf))) return 0;

		_flipStrategy = new FlipFullscreenDDStrategy(_lpddsurf);
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	}
	else{
		ZeroMemory(&_ddsd, sizeof(_ddsd));
		_ddsd.dwSize = sizeof(_ddsd);

		_ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		_ddsd.dwWidth = params->iWidth;
		_ddsd.dwHeight = params->iHeight;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

		if(FAILED(_lpdd7->CreateSurface(&_ddsd, &_lpddbacksurf, NULL))) return 0;

		if(FAILED(_lpdd7->CreateClipper(0, &_lpddclip, 0))) return 0;
		if(FAILED(_lpddclip->SetHWnd(0, _hWnd))) return 0;
		if(FAILED(_lpddsurf->SetClipper(_lpddclip))) return 0;

		_flipStrategy = new FlipWindowedDDStrategy(_lpddsurf, _lpddbacksurf, _hWnd, _sp.iWidth, _sp.iHeight);
	}

	this->ppObj3DMap = 0;
	/*this->ppObj3DMap = new POLYF4D_PTR[params->iWidth * params->iHeight];
	memset(this->ppObj3DMap, 0, sizeof(POLYF4D_PTR) * (params->iWidth * params->iHeight));*/
	if(!InitializeDrawStrategies(params)) return 0;

	//3DInit:
	POINT4D camPos = {0, 0, 0, 1};
	VECTOR4D camDir = {0, 0, 0, 1};
	POINT4D target = {0, 0, 1, 1};

	mainCam.Init_CAM4D(struct3D::CAM_MODEL_EULER, &camPos, &camDir, &target, 10.0f, 100000.0f, 90.0f, params->iWidth, params->iHeight); 
	//mainCam.Build_CAM4D_Matrix_Euler(struct3D::CAM_ROT_SEQ_ZYX);

	return 1;
}

int GraphicImpDirectDraw::InitializeDrawStrategies(_screenParams *params){

	if(params->iBitsPerPixel == 32)
	{
		_draw = new Draw32BitStrategy(params->iGraphicQuality);
		_draw->setClipBorders(0, 0, params->iWidth, params->iHeight);
		//_draw->setObj3DMap(this->ppObj3DMap, this->_sp.iWidth, this->_sp.iHeight);
		_draw->setScreenSize(this->_sp.iWidth, this->_sp.iHeight);
	}
	else if(params->iBitsPerPixel == 16)
	{
		_draw = new Draw16BitStrategy(params->iGraphicQuality);
		_draw->setClipBorders(0, 0, params->iWidth, params->iHeight);
		//_draw->setObj3DMap(this->ppObj3DMap, this->_sp.iWidth, this->_sp.iHeight);
		_draw->setScreenSize(this->_sp.iWidth, this->_sp.iHeight);
	}
	else
		return 0;

	return 1;
}

int GraphicImpDirectDraw::DrawBegin(bool bClearScreen){

	if(isBufferLocked){
		DrawEnd();
	}

	ZeroMemory(&_ddsd, sizeof(_ddsd));
	_ddsd.dwSize = sizeof(_ddsd);

	/*if(this->ppObj3DMap)
		memset(this->ppObj3DMap, 0, sizeof(POLYF4D_PTR) * this->_sp.iWidth * this->_sp.iHeight);*/
	
	if(bClearScreen){
		ZeroMemory(&_ddblt, sizeof(_ddblt));
		_ddblt.dwSize = sizeof(_ddblt);

		_ddblt.dwFillColor = _ARGB32BIT(0,255,255,255);//0;//

		if(FAILED(_lpddbacksurf->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &_ddblt))) return 0;
	}

	if(FAILED(_lpddbacksurf->Lock(NULL, &_ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))) {
		isBufferLocked = 0;
		return 0;
	}
	isBufferLocked = 1;
	_videoBuffer = _ddsd.lpSurface;
	_lPitch = _ddsd.lPitch;

	this->_draw->clearZBuffer();

	return 1;
}

int GraphicImpDirectDraw::DrawEnd(){
	if(isBufferLocked){
		if(FAILED(_lpddbacksurf->Unlock(NULL))){
			isBufferLocked = 1;
			return 0;
		}
		/*RECT r;
		r.left = r.top = 0;
		r.bottom = r.right = 100;
		if(FAILED(_lpddbacksurf->Blt(NULL, _lpddbacksurf, &r, DDBLT_WAIT, 0))) return 0;*/

		/*_flipStrategy->Flip();*/
	}
	_flipStrategy->Flip();
	isBufferLocked = 0;
	return 1;
}

int GraphicImpDirectDraw::DrawPixel(math3D::POINT2DI *pts, int numPts, unsigned int color)
{
	return _draw->DrawPixel(pts, numPts, color, _videoBuffer,_lPitch);
}
int GraphicImpDirectDraw::DrawPixel(math3D::POINT2D *pts, int numPts, unsigned int color)
{
	return _draw->DrawPixel(pts, numPts, color, _videoBuffer,_lPitch);
}

int GraphicImpDirectDraw::DrawLine(int x0, int y0, int x1, int y1, unsigned int color){
	if(isBufferLocked){
		if(ClipLine(x0, y0, x1, y1, &_sp))
		{
			_draw->DrawLine(x0,y0,x1,y1,color,_videoBuffer,_lPitch);
		}
		return 1;
	}
	return 0;
}

int GraphicImpDirectDraw::DrawText(TCHAR *tchTextString, int iStrLen, int iX, int iY, unsigned int iColor){
	if(isBufferLocked){
	
		iColor = (iColor & 0x0000FF00) | ((iColor >> 16) & 0x000000FF) | ((iColor << 16) & 0x00FF0000);

		_lpddbacksurf->GetDC(&_HDC);

		SetBkMode(_HDC, TRANSPARENT);
		SetTextColor(_HDC, iColor);
		TextOut(_HDC, iX, iY, tchTextString, iStrLen);
		
		_lpddbacksurf->ReleaseDC(_HDC);

		return 1;
	}
	return 0;
}

int GraphicImpDirectDraw::ClipLine( int &x1, int &y1, int &x2, int &y2, _screenParams *params){


#define CLIP_CODE_C 0x0000
#define CLIP_CODE_N 0x0008
#define CLIP_CODE_S 0x0004
#define CLIP_CODE_E 0x0002
#define CLIP_CODE_W 0x0001

#define CLIP_CODE_NE 0x000A
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009
#define CLIP_CODE_SW 0x0005

	int MIN_CLIP_Y = 2;
	int MIN_CLIP_X = 1;
	int MAX_CLIP_Y = params->iHeight - 2;
	int MAX_CLIP_X = params->iWidth - 1;

	int xc1 = x1, yc1 = y1, xc2 = x2, yc2 = y2;

	int p1_code = 0, p2_code = 0;

	if(y1 < MIN_CLIP_Y) p1_code |= CLIP_CODE_N;
	else if(y1 > MAX_CLIP_Y) p1_code |= CLIP_CODE_S;

	if(x1 < MIN_CLIP_X) p1_code |= CLIP_CODE_W;
	else if(x1 > MAX_CLIP_X) p1_code |= CLIP_CODE_E;

	if(y2 < MIN_CLIP_Y) p2_code |= CLIP_CODE_N;
	else if(y2 > MAX_CLIP_Y) p2_code |= CLIP_CODE_S;

	if(x2 < MIN_CLIP_X) p2_code |= CLIP_CODE_W;
	else if(x2 > MAX_CLIP_X) p2_code |= CLIP_CODE_E;

	if((p1_code & p2_code)) return 0;
	
	if(p1_code == 0 && p2_code == 0) return 1;

	switch(p1_code){
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
			yc1 = MIN_CLIP_Y;
			xc1 = x1 + 0.5 + (MIN_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			break;
		case CLIP_CODE_S:
			yc1 = MAX_CLIP_Y;
			xc1 = x1 + 0.5 + (MAX_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			break;
		case CLIP_CODE_W:
			xc1 = MIN_CLIP_X;
			yc1 = y1 + 0.5 + (MIN_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			break;
		case CLIP_CODE_E:
			xc1 = MAX_CLIP_X;
			yc1 = y1 + 0.5 + (MAX_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			break;
		case CLIP_CODE_NE:
			yc1 = MIN_CLIP_Y;
			xc1 = x1 + 0.5 + (MIN_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			if(xc1 < MIN_CLIP_X || xc1 > MAX_CLIP_X){
				xc1 = MAX_CLIP_X;
				yc1 = y1 + 0.5 + (MAX_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			}
			break;
		case CLIP_CODE_SE:
			yc1 = MAX_CLIP_Y;
			xc1 = x1 + 0.5 + (MAX_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			if(xc1 < MIN_CLIP_X || xc1 > MAX_CLIP_X){
				xc1 = MAX_CLIP_X;
				yc1 = y1 + 0.5 + (MAX_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			}
			break;
		case CLIP_CODE_NW:
			yc1 = MIN_CLIP_Y;
			xc1 = x1 + 0.5 + (MIN_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			if(xc1 < MIN_CLIP_X || xc1 > MAX_CLIP_X){
				xc1 = MIN_CLIP_X;
				yc1 = y1 + 0.5 + (MIN_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			}
			break;
		case CLIP_CODE_SW:
			yc1 = MAX_CLIP_Y;
			xc1 = x1 + 0.5 + (MAX_CLIP_Y - y1) * (x2 - x1) / (y2 - y1);
			if(xc1 < MIN_CLIP_X || xc1 > MAX_CLIP_X){
				xc1 = MIN_CLIP_X;
				yc1 = y1 + 0.5 + (MIN_CLIP_X - x1) * (y2 - y1) / (x2 - x1);
			}
			break;
		default : break;
	}

	switch(p2_code){
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
			yc2 = MIN_CLIP_Y;
			xc2 = x2 + 0.5 + (MIN_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			break;
		case CLIP_CODE_S:
			yc2 = MAX_CLIP_Y;
			xc2 = x2 + 0.5 + (MAX_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			break;
		case CLIP_CODE_W:
			xc2 = MIN_CLIP_X;
			yc2 = y2 + 0.5 + (MIN_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			break;
		case CLIP_CODE_E:
			xc2 = MAX_CLIP_X;
			yc2 = y2 + 0.5 + (MAX_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			break;
		case CLIP_CODE_NE:
			yc2 = MIN_CLIP_Y;
			xc2 = x2 + 0.5 + (MIN_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			if(xc2 < MIN_CLIP_X || xc2 > MAX_CLIP_X){
				xc2 = MAX_CLIP_X;
				yc2 = y2 + 0.5 + (MAX_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			}
			break;
		case CLIP_CODE_SE:
			yc2 = MAX_CLIP_Y;
			xc2 = x2 + 0.5 + (MAX_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			if(xc2 < MIN_CLIP_X || xc2 > MAX_CLIP_X){
				xc2 = MAX_CLIP_X;
				yc2 = y2 + 0.5 + (MAX_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			}
			break;
		case CLIP_CODE_NW:
			yc2 = MIN_CLIP_Y;
			xc2 = x2 + 0.5 + (MIN_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			if(xc2 < MIN_CLIP_X || xc1 > MAX_CLIP_X){
				xc2 = MIN_CLIP_X;
				yc2 = y2 + 0.5 + (MIN_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			}
			break;
		case CLIP_CODE_SW:
			yc2 = MAX_CLIP_Y;
			xc2 = x2 + 0.5 + (MAX_CLIP_Y - y2) * (x1 - x2) / (y1 - y2);
			if(xc2 < MIN_CLIP_X || xc2 > MAX_CLIP_X){
				xc2 = MIN_CLIP_X;
				yc2 = y2 + 0.5 + (MIN_CLIP_X - x2) * (y1 - y2) / (x1 - x2);
			}
			break;
		default : break;
	}


	if((xc1 < MIN_CLIP_X) || (xc1 > MAX_CLIP_X) || (yc1  < MIN_CLIP_Y) || (yc1 > MAX_CLIP_Y) || (xc2  < MIN_CLIP_X) || (xc2 > MAX_CLIP_X) || (yc2 < MIN_CLIP_Y) || (yc2 > MAX_CLIP_Y)) return 0;

	x1 = xc1;
	x2 = xc2;
	y1 = yc1;
	y2 = yc2;

	return 1;

}

void GraphicImpDirectDraw::ClearSurface(LPDIRECTDRAWSURFACE7 _lpSurf){
	ZeroMemory(&_ddblt, sizeof(_ddblt));
	_ddblt.dwSize = sizeof(_ddblt);
	_ddblt.dwFillColor = 0;

	_lpSurf->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &_ddblt);
}

void GraphicImpDirectDraw::Release(){

	if(_lpddclip){
		_lpddclip->Release();
		_lpddclip = 0;
	}
	if(_lpddbacksurf){
		_lpddbacksurf->Release();
		_lpddbacksurf = 0;
	}
	if(_lpddsurf){
		_lpddsurf->Release();
		_lpddsurf = 0;
	}
	if(_lpdd7){
		_lpdd7->Release();
		_lpdd7 = 0;
	}

	if(_flipStrategy){
		delete _flipStrategy;
		_flipStrategy = 0;
	}
	if(_draw){
		//delete _draw;
		_draw->Release();
		delete _draw;
		_draw = 0;
	}
	if(this->ppObj3DMap)
	{
		delete this->ppObj3DMap;
		this->ppObj3DMap = 0;
	}
}

int GraphicImpDirectDraw::DrawHLine(float x1, float x2, int y, unsigned int iColor)
{
	_draw->DrawHLine(x1, x2, y, iColor, _videoBuffer, _lPitch);

	return 1;
}

int GraphicImpDirectDraw::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, unsigned int iColor)
{
	_draw->DrawTriangle(x1, y1, x2, y2, x3, y3, iColor, (unsigned int *)_videoBuffer, _lPitch);

	return 1;
}

void GraphicImpDirectDraw::DrawOBJECT4DWire(OBJECT4D_PTR obj)
{
	pipe.ModelToWorldOBJECT4D(obj);

	if(!(obj->attr & struct3D::POLY4D_ATTR_2SIDED))
		pipe.RemoveBackfacesOBJECT4D(obj, &mainCam);
	mainCam.Build_CAM4D_Matrix_Euler(struct3D::CAM_ROT_SEQ_ZYX);
	pipe.WorldToCameraOBJECT4D(&mainCam, obj);
	if(pipe.bLighting)
	{
		pipe.lights.transformLights(&mainCam.mcam);
		pipe.LightOBJECT4D(obj, &mainCam);
	}
	pipe.CameraToPerspectiveOBJECT4D(obj, &mainCam);
	pipe.PerspectiveToScreenOBJECT4D(obj, &mainCam);

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		if(!(obj->plist[poly].state & struct3D::POLY4D_STATE_ACTIVE) || (obj->plist[poly].state & struct3D::POLY4D_STATE_CLIPPED) || (obj->plist[poly].state & struct3D::POLY4D_STATE_BACKFACE)) continue;

		int vindex0 = obj->plist[poly].vert[0];
		int vindex1 = obj->plist[poly].vert[1];
		int vindex2 = obj->plist[poly].vert[2];

		this->DrawLine(obj->vlist_trans[vindex0].x, obj->vlist_trans[vindex0].y, obj->vlist_trans[vindex1].x, obj->vlist_trans[vindex1].y, obj->plist[poly].lit_color[0]);
		this->DrawLine(obj->vlist_trans[vindex1].x, obj->vlist_trans[vindex1].y, obj->vlist_trans[vindex2].x, obj->vlist_trans[vindex2].y, obj->plist[poly].lit_color[0]);
		this->DrawLine(obj->vlist_trans[vindex2].x, obj->vlist_trans[vindex2].y, obj->vlist_trans[vindex0].x, obj->vlist_trans[vindex0].y, obj->plist[poly].lit_color[0]);
	}
}
void GraphicImpDirectDraw::DrawRENDERLIST4DWire(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos)
{
	pipe.WorldToCamera_and_BackfaceRemoveRENDERLIST4D(rendList, &mainCam);
	pipe.ClipPolysRENDERLIST4D(rendList, &mainCam, clipPoly::CLIP_POLY_XYZ_PLANES);
	if(pipe.bLighting)
	{
		pipe.lights.transformLights(&mainCam.mcam);
		pipe.LightRENDERLIST4D(rendList, &mainCam);
	}
	pipe.CameraToScreenRENDERLIST4D(rendList, &mainCam);

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		if(!(rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_ACTIVE) || (rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_CLIPPED) || (rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_BACKFACE)) continue;

		this->DrawLine(rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->lit_color[0]);
		this->DrawLine(rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->tvlist[2].x, rendList->poly_ptrs[poly]->tvlist[2].y, rendList->poly_ptrs[poly]->lit_color[0]);
		this->DrawLine(rendList->poly_ptrs[poly]->tvlist[2].x, rendList->poly_ptrs[poly]->tvlist[2].y, rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->lit_color[0]);

		if(this->state.checkFlag(RendState::RS_SHOWVERTEX))
		{
			RECT r;
			int rSize = this->state.checkFlag(RendState::RS_VERTEXSIZE) * this->_sp.iHeight / this->_sp.iHeight;
			unsigned int iColor;

			r.left = rendList->poly_ptrs[poly]->tvlist[0].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[0].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[0].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[0].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[0].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);

			r.left = rendList->poly_ptrs[poly]->tvlist[1].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[1].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[1].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[1].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[1].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);

			r.left = rendList->poly_ptrs[poly]->tvlist[2].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[2].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[2].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[2].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[2].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);
		}
		//this->DrawTriangle(rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->tvlist[2].x, rendList->poly_ptrs[poly]->tvlist[2].y, rendList->poly_ptrs[poly]->color);
	}
}

void GraphicImpDirectDraw::DrawOBJECT4DSolid(OBJECT4D_PTR obj)
{
	pipe.ModelToWorldOBJECT4D(obj);
	pipe.RemoveBackfacesOBJECT4D(obj, &mainCam);
	mainCam.Build_CAM4D_Matrix_Euler(struct3D::CAM_ROT_SEQ_ZYX);
	pipe.WorldToCameraOBJECT4D(&mainCam, obj);
	if(pipe.bLighting)
	{
		pipe.lights.transformLights(&mainCam.mcam);
		pipe.LightOBJECT4D(obj, &mainCam);
	}
	pipe.CameraToPerspectiveOBJECT4D(obj, &mainCam);
	pipe.PerspectiveToScreenOBJECT4D(obj, &mainCam);

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		if(!(obj->plist[poly].state & struct3D::POLY4D_STATE_ACTIVE) || (obj->plist[poly].state & struct3D::POLY4D_STATE_CLIPPED) || (obj->plist[poly].state & struct3D::POLY4D_STATE_BACKFACE)) continue;

		int vindex0 = obj->plist[poly].vert[0];
		int vindex1 = obj->plist[poly].vert[1];
		int vindex2 = obj->plist[poly].vert[2];

		if(obj->plist[poly].attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT)
			this->_draw->DrawTriangle(obj->vlist_trans[vindex0].x, obj->vlist_trans[vindex0].y, obj->vlist_trans[vindex1].x, obj->vlist_trans[vindex1].y, obj->vlist_trans[vindex2].x, obj->vlist_trans[vindex2].y, obj->plist[poly].lit_color[0], (unsigned int *)_videoBuffer, _lPitch);
		else if(obj->plist[poly].attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD)
			this->_draw->DrawGouraudTriangle(obj->vlist_trans[vindex0].x, obj->vlist_trans[vindex0].y, obj->vlist_trans[vindex1].x, obj->vlist_trans[vindex1].y, obj->vlist_trans[vindex2].x, obj->vlist_trans[vindex2].y, obj->plist[poly].lit_color[0], obj->plist[poly].lit_color[1], obj->plist[poly].lit_color[2], (unsigned int *)_videoBuffer, _lPitch);
	}
}
void GraphicImpDirectDraw::DrawRENDERLIST4DSolid(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos)
{
	/*if(worldPos != NULL)
	{
		pipe.ModelToWorldRENDERLIST4D(rendList, worldPos);
	}*/
	//pipe.RemoveBackfacesRENDERLIST4D(rendList, &mainCam);
	//mainCam.Build_CAM4D_Matrix_Euler(struct3D::CAM_ROT_SEQ_ZYX);
	//pipe.WorldToCameraRENDERLIST4D(&mainCam, rendList);

	pipe.WorldToCamera_and_BackfaceRemoveRENDERLIST4D(rendList, &mainCam);

	pipe.ClipPolysRENDERLIST4D(rendList, &mainCam, clipPoly::CLIP_POLY_Z_PLANE);

	//if(pipe.bLighting)
	if(state.checkFlag(RendState::RS_LIGHTING))
	{
		pipe.lights.transformLights(&mainCam.mcam);
		pipe.LightRENDERLIST4D(rendList, &mainCam);
	}
	//pipe.WorldToCameraLights(&mainCam);
	pipe.SortRENDERLIST4D(rendList, sortMethod::SORT_POLYLIST_AVZG);

	

	pipe.CameraToPerspectiveRENDERLIST4D(rendList, &mainCam);
	pipe.PerspectiveToScreenRENDERLIST4D(rendList, &mainCam);
	//pipe.CameraToScreenRENDERLIST4D(rendList, &mainCam);

	TCHAR mas[128];
	if(KEYDOWN(VK_NUMPAD5))
	{
		swprintf(mas, std::size(mas), L"tri: %f %f %f | %f %f %f | %f %f %f %F", rendList->poly_data[idx].tvlist[0].x, rendList->poly_data[idx].tvlist[0].y, rendList->poly_data[idx].tvlist[0].z,  rendList->poly_data[idx].tvlist[1].x, rendList->poly_data[idx].tvlist[1].y, rendList->poly_data[idx].tvlist[1].z, rendList->poly_data[idx].tvlist[2].x, rendList->poly_data[idx].tvlist[2].y, rendList->poly_data[idx].tvlist[2].z, rendList->poly_data[idx].tvlist[2].z);
		MessageBox(0, mas, 0, 0);
	}

	if(KEYDOWN(VK_NUMPAD2))
	{
		for(int i = 0; i < 3 ; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if((memcmp(&rendList->poly_data[idx].tvlist[i].x, &rendList->poly_data[idx2].tvlist[j].x, sizeof(float)) == 0) && (memcmp(&rendList->poly_data[idx].tvlist[i].y, &rendList->poly_data[idx2].tvlist[j].y, sizeof(float)) == 0) && (memcmp(&rendList->poly_data[idx].tvlist[i].z, &rendList->poly_data[idx2].tvlist[j].z, sizeof(float)) == 0))
				{
					
					swprintf(mas, std::size(mas), L"%d %d == %d %d", idx, i, idx2, j);
					MessageBox(0, mas, 0, 0);
				}
			}
		}
	}

	static int idx_old = -234;

	if(KEYDOWN(VK_NUMPAD1))
	{
		//wxFile file;

		//if(/*(idx != idx_old)*/file.Open("triTest.bin", wxFile::read_write))
		//{
		//	//file.Create("triTest.bin");
		//	file.SeekEnd(0);
		//	POLYF4D *p = &rendList->poly_data[idx];

		//	for(int i = 0; i < 3; i++)
		//	{
		//		file.Write(&p->tvlist[i].x, sizeof(float));
		//		file.Write(&p->tvlist[i].y, sizeof(float));
		//		file.Write(&p->tvlist[i].z, sizeof(float));
		//	}

		//	idx_old = idx;

		//	MessageBox(0, L"poly saved to file...", 0, 0);
		//}
		//else
		//{
		//	MessageBox(0, L"File Open Error!!!", 0, 0);
		//}
	}

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		if(!(rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_ACTIVE) || (rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_CLIPPED) || (rendList->poly_ptrs[poly]->state & struct3D::POLY4D_STATE_BACKFACE)) continue;

		/*if(rendList->poly_ptrs[poly]->attr & mat::MAT_ATTR_SHADE_MODE_TEXTURE)
		{
			this->_draw->DrawTriangleTex(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);
			continue;
		}
		if(rendList->poly_ptrs[poly]->attr & mat::MAT_ATTR_SHADE_MODE_1PLINE)
		{
			this->DrawLine(rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->lit_color[0]);
			continue;
		}*/

		/*if(!this->state.checkFlag(RendState::RS_WIREFRAME) || (rendList->poly_ptrs[poly]->attr & POLY4D_ATTR_SELECTED))
		{*/
			if((rendList->poly_ptrs[poly]->attr & POLY4D_ATTR_SELECTED))
			{
				rendList->poly_ptrs[poly]->color = rendList->poly_ptrs[poly]->lit_color[0] = rendList->poly_ptrs[poly]->lit_color[1] = rendList->poly_ptrs[poly]->lit_color[2] = ARGB32BIT(255,255,0,0);
			}
		if(rendList->poly_ptrs[poly]->attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT)
			//this->_draw->DrawTriangle(rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->tvlist[2].x, rendList->poly_ptrs[poly]->tvlist[2].y, rendList->poly_ptrs[poly]->lit_color[0], (unsigned int *)_videoBuffer, _lPitch);
			this->_draw->DrawTriangle2(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);
		else if(rendList->poly_ptrs[poly]->attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD)
			/*this->_draw->DrawGouraudTriangle3(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);*/
			/*this->_draw->DrawTriangle4(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);*/
			this->_draw->DrawTriangle7_sse(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);
		else if(rendList->poly_ptrs[poly]->attr & struct3D::POLY4D_ATTR_SHADE_MODE_PHONG)
			this->_draw->DrawPhongTriangle2(&this->mainCam, &pipe.lights, rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);
		else if(rendList->poly_ptrs[poly]->attr & struct3D::POLY4D_ATTR_SHADE_MODE_TEST)
			this->_draw->DrawTriangle3(rendList->poly_ptrs[poly], (unsigned int *)_videoBuffer, _lPitch);
		else
		{
			this->_draw->DrawTriangle(rendList->poly_ptrs[poly]->tvlist[0].x, rendList->poly_ptrs[poly]->tvlist[0].y, rendList->poly_ptrs[poly]->tvlist[1].x, rendList->poly_ptrs[poly]->tvlist[1].y, rendList->poly_ptrs[poly]->tvlist[2].x, rendList->poly_ptrs[poly]->tvlist[2].y, rendList->poly_ptrs[poly]->lit_color[0], (unsigned int *)_videoBuffer, _lPitch);
		}

		/*}
		else
		{
			RECT r;
			r.left = 2;
			r.top = 1;
			r.right = _sp.iWidth - 2;
			r.bottom = _sp.iHeight - 1;

			if(this->_draw->clipLineCS(&r, &rendList->poly_ptrs[poly]->tvlist[0].v, &rendList->poly_ptrs[poly]->tvlist[1].v))
			{
				this->_draw->DrawLine(&rendList->poly_ptrs[poly]->tvlist[0], &rendList->poly_ptrs[poly]->tvlist[1],  rendList->poly_ptrs[poly]->color, this->_videoBuffer, this->_lPitch);
			}

			if(this->_draw->clipLineCS(&r, &rendList->poly_ptrs[poly]->tvlist[1].v, &rendList->poly_ptrs[poly]->tvlist[2].v))
			{
				this->_draw->DrawLine(&rendList->poly_ptrs[poly]->tvlist[1], &rendList->poly_ptrs[poly]->tvlist[2],  rendList->poly_ptrs[poly]->color, this->_videoBuffer, this->_lPitch);
			}

			if(this->_draw->clipLineCS(&r, &rendList->poly_ptrs[poly]->tvlist[2].v, &rendList->poly_ptrs[poly]->tvlist[0].v))
			{
				this->_draw->DrawLine(&rendList->poly_ptrs[poly]->tvlist[2], &rendList->poly_ptrs[poly]->tvlist[0],  rendList->poly_ptrs[poly]->color, this->_videoBuffer, this->_lPitch);
			}
		}*/


		/*if(this->state.checkFlag(RendState::RS_SHOWVERTEX) && (rendList->poly_ptrs[poly]->attr & POLY4D_ATTR_SELECTION))
		{
			RECT r;
			int rSize = this->state.checkFlag(RendState::RS_VERTEXSIZE) * this->_sp.iHeight / this->_sp.iHeight;
			unsigned int iColor;

			r.left = rendList->poly_ptrs[poly]->tvlist[0].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[0].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[0].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[0].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[0].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			if(!(rendList->poly_ptrs[poly]->tvlist[0].attr & VERTEX4DT_ATTR_GENERATED))
				this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);

			r.left = rendList->poly_ptrs[poly]->tvlist[1].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[1].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[1].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[1].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[1].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			if(!(rendList->poly_ptrs[poly]->tvlist[1].attr & VERTEX4DT_ATTR_GENERATED))
				this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);

			r.left = rendList->poly_ptrs[poly]->tvlist[2].x - rSize;
			r.top = rendList->poly_ptrs[poly]->tvlist[2].y - rSize;

			r.right = rendList->poly_ptrs[poly]->tvlist[2].x + rSize;
			r.bottom = rendList->poly_ptrs[poly]->tvlist[2].y + rSize;

			iColor = ARGB32BIT(255,255,255,255);
			if(rendList->poly_ptrs[poly]->tvlist[2].attr & struct3D::VERTEX4DT_ATTR_SELECTED)
				iColor = ARGB32BIT(255,255,0,0);

			if(!(rendList->poly_ptrs[poly]->tvlist[2].attr & VERTEX4DT_ATTR_GENERATED))
				this->_draw->DrawRect(&r, iColor, this->_videoBuffer, this->_lPitch);
		}*/
	}

	//VERTEX4DT v0, v1;

	//ZeroMemory(&v0, sizeof(VERTEX4DT));
	//ZeroMemory(&v1, sizeof(VERTEX4DT));

	//v0.x = 10;
	//v0.y = 100;
	//v0.z = 100;

	//v1.x = 50;
	//v1.y = 10;
	//v1.z = 1000;

	//this->_draw->DrawLine(&v1, &v0, ARGB32BIT(255,0,255,0), this->_videoBuffer, this->_lPitch);

	//POLYF4D p, p2, p3, p4, p5;
	//ZeroMemory(&p, sizeof(POLYF4D));
	//ZeroMemory(&p2, sizeof(POLYF4D));
	//ZeroMemory(&p3, sizeof(POLYF4D));
	//ZeroMemory(&p4, sizeof(POLYF4D));
	//ZeroMemory(&p5, sizeof(POLYF4D));

	//p.tvlist[1].x = 0;
	//p.tvlist[1].y = -100;

	//p.tvlist[2].x = -100;
	//p.tvlist[2].y = 20;

	//p.tvlist[0].x = 50;
	//p.tvlist[0].y = 100;

	//static float angle = 152;

	//TCHAR mas[128];

	///*swprintf(mas, L"%f", angle);
	//MessageBox(0,mas,0,0);*/

	//MATRIX4X4 mrot;

	//mrot.Build_XYZ_Rotation_MATRIX4X4(0,0,angle);

	///*p2.tvlist[0].x = cos(angle * PI) * p.tvlist[0].x + 300;
	//p2.tvlist[0].y = sin(angle * PI) * p.tvlist[0].y + 300;

	//p2.tvlist[1].x = cos(angle * PI) * p.tvlist[1].x + 300;
	//p2.tvlist[1].y = sin(angle * PI) * p.tvlist[1].y + 300;

	//p2.tvlist[2].x = cos(angle * PI) * p.tvlist[2].x + 300;
	//p2.tvlist[2].y = sin(angle * PI) * p.tvlist[2].y + 300;*/

	//vecMulMat(&p.tvlist[0].v, &mrot, &p2.tvlist[0].v);
	//vecMulMat(&p.tvlist[1].v, &mrot, &p2.tvlist[1].v);
	//vecMulMat(&p.tvlist[2].v, &mrot, &p2.tvlist[2].v);

	///*p2.tvlist[0].x += 300;
	//p2.tvlist[0].y += 300;

	//p2.tvlist[1].x += 300;
	//p2.tvlist[1].y += 300;

	//p2.tvlist[2].x += 300;
	//p2.tvlist[2].y += 300;*/

	//p2.tvlist[0].x = 200;
	//p2.tvlist[0].y = 300;
	//p2.tvlist[0].z = 1;

	//p2.tvlist[1].x = 100;
	//p2.tvlist[1].y = 400;
	//p2.tvlist[1].z = 1;

	//p2.tvlist[2].x = 200;
	//p2.tvlist[2].y = 500;
	//p2.tvlist[2].z = 1;

	//p2.lit_color[0] = ARGB32BIT(255,255,0,0);
	//p2.lit_color[1] = ARGB32BIT(255,0,255,0);
	//p2.lit_color[2] = ARGB32BIT(255,0,0,255);

	/*swprintf(mas, L"1: %f %f 2: %f %f 3: %f %f", p2.tvlist[0].x, p2.tvlist[0].y, p2.tvlist[1].x, p2.tvlist[1].y, p2.tvlist[2].x, p2.tvlist[2].y);
	MessageBox(0,mas,0,0);*/

	//angle += 1;

	/*p3.tvlist[0].x = p.tvlist[1].x;
	p3.tvlist[0].y = p.tvlist[1].y;

	p3.tvlist[1].x = p.tvlist[2].x;
	p3.tvlist[1].y = p.tvlist[2].y;

	p3.tvlist[2].x = 250;
	p3.tvlist[2].y = 200;

	p4.tvlist[0].x = p.tvlist[2].x;
	p4.tvlist[0].y = p.tvlist[2].y;

	p4.tvlist[1].x = p3.tvlist[2].x;
	p4.tvlist[1].y = p3.tvlist[2].y;

	p4.tvlist[2].x = 350;
	p4.tvlist[2].y = 200;

	p5.tvlist[0].x = p2.tvlist[1].x;
	p5.tvlist[0].y = p2.tvlist[1].y;

	p5.tvlist[2].x = p2.tvlist[2].x;
	p5.tvlist[2].y = p2.tvlist[2].y;

	p5.tvlist[1].x = 400;
	p5.tvlist[1].y = p2.tvlist[2].y;*/

	//this->_draw->DrawGouraudTriangle3(&p, (unsigned int *)this->_videoBuffer, this->_lPitch);
	//this->_draw->DrawGouraudTriangle3(&p2, (unsigned int *)this->_videoBuffer, this->_lPitch);
	//this->_draw->DrawTriangle(p2.tvlist[0].x, p2.tvlist[0].y, p2.tvlist[1].x, p2.tvlist[1].y, p2.tvlist[2].x, p2.tvlist[2].y, ARGB32BIT(127,255,0,0), (unsigned int *)_videoBuffer, _lPitch);
	//this->_draw->DrawGouraudTriangle3(&p3, (unsigned int *)this->_videoBuffer, this->_lPitch);
	//this->_draw->DrawGouraudTriangle3(&p4, (unsigned int *)this->_videoBuffer, this->_lPitch);
	//this->_draw->DrawGouraudTriangle3(&p5, (unsigned int *)this->_videoBuffer, this->_lPitch);
}

void GraphicImpDirectDraw::DrawOBJECT4DLINE(OBJECT4D_LINE_PTR obj)
{
	obj->vtrans_curr = 0;
	VECTOR2D v1, v2;
	POINT2D p, p2, p3;

	/*vecZero(&obj->vlocal[0].v0.v);
	vecZero(&obj->vlocal[0].v1.v);

	obj->vlocal[0].v0.v.z = 10;
	obj->vlocal[0].v1.v.z = 10;

	obj->vlocal[0].v1.v.x = -1000;*/

	this->pipe.LocalToCameraAndClipOBJECT4DLINE(obj, &this->mainCam);
	this->pipe.CameraToScreenOBJECT4DLINE(obj, &this->mainCam);

	for(int i = 0; i < obj->vtrans_curr; i++)
	{
		//int x0, y0, x1, y1;

		//x0 = obj->vtrans[i].v0.x;
		//y0 = obj->vtrans[i].v0.y;

		//x1 = obj->vtrans[i].v1.x;
		//y1 = obj->vtrans[i].v1.y;

		//if(this->ClipLine(x0, y0, x1, y1, &this->_sp))
		//{
		//	obj->vtrans[i].v0.x = x0;
		//	obj->vtrans[i].v0.y = y0;

		//	obj->vtrans[i].v1.x = x1;
		//	obj->vtrans[i].v1.y = y1;

		RECT r;
		r.left = 2;
		r.top = 1;
		r.right = _sp.iWidth - 2;
		r.bottom = _sp.iHeight - 1;

		if(this->_draw->clipLineCS(&r, &obj->vtrans[i].v0.v, &obj->vtrans[i].v1.v))
		{
			this->_draw->DrawLine(&obj->vtrans[i].v0, &obj->vtrans[i].v1, obj->color, this->_videoBuffer, this->_lPitch);
		}
	}
}

CAM4D_PTR GraphicImpDirectDraw::getMainCamera()
{
	return &mainCam;
}
bool GraphicImpDirectDraw::isOBJECT4DCulled(OBJECT4D_PTR obj)
{
	return pipe.CullOBJECT4D(obj, &mainCam, cullObject::CULL_OBJECT_XYZ_PLANES);
}

int GraphicImpDirectDraw::setRenderState(RendState enm, int value)
{
	this->state.setFlag(enm, value);

	/*switch(enm)
	{
	case RendState::RS_LIGHTING:
		{
			if(value != 0)
				pipe.bLighting = 1;
			else
				pipe.bLighting = 0;
		}break;
	}*/

	return 1;
}

int GraphicImpDirectDraw::AddLight(int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf)
{
	return pipe.lights.InitLightLIGHT(pipe.lights.getNumLights(), state, attr, c_ambient, c_diffuse, c_specular, pos, dir, kc, kl, kq, spot_inner, spot_outer, pf);
}

int GraphicImpDirectDraw::DrawPOLYF4D2D(struct3D::POLYF4D_PTR face)
{
	/*if(!(face->state & struct3D::POLY4D_STATE_ACTIVE) || (face->state & struct3D::POLY4D_STATE_CLIPPED) || (face->state & struct3D::POLY4D_STATE_BACKFACE)) return 0;

		if(face->attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT || face->attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT)
			this->_draw->DrawTriangle(face->tvlist[0].x, face->tvlist[0].y, face->tvlist[1].x, face->tvlist[1].y, face->tvlist[2].x, face->tvlist[2].y, face->lit_color[0], (unsigned int *)_videoBuffer, _lPitch);
		else if(face->attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD)
			this->_draw->DrawGouraudTriangle(face, (unsigned int *)_videoBuffer, _lPitch);*/

	this->_draw->DrawTriangle5(face, (unsigned int *)_videoBuffer, _lPitch);

	return 1;
}

LIGHT_PTR GraphicImpDirectDraw::getLight(int index)
{
	LIGHT_PTR light = 0;

	if(index >= 0 || index < this->pipe.lights.getNumLights())
	{
		light = &this->pipe.lights.lights[index];
	}

	return light;
}

int GraphicImpDirectDraw::DrawTexture(mat::TEXTURE2D_PTR tex, RECT *rect)
{
	
	int lp2 = this->_lPitch >> 2;
	int texlp = tex->lpitch >> 2;
	unsigned int *vb = (unsigned int *)this->_videoBuffer;
	unsigned int *t = (unsigned int *)tex->buffer;

	float texAR;// = tex->width / tex->height;
	float xInc, yInc;
	float fx = 0, fy = 0;

	if(rect != 0)
	{
		if(rect->bottom < rect->right)
		{
			texAR = (float)tex->width / (float)tex->height;
			float newH = rect->bottom;
			float newW = newH * texAR;

			/*TCHAR m[128];
			swprintf(m, L"%f ; %f", texAR, yInc);
			MessageBox(0,m,0,0);*/

			//MessageBox(0,0,0,0);

			xInc = (float)tex->width / newW;
			yInc = (float)tex->height / newH;
		}
		else
		{
			texAR = (float)tex->height / (float)tex->width;
			float newW = rect->right;
			float newH = newW * texAR;

			xInc = (float)tex->width / newW;
			yInc = (float)tex->height / newH;
		}
	}
	else
	{
		xInc = yInc = 1;
	}

	fx = fy = 0;

	for(int y = 0; fy < (float)tex->height; y++)
	{
		fx = 0;
		for(int x = 0; fx < (float)tex->width; x++)
		{
			vb[x + y * lp2] = t[(int)fx + (int)fy * texlp];
			fx += xInc;
		}
		fy += yInc;
	}

	return 1;
}

int GraphicImpDirectDraw::DrawTexTri(struct3D::POLYF4D_PTR face)
{
	if(!face) return 0;

	this->_draw->DrawTriangleTex(face, (unsigned int *)this->_videoBuffer, this->_lPitch);

	return 1;
}

POLYF4D_PTR GraphicImpDirectDraw::getSelectedPoly(int x, int y)
{
	if(x < 0 || y < 0 || x > this->_sp.iWidth || y > this->_sp.iHeight)
		return 0;

	return this->ppObj3DMap[x + y * this->_sp.iWidth];
}

int GraphicImpDirectDraw::DrawCircle(POINT2D *ptCenter, unsigned int iRadius, unsigned int iColor, bool bSmooth)
{
	POINT2D ptCurr[8];// = {0, 0};

	/*float a = 50;
	float b = 20;
	float a_inv = 0;

	a = a*a;
	b = b*b;
	a_inv = 1.0f / a;

	for(float x = 0; x < a; x++)
	{
		ptCurr.y = sqrt(b - ((x * x * b) / a));
		ptCurr.x = x + 20;

		this->DrawPixel(&ptCurr, 1, ARGB32BIT(255,0,255,0));

		ptCurr.x -= x * 2;

		this->DrawPixel(&ptCurr, 1, ARGB32BIT(255,0,255,0));
	}*/

	float R = iRadius;
	float Z = 0, x = 0, y = R;

	if(!bSmooth)
	{
		while(x <= y)
		{
			ptCurr[0].y = ptCenter->y - y;
			ptCurr[0].x = ptCenter->x - x;

			ptCurr[1].y = ptCenter->y + y;
			ptCurr[1].x = ptCenter->x - x;

			ptCurr[2].y = ptCenter->y - y;
			ptCurr[2].x = ptCenter->x + x;

			ptCurr[3].y = ptCenter->y + y;
			ptCurr[3].x = ptCenter->x + x;

			ptCurr[4].y = ptCenter->y - x;
			ptCurr[4].x = ptCenter->x - y;

			ptCurr[5].y = ptCenter->y + x;
			ptCurr[5].x = ptCenter->x - y;

			ptCurr[6].y = ptCenter->y - x;
			ptCurr[6].x = ptCenter->x + y;

			ptCurr[7].y = ptCenter->y + x;
			ptCurr[7].x = ptCenter->x + y;

			this->DrawPixel(ptCurr, 8, iColor);

			if (Z>0) { y--; Z -= y; Z -= y; }

			x++; Z += x; Z += x;
		}
	}
	else
	{
		ptCurr[7].y = ptCurr[7].x = 0;
		for(x = 0; x <= ptCurr[7].y; x++)
		{
			y = sqrt(R*R - x*x);

			ptCurr[0].y = ptCenter->y - y;
			ptCurr[0].x = ptCenter->x - x;

			ptCurr[1].y = ptCenter->y + y;
			ptCurr[1].x = ptCenter->x - x;

			ptCurr[2].y = ptCenter->y - y;
			ptCurr[2].x = ptCenter->x + x;

			ptCurr[3].y = ptCenter->y + y;
			ptCurr[3].x = ptCenter->x + x;

			ptCurr[4].y = ptCenter->y - x;
			ptCurr[4].x = ptCenter->x - y;

			ptCurr[5].y = ptCenter->y + x;
			ptCurr[5].x = ptCenter->x - y;

			ptCurr[6].y = ptCenter->y - x;
			ptCurr[6].x = ptCenter->x + y;

			ptCurr[7].y = ptCenter->y + x;
			ptCurr[7].x = ptCenter->x + y;

			this->DrawPixel(ptCurr, 8, iColor);

			if(x*x >= (R*R - x*x)) break;
		}
	}

	return 1;
}

float factorial (int num)
{
 float result=1;
 for (int i=1; i<=num; ++i)
    result=result*=i;
 return result;
}

int GraphicImpDirectDraw::DrawSpline(std::vector<POINT2D> *pts)
{
	float t = 0;
	POINT2D pt;

	int numPts = pts->size();

	if(numPts > 10 || numPts == 0) return 0;

	float step = 1.0f / 400;

	float *c = new float[numPts];
	int m = numPts - 1;
	float factM = factorial(m);

	for(int i = 0; i < numPts; i++)
	{
		c[i] = factM / (factorial(i) * factorial(m - i));
	}

	for(t = 0; t < 1.0f; t+= step)
	{
		pt.x = 0;
		pt.y = 0;

		for(int i = 0; i < numPts; i++)
		{
			pt.x += c[i] * pow(t, i) * pow((1.0f - t), m - i) * pts->at(i).x;
			pt.y += c[i] * pow(t, i) * pow((1.0f - t), m - i) * pts->at(i).y;
		}

		this->DrawPixel(&pt, 1, ARGB32BIT(255,0,255,0));
	}

	for(int i = 0; i < numPts; i++)
	{
		pt.x = pts->at(i).x;
		pt.y = pts->at(i).y;

		this->DrawPixel(&pt, 1, ARGB32BIT(255,255,0,0));
	}

	delete c;

	return 1;
}

int GraphicImpDirectDraw::DrawSmoothLine(POINT2D *pt0, POINT2D *pt1, unsigned int iColor)
{
	POINT2D ptCurr = {0.0f, 0.0f};

	float dx = pt1->x - pt0->x;
	float dy = pt1->y - pt0->y;

	float interp = 0;

	if(abs(dx) > abs(dy))
	{
		interp = dy / dx;

		if(pt0->x > pt1->x)
		{
			vecSwap(pt0, pt1);
		}

		ptCurr.y = pt0->y;

		for(float x = pt0->x; x < pt1->x; x++)
		{
			ptCurr.x = x;
			ptCurr.y += interp;

			this->DrawPixel(&ptCurr, 1, iColor);
		}
	}
	else
	{
		interp = dx / dy;

		if(pt0->y > pt1->y)
		{
			vecSwap(pt0, pt1);
		}

		ptCurr.x = pt0->x;

		for(float y = pt0->y; y < pt1->y; y++)
		{
			ptCurr.x += interp;
			ptCurr.y = y;

			this->DrawPixel(&ptCurr, 1, iColor);
		}
	}

	return 1;
}

int GraphicImpDirectDraw::point2DTo3D(math3D::POINT2D_PTR pt2D, math3D::POINT4D_PTR pt3D)
{
	float z = 1.0f;//fabs(this->mainCam.pos.z);//600;
	MATRIX4X4 mCamInv;
	POINT4D pTmp;

	matInverse(&this->mainCam.mcam, &mCamInv);
	vecZero(pt3D);

	mCamInv.M30 = mCamInv.M31 = mCamInv.M32 = 0;

	float alpha = (0.5f * this->mainCam.viewport_width/* - 0.5f*/);
	float beta = (0.5f * this->mainCam.viewport_height/* - 0.5f*/);

	//�������������� � �����������������
	pt3D->x = (pt2D->x - alpha) / alpha;
	pt3D->y = (-pt2D->y + beta) / beta;

	//�������������� � ���������� ������
	pt3D->x = pt3D->x / this->mainCam.view_dist * z;
	pt3D->y = pt3D->y / this->mainCam.aspect_ratio / this->mainCam.view_dist * z;
	pt3D->z = z;
	//pt3D->z -= this->mainCam.pos.z;
	//pt3D->z = this->mainCam.pos.z;
	
	//�������������� � �������
	vecMulMat(pt3D, &mCamInv, &pTmp);

	vecCopy(&pTmp, pt3D);

	//pt3D->z = z;//this->mainCam.pos.z;

	vecNormalize(pt3D);

	//��� ������ ������ "������" � ������ ��������� ����.
	//������ ���� ���������� ����������� �������, p0 == ������� ������, � ��������� xz 

	POINT4D p1, p2;
	VECTOR4D n, p;

	vecZero(&n);
	vecZero(&p);

	n.y = 1;

	vecNormalize(&n);

	vecCopy(&this->mainCam.pos, &p1);

	vecAdd(&p1, pt3D, &p2);

	float t = 0;
	float dpVN = (n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z));
	float dpMagic = vecDot(&n, &p1);

	//
	if(dpVN == 0.0f) return 0;
	if((dpVN * dpMagic) > 0.0f) return 0;

	t = (n.x * (p.x - p1.x) + n.y * (p.y - p1.y) + n.z * (p.z - p1.z)) / dpVN;//(n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z));

	//t ����� ��������� ������ ������� ����������� ����� ������ �� ��������� � �������� ������.
	//��� ��� �� ����� ���������� ������ �� ������� ��������� ���������.
	if(t > this->mainCam.far_clip_z /*|| t == numeric_limits<float>::infinity()*/) return 0;

	pt3D->x = p1.x + t * (p2.x - p1.x);
	pt3D->y = p1.y + t * (p2.y - p1.y);
	pt3D->z = p1.z + t * (p2.z - p1.z);

	//!!!
	//pt3D->w = t;
	//!!!

	return 1;
}

int GraphicImpDirectDraw::point2DToVector3D(math3D::POINT2D_PTR pt2D, math3D::VECTOR4D_PTR pt3D)
{
	float z = 1.0f;
	MATRIX4X4 mCamInv;
	POINT4D pTmp;

	matInverse(&this->mainCam.mcam, &mCamInv);
	vecZero(pt3D);

	mCamInv.M30 = mCamInv.M31 = mCamInv.M32 = 0;

	//�������������� � ���������� ������
	pt3D->x = ((pt2D->x - this->mainCam.viewport_center_x) / this->mainCam.view_dist2) * z;
	pt3D->y = ((this->mainCam.viewport_center_y - pt2D->y) / this->mainCam.view_dist2) * z;
	pt3D->z = z;

	//�������������� � �������
	vecMulMat(pt3D, &mCamInv, &pTmp);
	vecCopy(&pTmp, pt3D);

	vecNormalize(pt3D);

	return 1;
}

int GraphicImpDirectDraw::DrawPolygon(std::vector<POINT2D> *pts, unsigned int iColor)
{
	int numPts = pts->size();
	int lpitch = this->_lPitch >> 2;
	unsigned int *vb = (unsigned int *)_videoBuffer;

	if(numPts == 0) return 0;
	if(numPts == 1)
	{
		int x,y;
		x = pts->at(0).x;
		y = pts->at(0).y;
		vb[x + y * lpitch] = iColor;

		return 1;
	}

	for(int i = 0; i < numPts - 1; i++)
	{
		this->DrawLine(pts->at(i).x, pts->at(i).y, pts->at(i + 1).x, pts->at(i + 1).y, iColor);
	}

	if(numPts > 2)
	{
		this->DrawLine(pts->at(0).x, pts->at(0).y, pts->at(numPts - 1).x, pts->at(numPts - 1).y, iColor);
	}

	return 1;
}

int GraphicImpDirectDraw::createShadowProjection(OBJECT4D_PTR obj, POLYF4D_PTR proj, int numLines, int *createdLines)
{
	PLANE4D plane;
	VECTOR4D np;
	POINT4D p0;
	POINT4D pProj;
	const int shadowAttr = mat::MAT_ATTR_2SIDED | struct3D::POLY4D_ATTR_SHADE_MODE_PURE;

	vecZero(&np);
	vecZero(&p0);

	*createdLines = 0;

	int cl = 0;
	int isTriCorrect = 0;

	np.y = 1;
	p0.y = -500;

	planeInit(&plane, &p0, &np, 1);

	vecZero(&np);
	vecZero(&p0);

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4D_PTR currPoly = &obj->plist[poly];

		int vindex0 = currPoly->vert[0];
		int vindex1 = currPoly->vert[1];
		int vindex2 = currPoly->vert[2];

		VECTOR4D u, v, n;

		u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
		v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

		u.VECTOR4D_Cross(&v, &n);

		VECTOR4D view, vecShProj;

		for(int light = 0; light < this->pipe.lights.getNumLights(); light++)
		{
			if(pipe.lights.lights[light].attr & mat::LIGHT_ATTR_POINT)
			{

				view.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &pipe.lights.lights[light].pos);

				float dp = n.VECTOR4D_Dot(&view);

				if(dp < 0.0f)
				{

					isTriCorrect = 0;
						for(int vertex = 0; vertex < 3; vertex++)
						{
							vecBuild(&pipe.lights.lights[light].pos, &obj->vlist_trans[currPoly->vert[vertex]].v, &vecShProj);

							if(planeGetIntersectPoint(&plane, &pipe.lights.lights[light].pos, &vecShProj, &pProj))
							{
								vecCopy(&pProj, &proj[cl].tvlist[vertex].v);
								isTriCorrect++;
							}
						}
						if(isTriCorrect == 3)
							{
								proj[cl].attr = shadowAttr;
								proj[cl].state = struct3D::POLY4D_STATE_ACTIVE;
								proj[cl].lit_color[0] = ARGB32BIT(255,255,255,255);
								proj[cl].color = ARGB32BIT(255,255,255,255);
								cl++;
							}
						if(cl == numLines)
							{
								*createdLines = numLines;
								return 1;
							}
							*createdLines = cl;
					}
			}
		}
	}

	return 1;
}

int GraphicImpDirectDraw::DrawRect(RECT *r, unsigned int iColor)
{
	return this->_draw->DrawRect(r, iColor, this->_videoBuffer, this->_lPitch);
}

void GraphicImpDirectDraw::clearZBuffer()
{
	this->_draw->clearZBuffer();
}

int GraphicImpDirectDraw::Resize(int newWidth, int newHeight)
{
	/*if(!this->DrawEnd())
	{
		MessageBox(0, L"Error in GraphicImpDirectDraw::Resize(int newWidth, int newHeight)",0,0);
		return 0;
	}*/

	this->_sp.iWidth = newWidth;
	this->_sp.iHeight = newHeight;

	isBufferLocked = 0;

	/*if(this->_sp.iFullScreen){
		if(FAILED(_lpdd7->SetCooperativeLevel(_hWnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))) return 0;
		if(FAILED(_lpdd7->SetDisplayMode(this->_sp.iWidth, this->_sp.iHeight, this->_sp.iBitsPerPixel, 0, 0))) return 0;
	}
	else{
		if(FAILED(_lpdd7->SetCooperativeLevel(_hWnd, DDSCL_NORMAL))) return 0;
	}*/

	ZeroMemory(&_ddsd, sizeof(_ddsd));
	_ddsd.dwSize = sizeof(_ddsd);

	if(this->_lpddsurf)
	{
		this->_lpddsurf->Release();
		this->_lpddsurf = 0;
	}

	if(this->_sp.iFullScreen){
		_ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_SYSTEMMEMORY;
		_ddsd.dwBackBufferCount = 1;
	}
	else{
		_ddsd.dwFlags = DDSD_CAPS;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	if(this->_lpddbacksurf)
	{
		this->_lpddbacksurf->Release();
		this->_lpddbacksurf = 0;
	}

	if(this->_lpddclip)
	{
		this->_lpddclip->Release();
		this->_lpddclip = 0;
	}
	if(this->_flipStrategy)
	{
		delete this->_flipStrategy;
		this->_flipStrategy = 0;
	}
	if(this->_draw)
	{
		this->_draw->Release();
		delete this->_draw;
		this->_draw = 0;
	}

	if(FAILED(_lpdd7->CreateSurface(&_ddsd,&_lpddsurf,NULL))) return 0;

	if(this->_sp.iFullScreen){
		DDSCAPS2 ddsc;
		ZeroMemory(&ddsc, sizeof(ddsc));
		ddsc.dwCaps = DDSCAPS_BACKBUFFER;
		if(FAILED(_lpddsurf->GetAttachedSurface(&ddsc, &_lpddbacksurf))) return 0;

		_flipStrategy = new FlipFullscreenDDStrategy(_lpddsurf);
		SetWindowLong(_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	}
	else{
		ZeroMemory(&_ddsd, sizeof(_ddsd));
		_ddsd.dwSize = sizeof(_ddsd);

		_ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		_ddsd.dwWidth = this->_sp.iWidth;
		_ddsd.dwHeight = this->_sp.iHeight;
		_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

		if(FAILED(_lpdd7->CreateSurface(&_ddsd, &_lpddbacksurf, NULL))) return 0;

		if(FAILED(_lpdd7->CreateClipper(0, &_lpddclip, 0))) return 0;
		if(FAILED(_lpddclip->SetHWnd(0, _hWnd))) return 0;
		if(FAILED(_lpddsurf->SetClipper(_lpddclip))) return 0;

		_flipStrategy = new FlipWindowedDDStrategy(_lpddsurf, _lpddbacksurf, _hWnd, _sp.iWidth, _sp.iHeight);
	}

	/*this->ppObj3DMap = 0;
	this->ppObj3DMap = new POLYF4D_PTR[params->iWidth * params->iHeight];
	memset(this->ppObj3DMap, 0, sizeof(POLYF4D_PTR) * (params->iWidth * params->iHeight));*/
	if(!InitializeDrawStrategies(&this->_sp)) return 0;

	//3DInit:
	POINT4D camPos = {0, 0, 0, 1};
	VECTOR4D camDir = {0, 0, 0, 1};
	POINT4D target = {0, 0, 1, 1};

	mainCam.Init_CAM4D(struct3D::CAM_MODEL_EULER, &camPos, &camDir, &target, 10.0f, 100000.0f, 90.0f, this->_sp.iWidth, this->_sp.iHeight);

	return 1;
}

void GraphicImpDirectDraw::testAdj(POINT2D_PTR pt, OBJECT4D_PTR obj)
{
	for(int i = 0; i < obj->num_polys; i++)
	{
		obj->plist[i].attr &= ~POLY4D_ATTR_SELECTED;
		obj->plist[i].color = ARGB32BIT(255,0,255,0);
	}

	POINT4D p, v;

	//vecZero(&p);
	//vecZero(&v);

	//v.z = 1;

	for(int i = 0; i < obj->num_polys; i++)
	{
		vecSub(&this->mainCam.pos, &obj->world_pos, &p);

		if(this->isInPoly(&p, &this->mainCam._look, &obj->plist[i], obj))
		{
			obj->plist[i].attr |= POLY4D_ATTR_SELECTED;
			
			obj->plist[obj->adj[i].adj[0]].color = ARGB32BIT(255,255,255,255);
			obj->plist[obj->adj[i].adj[1]].color = ARGB32BIT(255,0,0,255);
			obj->plist[obj->adj[i].adj[2]].color = ARGB32BIT(255,255,0,255);
		}
	}
}

bool GraphicImpDirectDraw::isInPoly(POINT4D_PTR p, VECTOR4D_PTR v, POLY4D_PTR poly, OBJECT4D_PTR obj)
{
	int vindex0 = poly->vert[0];
	int vindex1 = poly->vert[1];
	int vindex2 = poly->vert[2];

	VECTOR4D u, vt, n;

	u.VECTOR4D_Build(&obj->vlist_local[vindex0].v, &obj->vlist_local[vindex1].v);
	vt.VECTOR4D_Build(&obj->vlist_local[vindex0].v, &obj->vlist_local[vindex2].v);

	u.VECTOR4D_Cross(&vt, &n);

	VECTOR4D view;

	view.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, p);

	float dp = n.VECTOR4D_Dot(&view);

	if(dp > 0.0f)
	{
		PLANE4D plane;
		POINT4D pRes;

		planeInit(&plane, &obj->vlist_local[vindex0].v, &n, 1);

		if(planeGetIntersectPoint(&plane, p, v, &pRes))
		{
			MATRIX4X4 mat;
			VECTOR4D v1, v2, v3, vTmp;
			VECTOR4D vt1, vt2, vt3;

			matAxisRotation(&mat, &n, 90);

			vecBuild(&obj->vlist_local[vindex0].v, &obj->vlist_local[vindex1].v, &v1);
			vecBuild(&obj->vlist_local[vindex1].v, &obj->vlist_local[vindex2].v, &v2);
			vecBuild(&obj->vlist_local[vindex2].v, &obj->vlist_local[vindex0].v, &v3);

			vecMulMat(&v1, &mat, &vTmp);
			vecCopy(&vTmp, &v1);

			vecMulMat(&v2, &mat, &vTmp);
			vecCopy(&vTmp, &v2);

			vecMulMat(&v3, &mat, &vTmp);
			vecCopy(&vTmp, &v3);

			int hit = 0;

			//����� �� ���������� ����� � ������������� ����������������

			vecBuild(&pRes, &obj->vlist_local[vindex0].v, &vt1);
			if(vecDot(&vt1, &v1) > 0.0f) hit++;

			vecBuild(&pRes, &obj->vlist_local[vindex1].v, &vt2);
			if(vecDot(&vt2, &v2) > 0.0f) hit++;

			vecBuild(&pRes, &obj->vlist_local[vindex2].v, &vt3);
			if(vecDot(&vt3, &v3) > 0.0f) hit++;

			//TCHAR mas[128];
			//swprintf(mas, L"%d", hit);
			//MessageBox(0,mas,0,0);

			if(hit == 3)
				return true;
			else
				return false;
		}
	}

	return false;
}