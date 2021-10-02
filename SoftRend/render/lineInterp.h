#include "structures.h"
#include "stdafx.h"

using namespace struct3D;
using namespace mat;

class lineInterp
{
public:
	lineInterp(void);
	~lineInterp(void);

	void init(POINT4D_PTR p1, POINT4D_PTR p2, bool isYSorted = false);
	float getX(float y_idx);

	POINT4D p1, p2;
	float height;
	float heightF;
	float currT;
	float dxdy;
	bool xinv;
};

