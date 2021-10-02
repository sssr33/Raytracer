#pragma once

enum RendState
{
	RS_LIGHTING = 0x00000001, 
	RS_SHOWVERTEX = 0x000000002, RS_VERTEXSIZE,
	RS_BACKFACEREMOVE = 0x00000004,
	RS_WIREFRAME = 0x00000008
};


class RenderState
{
public:
	RenderState(void);
	~RenderState(void);
	int checkFlag(RendState enm);
	int setFlag(RendState enm, int value);
	int reset();
private:
	unsigned int flags1;
	unsigned int vertSize;
};