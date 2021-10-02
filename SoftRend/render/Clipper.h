#pragma once
#include <vector>
#include "structures.h"

class Clipper
{
public:
	Clipper(void);
	~Clipper(void);

	int clipCohenSutherland(RECT *clippingRect, std::vector<POINT2D> *subjPoly, std::vector<POINT2D> *resPoly, std::vector<POINT2D> *clippedParts);
	int clipWeilerAtherton(std::vector<POINT2D> *clipPoly, std::vector<POINT2D> *subjPoly, std::vector<POINT2D> *resPoly, std::vector<POINT2D> *clippedParts);
private:
	int clipLineCS(RECT *clippingRect, POINT2D *pt1, POINT2D *pt2, int pcode1, int pcode2);
};

