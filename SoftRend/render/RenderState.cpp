#include "StdAfx.h"
#include "RenderState.h"


RenderState::RenderState(void)
{
	ZeroMemory(this, sizeof(RendState));
}


RenderState::~RenderState(void)
{
}

int RenderState::checkFlag(RendState enm)
{
	switch(enm)
	{
	case RendState::RS_VERTEXSIZE:
		{
			return this->vertSize;
		}break;
	}

	return (unsigned int)enm & this->flags1;
}
int RenderState::setFlag(RendState enm, int value)
{
	switch(enm)
	{
	case RendState::RS_VERTEXSIZE:
		{
			this->vertSize = abs(value) % 100;
			return 1;
		}
	}

	if(value != 0)
		this->flags1 |= (unsigned int)enm;
	else this->flags1 &= ~((unsigned int)enm);

	return 1;
}

int RenderState::reset()
{
	ZeroMemory(this, sizeof(RendState));

	return 1;
}