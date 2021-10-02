#include "StdAfx.h"
#include "Clipper.h"


Clipper::Clipper(void)
{
}


Clipper::~Clipper(void)
{
}

int Clipper::clipCohenSutherland(RECT *clippingRect, std::vector<POINT2D> *subjPoly, std::vector<POINT2D> *resPoly, std::vector<POINT2D> *clippedParts)
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

	int minX = clippingRect->left;
	int minY = clippingRect->top;

	int maxX = clippingRect->right;
	int maxY = clippingRect->bottom;

	int pcode1, pcode2;
	POINT2D pRes1, pRes2;

	int numPts = subjPoly->size();

	if(numPts < 1) return 0;

	clippedParts->resize(0);
	resPoly->resize(0);

	for(int i = 0; i < numPts - 1; i++)
	{
		pcode1 = pcode2 = 0;

		pRes1.x = subjPoly->at(i).x;
		pRes1.y = subjPoly->at(i).y;

		pRes2.x = subjPoly->at(i + 1).x;
		pRes2.y = subjPoly->at(i + 1).y;

		if(pRes1.x < minX) pcode1 |= CLIP_CODE_W;
		else if(pRes1.x > maxX) pcode1 |= CLIP_CODE_E;

		if(pRes1.y < minY) pcode1 |= CLIP_CODE_N;
		else if(pRes1.y > maxY) pcode1 |= CLIP_CODE_S;

		if(pRes2.x < minX) pcode2 |= CLIP_CODE_W;
		else if(pRes2.x > maxX) pcode2 |= CLIP_CODE_E;

		if(pRes2.y < minY) pcode2 |= CLIP_CODE_N;
		else if(pRes2.y > maxY) pcode2 |= CLIP_CODE_S;

		if(pcode1 & pcode2) 
		{
			clippedParts->push_back(subjPoly->at(i));
			clippedParts->push_back(subjPoly->at(i + 1));
			continue;
		}
		if(pcode1 == 0 && pcode2 == 0)
		{
			resPoly->push_back(pRes1);
			resPoly->push_back(pRes2);

			continue;
		}

		if(clipLineCS(clippingRect, &pRes1, &pRes2, pcode1, pcode2))
		{
			resPoly->push_back(pRes1);
			resPoly->push_back(pRes2);

			clippedParts->push_back(subjPoly->at(i));
			clippedParts->push_back(pRes1);
			clippedParts->push_back(subjPoly->at(i + 1));
			clippedParts->push_back(pRes2);
		}
		else
		{
			clippedParts->push_back(subjPoly->at(i));
			clippedParts->push_back(subjPoly->at(i + 1));
		}
	}

	//отсечение линии от последней точки к начальной
	if(numPts > 2)
	{
	pcode1 = pcode2 = 0;

	pRes1.x = subjPoly->at(numPts - 1).x;
	pRes1.y = subjPoly->at(numPts - 1).y;

	pRes2.x = subjPoly->at(0).x;
	pRes2.y = subjPoly->at(0).y;

	if(pRes1.x < minX) pcode1 |= CLIP_CODE_W;
	else if(pRes1.x > maxX) pcode1 |= CLIP_CODE_E;

	if(pRes1.y < minY) pcode1 |= CLIP_CODE_N;
	else if(pRes1.y > maxY) pcode1 |= CLIP_CODE_S;

	if(pRes2.x < minX) pcode2 |= CLIP_CODE_W;
	else if(pRes2.x > maxX) pcode2 |= CLIP_CODE_E;

	if(pRes2.y < minY) pcode2 |= CLIP_CODE_N;
	else if(pRes2.y > maxY) pcode2 |= CLIP_CODE_S;

	if(pcode1 & pcode2) 
	{
		clippedParts->push_back(subjPoly->at(numPts - 1));
		clippedParts->push_back(subjPoly->at(0));
		return 1;
	}
	if(pcode1 == 0 && pcode2 == 0)
	{
		resPoly->push_back(pRes1);
		resPoly->push_back(pRes2);

		return 1;
	}

	if(clipLineCS(clippingRect, &pRes1, &pRes2, pcode1, pcode2))
	{
		resPoly->push_back(pRes1);
		resPoly->push_back(pRes2);

		clippedParts->push_back(subjPoly->at(numPts - 1));
		clippedParts->push_back(pRes1);
		clippedParts->push_back(subjPoly->at(0));
		clippedParts->push_back(pRes2);
	}
	else
	{
		clippedParts->push_back(subjPoly->at(numPts - 1));
		clippedParts->push_back(subjPoly->at(0));
	}
	}

	return 1;
}

int Clipper::clipLineCS(RECT *clippingRect, POINT2D *pt1, POINT2D *pt2, int pcode1, int pcode2)
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
	
	float minX = clippingRect->left;
	float minY = clippingRect->top;

	float maxX = clippingRect->right;
	float maxY = clippingRect->bottom;

	float t = 0;

	POINT2D ptc1;
	POINT2D ptc2;



	switch(pcode1)
	{
	case CLIP_CODE_C: 
		{
			ptc1.x = pt1->x;
			ptc1.y = pt1->y;
		}break;
	case CLIP_CODE_N:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
		}break;
	case CLIP_CODE_S:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
		}break;
	case CLIP_CODE_E:
		{
			t = (maxX - pt1->x) / (pt2->x - pt1->x);
			ptc1.x = maxX;
			ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
		}break;
	case CLIP_CODE_W:
		{
			t = (minX - pt1->x) / (pt2->x - pt1->x);
			ptc1.x = minX;
			ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
		}break;
	case CLIP_CODE_NE:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (maxX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = maxX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			}
		}break;
	case CLIP_CODE_SE:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (maxX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = maxX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			}
		}break;
	case CLIP_CODE_NW:
		{
			t = (minY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = minY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (minX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = minX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			}
		}break;
	case CLIP_CODE_SW:
		{
			t = (maxY - pt1->y) / (pt2->y - pt1->y);
			ptc1.y = maxY;
			ptc1.x = pt1->x + (pt2->x - pt1->x) * t;
			if(ptc1.x < minX || ptc1.x > maxX)
			{
				t = (minX - pt1->x) / (pt2->x - pt1->x);
				ptc1.x = minX;
				ptc1.y = pt1->y + (pt2->y - pt1->y) * t;
			}
		}break;
	default: break;
	}

	switch(pcode2)
	{
	case CLIP_CODE_C: 
		{
			ptc2.x = pt2->x;
			ptc2.y = pt2->y;
		}break;
	case CLIP_CODE_N:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
		}break;
	case CLIP_CODE_S:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
		}break;
	case CLIP_CODE_E:
		{
			t = (maxX - pt2->x) / (pt1->x - pt2->x);
			ptc2.x = maxX;
			ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
		}break;
	case CLIP_CODE_W:
		{
			t = (minX - pt2->x) / (pt1->x - pt2->x);
			ptc2.x = minX;
			ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
		}break;
	case CLIP_CODE_NE:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (maxX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = maxX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			}
		}break;
	case CLIP_CODE_SE:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (maxX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = maxX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			}
		}break;
	case CLIP_CODE_NW:
		{
			t = (minY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = minY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (minX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = minX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			}
		}break;
	case CLIP_CODE_SW:
		{
			t = (maxY - pt2->y) / (pt1->y - pt2->y);
			ptc2.y = maxY;
			ptc2.x = pt2->x + (pt1->x - pt2->x) * t;
			if(ptc2.x < minX || ptc2.x > maxX)
			{
				t = (minX - pt2->x) / (pt1->x - pt2->x);
				ptc2.x = minX;
				ptc2.y = pt2->y + (pt1->y - pt2->y) * t;
			}
		}break;
	default: break;
	}

	if((ptc1.x < minX) || (ptc1.x > maxX) || (ptc1.y  < minY) || (ptc1.y > maxY) || (ptc2.x  < minX) || (ptc2.x > maxX) || (ptc2.y < minY) || (ptc2.y > maxY)) 
	{
		return 0;
	}

	pt1->x = ptc1.x;
	pt2->x = ptc2.x;

	pt1->y = ptc1.y;
	pt2->y = ptc2.y;

	return 1;
}

int Clipper::clipWeilerAtherton(std::vector<POINT2D> *clipPoly, std::vector<POINT2D> *subjPoly, std::vector<POINT2D> *resPoly, std::vector<POINT2D> *clippedParts)
{
	resPoly->resize(0);
	clippedParts->resize(0);

	return 1;
}
