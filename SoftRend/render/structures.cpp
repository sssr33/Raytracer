#include "StdAfx.h"
#include "structures.h"
#include <math.h>

void struct3D::RENDERLIST4D_TYP::Reset_RENDERLIST4D()
{
	this->num_polys = 0;
	this->num_objects = 0;
}

int struct3D::RENDERLIST4D_TYP::Insert_POLYF4D_RENDERLIST4D(POLYF4D_PTR poly)
{
	if (this->num_polys >= RENDERLIST4D_MAX_POLYS)
		return(0);

	// step 1: copy polygon into next opening in polygon render list

	// point pointer to polygon structure
	this->poly_ptrs[this->num_polys] = &this->poly_data[this->num_polys];
	
	// copy face right into array, thats it
	memcpy(&this->poly_data[this->num_polys],poly, sizeof(POLYF4D));

	// now the polygon is loaded into the next free array position, but
	// we need to fix up the links
	// test if this is the first entry
	if (this->num_polys == 0)
	   {
	   // set pointers to null, could loop them around though to self
	   this->poly_data[0].next = NULL;
	   this->poly_data[0].prev = NULL;
	   } // end if
	else
	   {
	   // first set this node to point to previous node and next node (null)
	   this->poly_data[this->num_polys].next = NULL;
	   this->poly_data[this->num_polys].prev = 
			 &this->poly_data[this->num_polys-1];

	   // now set previous node to point to this node
	   this->poly_data[this->num_polys-1].next = 
			  &this->poly_data[this->num_polys];
	   } // end else

	// increment number of polys in list
	this->num_polys++;

	// return successful insertion
	return(1);
}

int struct3D::RENDERLIST4D_TYP::Insert_POLY4D_RENDERLIST4D(POLY4D_PTR poly, bool selection)
{
	// step 0: are we full?
if (this->num_polys >= RENDERLIST4D_MAX_POLYS)
   return(0);

// step 1: copy polygon into next opening in polygon render list

// point pointer to polygon structure
this->poly_ptrs[this->num_polys] = &this->poly_data[this->num_polys];

// copy fields
this->poly_data[this->num_polys].state = poly->state;
this->poly_data[this->num_polys].attr  = poly->attr;
this->poly_data[this->num_polys].color = poly->color;
this->poly_data[this->num_polys].nlength = poly->nlength;
this->poly_data[this->num_polys].texture = poly->texture;
this->poly_data[this->num_polys].objID = num_objects;

this->poly_data[this->num_polys].lit_color[0] = poly->lit_color[0];
this->poly_data[this->num_polys].lit_color[1] = poly->lit_color[1];
this->poly_data[this->num_polys].lit_color[2] = poly->lit_color[2];

if(selection)
	this->poly_data[this->num_polys].attr |= POLY4D_ATTR_SELECTION;
// now copy vertices, be careful! later put a loop, but for now
// know there are 3 vertices always!
//poly->vlist[poly->vert[0]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].tvlist[0].v);
//poly->vlist[poly->vert[1]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].tvlist[1].v);
//poly->vlist[poly->vert[2]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].tvlist[2].v);
//
//// and copy into local vertices too
//poly->vlist[poly->vert[0]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].vlist[0].v);
//poly->vlist[poly->vert[1]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].vlist[1].v);
//poly->vlist[poly->vert[2]].v.VECTOR4D_COPY(&this->poly_data[this->num_polys].vlist[2].v);
poly->vlist[poly->vert[0]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].tvlist[0]);
poly->vlist[poly->vert[1]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].tvlist[1]);
poly->vlist[poly->vert[2]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].tvlist[2]);

poly->vlist[poly->vert[0]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].vlist[0]);
poly->vlist[poly->vert[1]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].vlist[1]);
poly->vlist[poly->vert[2]].VERTEX4DT_COPY(&this->poly_data[this->num_polys].vlist[2]);

memcpy(&this->poly_data[this->num_polys].vert, poly->vert, sizeof(int) * 3);

/*this->poly_data[this->num_polys].tvlist[0].t = poly->tlist[ poly->text[0] ];
this->poly_data[this->num_polys].tvlist[1].t = poly->tlist[ poly->text[1] ];
this->poly_data[this->num_polys].tvlist[2].t = poly->tlist[ poly->text[2] ];

this->poly_data[this->num_polys].vlist[0].t = poly->tlist[ poly->text[0] ];
this->poly_data[this->num_polys].vlist[1].t = poly->tlist[ poly->text[1] ];
this->poly_data[this->num_polys].vlist[2].t = poly->tlist[ poly->text[2] ];*/

// now the polygon is loaded into the next free array position, but
// we need to fix up the links

// test if this is the first entry
if (this->num_polys == 0)
   {
   // set pointers to null, could loop them around though to self
   this->poly_data[0].next = NULL;
   this->poly_data[0].prev = NULL;
   } // end if
else
   {
   // first set this node to point to previous node and next node (null)
   this->poly_data[this->num_polys].next = NULL;
   this->poly_data[this->num_polys].prev = 
         &this->poly_data[this->num_polys-1];

   // now set previous node to point to this node
   this->poly_data[this->num_polys-1].next = 
          &this->poly_data[this->num_polys];
   } // end else

// increment number of polys in list
this->num_polys++;

// return successful insertion
return(1);

}

int struct3D::RENDERLIST4D_TYP::Insert_OBJECT4D_RENDERLIST4D(OBJECT4D_PTR obj, bool selection, int insert_local)
{
	if (!(obj->state & OBJECT4D_STATE_ACTIVE) ||
     (obj->state & OBJECT4D_STATE_CULLED) ||
     !(obj->state & OBJECT4D_STATE_VISIBLE))
   return(0); 

	//this->objRefs[this->num_objects] = obj;

// the object is valid, let's rip it apart polygon by polygon
for (int poly = 0; poly < obj->num_polys; poly++)
    {
    // acquire polygon
    POLY4D_PTR curr_poly = &obj->plist[poly];

    // first is this polygon even visible?
    if (!(curr_poly->state & POLY4D_STATE_ACTIVE) ||
         (curr_poly->state & POLY4D_STATE_CLIPPED ) ||
         (curr_poly->state & POLY4D_STATE_BACKFACE) )
    continue; // move onto next poly

    // override vertex list polygon refers to
    // the case that you want the local coords used
    // first save old pointer
    VERTEX4DT_PTR vlist_old = curr_poly->vlist;

    if (insert_local)
       curr_poly->vlist = obj->vlist_local;
    else
	{	
       curr_poly->vlist = obj->vlist_trans;
	}

	curr_poly->texture = obj->texture;

    // now insert this polygon
    if (!Insert_POLY4D_RENDERLIST4D(curr_poly, selection))
       {
       // fix vertex list pointer
       curr_poly->vlist = vlist_old;
   
       // the whole object didn't fit!
       return(0);
       } // end if

    // fix vertex list pointer
    curr_poly->vlist = vlist_old;

    } // end for

	this->num_objects++;

// return success
return(1);
}

void struct3D::RENDERLIST4D_TYP::TransformRENDERLIST4D(MATRIX4X4_PTR mt, coordSelect enm)
{
	switch(enm)
	{
	case coordSelect::TRANSFORM_LOCAL_ONLY:
		{
			for(int poly = 0; poly < this->num_polys; poly++)
			{
				struct3D::POLYF4D_PTR currPoly = this->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					POINT4D pResult;

					currPoly->vlist[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);

					pResult.VECTOR4D_COPY(&currPoly->vlist[vertex].v);
				}
			}
		}break;
	case coordSelect::TRANSFORM_TRANS_ONLY:
		{
			for(int poly = 0; poly < this->num_polys; poly++)
			{
				struct3D::POLYF4D_PTR currPoly = this->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					POINT4D pResult;

					currPoly->tvlist[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);

					pResult.VECTOR4D_COPY(&currPoly->tvlist[vertex].v);
				}
			}
		}break;
	case coordSelect::TRANSFORM_LOCAL_TO_TRANS:
		{
			for(int poly = 0; poly < this->num_polys; poly++)
			{
				struct3D::POLYF4D_PTR currPoly = this->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					currPoly->vlist[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &currPoly->tvlist[vertex].v);
				}
			}
		}break;
	default: break;
	}
}

int struct3D::RENDERLIST4D_TYP::selectionVertex(RECT *r, VERTEX4DT_PTR *verts, int *num)
{
	int temp;
	*num = 0;
	struct3D::POLYF4D_PTR currPoly;
	OBJECT4D_PTR obj = nullptr;

	if(r->left > r->right) SWAP(r->left, r->right, temp);
	if(r->top > r->bottom) SWAP(r->top, r->bottom, temp);

	for(int poly = 0; poly < this->num_polys; poly++)
	{
		currPoly = this->poly_ptrs[poly];
		/*obj = this->objRefs[currPoly->objID];*/
		obj->vlist_local[currPoly->vert[0]].attr &= ~struct3D::VERTEX4DT_ATTR_SELECTED;
		obj->vlist_local[currPoly->vert[1]].attr &= ~struct3D::VERTEX4DT_ATTR_SELECTED;
		obj->vlist_local[currPoly->vert[2]].attr &= ~struct3D::VERTEX4DT_ATTR_SELECTED;
	}

	for(int poly = 0; poly < this->num_polys; poly++)
	{
		currPoly = this->poly_ptrs[poly];

		/*if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
			continue;*/

		for(int vertex = 0; vertex < 3; vertex++)
		{
			/*obj = this->objRefs[currPoly->objID];*/

			if(obj->vlist_local[currPoly->vert[vertex]].attr & struct3D::VERTEX4DT_ATTR_SELECTED) continue;

			float x,y;

			x = obj->scr_pts[currPoly->vert[vertex]].x;
			y = obj->scr_pts[currPoly->vert[vertex]].y;

			if(x > r->left && x < r->right && y > r->top && y < r->bottom)
			{	
				obj->vlist_local[currPoly->vert[vertex]].attr |= struct3D::VERTEX4DT_ATTR_SELECTED;

				verts[(*num)] = &obj->vlist_local[currPoly->vert[vertex]];
				(*num)++;
			}
			else
			{
				obj->vlist_local[currPoly->vert[vertex]].attr &= ~struct3D::VERTEX4DT_ATTR_SELECTED;
			}
		}
	}

	return 1;
}

float struct3D::OBJECT4D_TYP::Compute_OBJECT4D_Radius()
{
	this->avg_radius[this->curr_frame] = 0;
	this->max_radius[this->curr_frame] = 0;

	for (int vertex = 0; vertex < this->num_vertices; vertex++)
    {
		float dist_to_vertex = 
			  sqrt(this->vlist_local[vertex].x * this->vlist_local[vertex].x +
				   this->vlist_local[vertex].y * this->vlist_local[vertex].y +
				   this->vlist_local[vertex].z * this->vlist_local[vertex].z);

		this->avg_radius[this->curr_frame]+=dist_to_vertex;
 
		if (dist_to_vertex > this->max_radius[this->curr_frame])
		   this->max_radius[this->curr_frame] = dist_to_vertex; 
    }

	this->avg_radius[this->curr_frame] /= this->num_vertices;

	return(this->max_radius[this->curr_frame]);
}

void struct3D::OBJECT4D_TYP::ComputeBBox()
{
	vecCopy(&this->world_pos, &this->boundingBox.c);
	vecZero(&this->boundingBox.r);

	this->boundingBox.r.x = this->boundingBox.r.y = this->boundingBox.r.z = FLT_MIN;

	for(int i = 0; i < this->num_vertices; i++)
	{
		float x = fabs(this->vlist_local[i].x);
		float y = fabs(this->vlist_local[i].y);
		float z = fabs(this->vlist_local[i].z);
		if(x > this->boundingBox.r.x) this->boundingBox.r.x = x;
		if(y > this->boundingBox.r.y) this->boundingBox.r.y = y;
		if(z > this->boundingBox.r.z) this->boundingBox.r.z = z;
	}
}

void struct3D::OBJECT4D_TYP::ComputeAdjacency()
{
	if(this->adj)
		delete adj;

	this->adj = new AdjacencyPoly[this->num_polys];

	memset(adj, -1, sizeof(AdjacencyPoly) * this->num_polys);

	const int e0_1 = 1, e1_2 = 2, e2_0 = 4, eAll = 7;
	int adjRes = 0;

	int idx[4] = {0, 1, 2, 0};
	
	for(int i = 0; i < this->num_polys; i++)
	{
		adjRes = 0;

		for(int j = 0; j < this->num_polys; j++)
		{
			if(i == j) continue;

			/*for(int o = 0; o < 3; o++)
				for(int k = 0; k < 3; k++)
				{
					if((this->plist[i].vert[idx[o]] == this->plist[j].vert[idx[k]]) && (this->plist[i].vert[idx[o + 1]] == this->plist[j].vert[idx[k + 1]]))
					{
						this->adj[i].adj[o] = j;
						tst++;
					}
					else if((this->plist[i].vert[idx[o]] == this->plist[j].vert[idx[k + 1]]) && (this->plist[i].vert[idx[o + 1]] == this->plist[j].vert[idx[k]]))
					{
						this->adj[i].adj[o] = j;
						tst++;
					}
				}*/

			if(!(adjRes & e0_1))
			{
				for(int k = 0; k < 3; k++)
				{
					if((this->plist[i].vert[0] == this->plist[j].vert[idx[k]]) && (this->plist[i].vert[idx[1]] == this->plist[j].vert[idx[k + 1]]))
					{
						this->adj[i].adj[0] = j;
						adjRes |= e0_1;
					}
					else if((this->plist[i].vert[0] == this->plist[j].vert[idx[k + 1]]) && (this->plist[i].vert[idx[1]] == this->plist[j].vert[idx[k]]))
					{
						this->adj[i].adj[0] = j;
						adjRes |= e0_1;
					}
				}
			}

			if(!(adjRes & e1_2))
			{
				for(int k = 0; k < 3; k++)
				{
					if((this->plist[i].vert[1] == this->plist[j].vert[idx[k]]) && (this->plist[i].vert[idx[2]] == this->plist[j].vert[idx[k + 1]]))
					{
						this->adj[i].adj[1] = j;
						adjRes |= e1_2;
					}
					else if((this->plist[i].vert[1] == this->plist[j].vert[idx[k + 1]]) && (this->plist[i].vert[idx[2]] == this->plist[j].vert[idx[k]]))
					{
						this->adj[i].adj[1] = j;
						adjRes |= e1_2;
					}
				}
			}

			if(!(adjRes & e2_0))
			{
				for(int k = 0; k < 3; k++)
				{
					if((this->plist[i].vert[2] == this->plist[j].vert[idx[k]]) && (this->plist[i].vert[idx[0]] == this->plist[j].vert[idx[k + 1]]))
					{
						this->adj[i].adj[2] = j;
						adjRes |= e2_0;
					}
					else if((this->plist[i].vert[2] == this->plist[j].vert[idx[k + 1]]) && (this->plist[i].vert[idx[0]] == this->plist[j].vert[idx[k]]))
					{
						this->adj[i].adj[2] = j;
						adjRes |= e2_0;
					}
				}
			}

			if(adjRes == eAll) break;
		}
	}
}

void struct3D::OBJECT4D_TYP::updateBBoxPosition()
{
	vecCopy(&this->world_pos, &this->boundingBox.c);
}

void struct3D::OBJECT4D_TYP::ResetOBJECT4D()
{
	this->state &= ~struct3D::OBJECT4D_STATE_CULLED; //reset bit

	for(int poly = 0; poly < this->num_polys; poly++)
	{
		POLY4D_PTR currPoly = &this->plist[poly];

		if(!(currPoly->state & struct3D::POLY4D_STATE_ACTIVE)) continue;

		currPoly->state &= ~struct3D::POLY4D_STATE_CLIPPED;
		currPoly->state &= ~struct3D::POLY4D_STATE_BACKFACE;
		currPoly->state &= ~struct3D::POLY4D_STATE_LIT;
	}
}

int struct3D::OBJECT4D_TYP::ComputePolyNormals()
{

	for(int poly = 0; poly < this->num_polys; poly++)
	{
		int vertex0 = this->plist[poly].vert[0];
		int vertex1 = this->plist[poly].vert[1];
		int vertex2 = this->plist[poly].vert[2];

		VECTOR4D u, v, n;

		u.VECTOR4D_Build(&this->vlist_local[vertex0].v, &this->vlist_local[vertex1].v);
		v.VECTOR4D_Build(&this->vlist_local[vertex0].v, &this->vlist_local[vertex2].v);

		u.VECTOR4D_Cross(&v, &n);

		this->plist[poly].nlength = n.VECTOR4D_Length();
	}

	return 1;
}

int struct3D::OBJECT4D_TYP::ComputeVertexNormals()
{
	int polys_touch_vertex[OBJECT4D_MAX_VERTICES];
	memset((void *)polys_touch_vertex, 0, sizeof(int) * OBJECT4D_MAX_VERTICES);

	for(int poly = 0; poly < this->num_polys; poly++)
	{
		if(this->plist[poly].attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD || this->plist[poly].attr & struct3D::POLY4D_ATTR_SHADE_MODE_PHONG)
		{
			int vertex0 = this->plist[poly].vert[0];
			int vertex1 = this->plist[poly].vert[1];
			int vertex2 = this->plist[poly].vert[2];

			VECTOR4D u, v, n;

			u.VECTOR4D_Build(&this->vlist_local[vertex0].v, &this->vlist_local[vertex1].v);
			v.VECTOR4D_Build(&this->vlist_local[vertex0].v, &this->vlist_local[vertex2].v);

			u.VECTOR4D_Cross(&v, &n);

			polys_touch_vertex[vertex0]++;
			polys_touch_vertex[vertex1]++;
			polys_touch_vertex[vertex2]++;

			this->vlist_local[vertex0].n.VECTOR4D_Add(&n, &this->vlist_local[vertex0].n);
			this->vlist_local[vertex1].n.VECTOR4D_Add(&n, &this->vlist_local[vertex1].n);
			this->vlist_local[vertex2].n.VECTOR4D_Add(&n, &this->vlist_local[vertex2].n);
		}
	}

	for(int vertex = 0; vertex < this->num_vertices; vertex++)
	{
		if(polys_touch_vertex[vertex] >= 1)
		{
			this->vlist_local[vertex].nx /= polys_touch_vertex[vertex];
			this->vlist_local[vertex].ny /= polys_touch_vertex[vertex];
			this->vlist_local[vertex].nz /= polys_touch_vertex[vertex];

			this->vlist_local[vertex].n.VECTOR4D_Normalize();
		}
	}

	return 1;
}

int struct3D::OBJECT4D_TYP::SetFrame(int frame)
{

	if(!(this->attr & struct3D::OBJECT4D_ATTR_MULTI_FRAME))
		return 0;

	if(frame < 0)
		frame = 0;
	else if(frame >= this->num_frames)
		frame = this->num_frames - 1;

	this->curr_frame = frame;

	this->vlist_local = &(this->head_vlist_local[frame * this->num_vertices]);
	this->vlist_trans = &(this->head_vlist_trans[frame * this->num_vertices]);

	return 1;
}

int struct3D::OBJECT4D_TYP::Destroy()
{
	if(this->scr_pts)
		free(this->scr_pts);

	if(this->head_vlist_local)
		free(this->head_vlist_local);

	if(this->head_vlist_trans)
		free(this->head_vlist_trans);

	if(this->tlist)
		free(this->tlist);

	if(this->plist)
		free(this->plist);

	if(this->avg_radius)
		free(this->avg_radius);

	if(this->max_radius)
		free(this->max_radius);
	
	if(this->adj)
		delete this->adj;

	//memset(this, 0 ,sizeof(OBJECT4D_TYP));

	return 1;
}

int struct3D::OBJECT4D_TYP::Init(int num_vertices, int num_polys, int num_frames, int destroy)
{
	if(destroy)
		this->Destroy();

	this->adj = 0;

	/*if(!(this->vlist_local = (VERTEX4DT_PTR)malloc(sizeof(VERTEX4DT_TYP) * num_vertices * num_frames))) 
		return 0;*/

	this->vlist_local = new VERTEX4DT_TYP[num_vertices * num_frames];

	memset(this->vlist_local, 0, sizeof(VERTEX4DT_TYP) * num_vertices * num_frames);
	this->scr_pts = 0;
	/*if(!(this->scr_pts = (POINT2D_PTR)malloc(sizeof(POINT2D) * num_vertices * num_frames))) 
		return 0;

	memset(this->scr_pts, 0, sizeof(POINT2D) * num_vertices * num_frames);*/

	if(!(this->vlist_trans = (VERTEX4DT_PTR)malloc(sizeof(VERTEX4DT_TYP) * num_vertices * num_frames))) 
		return 0;

	memset(this->vlist_trans, 0, sizeof(VERTEX4DT_TYP) * num_vertices * num_frames);

	if(!(this->tlist = (POINT2D_PTR)malloc(sizeof(POINT2D) * num_polys * 3)))
		return 0;

	memset(this->tlist, 0, sizeof(POINT2D) * num_polys * 3);

	if(!(this->avg_radius = (float *)malloc(sizeof(float) * num_frames)))
		return 0;

	memset(this->avg_radius, 0, sizeof(float) * num_frames);

	if(!(this->max_radius = (float *)malloc(sizeof(float) * num_frames)))
		return 0;

	memset(this->max_radius, 0, sizeof(float) * num_frames);

	/*if(!(this->plist = (POLY4D_PTR)malloc(sizeof(POLY4D) * num_polys)))
		return 0;*/

	this->plist = new POLY4D[num_polys];

	memset(this->plist, 0, sizeof(POLY4D) * num_polys);

	this->head_vlist_local = this->vlist_local;
	this->head_vlist_trans = this->vlist_trans;

	this->num_frames = num_frames;
	this->num_polys = num_polys;
	this->num_vertices = num_vertices;
	this->total_vertices = num_vertices * num_frames;
	this->curr_frame = 0;

	return 1;
}

void struct3D::OBJECT4D_TYP::ModelToWorld(int all_frames)
{
	if(!all_frames)
	{
		for(int vertex = 0; vertex < this->num_vertices; vertex++)
		{
			this->vlist_local[vertex].v.VECTOR4D_Add(&this->world_pos, &this->vlist_trans[vertex].v);
			this->vlist_local[vertex].n.VECTOR4D_COPY(&this->vlist_trans[vertex].n);
			//this->vlist_local[vertex].t.VECTOR4D_COPY(&this->vlist_trans[vertex].t);
			vecCopy(&this->vlist_local[vertex].t, &this->vlist_trans[vertex].t);
			this->vlist_trans[vertex].attr = this->vlist_local[vertex].attr;
		}
	}
	else
	{
		for(int vertex = 0; vertex < this->total_vertices; vertex++)
		{
			this->head_vlist_local[vertex].v.VECTOR4D_Add(&this->world_pos, &this->head_vlist_trans[vertex].v);
			this->vlist_local[vertex].n.VECTOR4D_COPY(&this->vlist_trans[vertex].n);
		}
	}
}

void struct3D::OBJECT4D_TYP::TransformOBJECT4D(MATRIX4X4_PTR mt, coordSelect enm, bool transformBasis, int all_frames)
{
	if(!all_frames)
	{
		switch(enm)
		{
		case coordSelect::TRANSFORM_LOCAL_ONLY:
			{
				for(int vertex = 0; vertex < this->num_vertices; vertex++)
				{
					POINT4D pResult;

					this->vlist_local[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
					pResult.VECTOR4D_COPY(&this->vlist_local[vertex].v);

					if(this->vlist_local[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->vlist_local[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
						pResult.VECTOR4D_COPY(&this->vlist_local[vertex].n);
					}
				}
			}break;
		case coordSelect::TRANSFORM_TRANS_ONLY:
			{
				for(int vertex = 0; vertex < this->num_vertices; vertex++)
				{
					POINT4D pResult;

					this->vlist_trans[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
					pResult.VECTOR4D_COPY(&this->vlist_trans[vertex].v);

					if(this->vlist_trans[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->vlist_trans[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
						pResult.VECTOR4D_COPY(&this->vlist_trans[vertex].n);
					}
				}
			}break;
		case coordSelect::TRANSFORM_LOCAL_TO_TRANS:
			{
				for(int vertex = 0; vertex < this->num_vertices; vertex++)
				{
					this->vlist_local[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &this->vlist_trans[vertex].v);

					if(this->vlist_local[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->vlist_local[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &this->vlist_trans[vertex].v);
					}
				}
			}break;
		default: break;
		}
	}
	else
	{
		switch(enm)
		{
		case coordSelect::TRANSFORM_LOCAL_ONLY:
			{
				for(int vertex = 0; vertex < this->total_vertices; vertex++)
				{
					POINT4D pResult;

					this->head_vlist_local[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
					pResult.VECTOR4D_COPY(&this->head_vlist_local[vertex].v);

					if(this->head_vlist_local[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->head_vlist_local[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
						pResult.VECTOR4D_COPY(&this->head_vlist_local[vertex].n);
					}
				}
			}break;
		case coordSelect::TRANSFORM_TRANS_ONLY:
			{
				for(int vertex = 0; vertex < this->total_vertices; vertex++)
				{
					POINT4D pResult;

					this->head_vlist_trans[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
					pResult.VECTOR4D_COPY(&this->head_vlist_trans[vertex].v);

					if(this->head_vlist_trans[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->head_vlist_trans[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &pResult);
						pResult.VECTOR4D_COPY(&this->head_vlist_trans[vertex].n);
					}
				}
			}break;
		case coordSelect::TRANSFORM_LOCAL_TO_TRANS:
			{
				for(int vertex = 0; vertex < this->total_vertices; vertex++)
				{
					this->head_vlist_local[vertex].v.Mat_Mul_VECTOR4D_4X4(mt, &this->head_vlist_trans[vertex].v);

					if(this->head_vlist_local[vertex].attr & struct3D::VERTEX4DT_ATTR_NORMAL)
					{
						this->head_vlist_local[vertex].n.Mat_Mul_VECTOR4D_4X4(mt, &this->head_vlist_trans[vertex].v);
					}
				}
			}break;
		default: break;
		}	
	}

	if(transformBasis)
	{
		VECTOR4D vResult;

		this->ux.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&this->ux);

		this->uy.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&this->uy);

		this->uz.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&this->uz);
	}
}

void struct3D::CAM4D_TYP::Init_CAM4D(int cam_attr, POINT4D_PTR cam_pos, VECTOR4D_PTR cam_dir, POINT4D_PTR cam_target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height)
{
	this->attr = cam_attr;

	cam_pos->VECTOR4D_COPY(&this->pos);
	cam_dir->VECTOR4D_COPY(&this->dir);

	this->u.VECTOR4D_INITXYZ(1.0f, 0.0f, 0.0f);
	this->v.VECTOR4D_INITXYZ(0.0f, 1.0f, 0.0f);
	this->n.VECTOR4D_INITXYZ(0.0f, 0.0f, 1.0f);

	if(cam_target != NULL)
	{
		cam_target->VECTOR4D_COPY(&this->target);
	}
	else
	{
		this->target.VECTOR4D_ZERO();
	}

	this->near_clip_z = near_clip_z;
	this->far_clip_z = far_clip_z;

	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;
	this->viewport_center_x = (viewport_width - 1) / 2;
	this->viewport_center_y = (viewport_height - 1) / 2;
	this->aspect_ratio = (float)viewport_width / (float)viewport_height;

	this->mcam.MAT_IDENTITY_4X4();
	this->mper.MAT_IDENTITY_4X4();
	this->mscr.MAT_IDENTITY_4X4();

	this->fov = fov;

	this->viewplane_width = 2.0f;
	this->viewplane_height = 2.0f / this->aspect_ratio;

	float tan_fov_div2 = cos(math3D::mathFunc.DEG_TO_RAD(fov / 2)) / sin(math3D::mathFunc.DEG_TO_RAD(fov / 2));//tan(math3D::mathFunc.DEG_TO_RAD(fov / 2));//В книге используеться тангенс, но в таком случае при уменшении угла озора он будет увеличиваться, так что нужен катангенс как противоположная функция

	this->view_dist = /*0.5f * this->viewplane_width * */tan_fov_div2;
	this->view_dist2 = this->viewport_center_x * tan_fov_div2;
	//this->view_dist = /*0.5f **/ this->viewport_center_x * tan_fov_div2;//viewplane заменен на viewport для совмещения преобразований в CameraToPerspective и PerspectiveToScreen.

	if(fov == 90.0f)
	{
		POINT3D pt_origin;
		pt_origin.VECTOR3D_ZERO();

		VECTOR3D vn;

		vn.VECTOR3D_INITXYZ(1.0f, 0.0f, -1.0f);
		this->rt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(-1.0f, 0.0f, -1.0f);
		this->lt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(0.0, 1.0f, -1.0f);
		this->tp_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(0.0f, -1.0f, -1.0f);
		this->bt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);
	}
	else
	{
		POINT3D pt_origin;
		pt_origin.VECTOR3D_ZERO();

		VECTOR3D vn;

		vn.VECTOR3D_INITXYZ(this->view_dist, 0.0f, -this->viewplane_width / 2.0f);
		this->rt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(-this->view_dist, 0.0f, -this->viewplane_width / 2.0f);
		this->lt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(0.0f, this->view_dist, -this->viewplane_width / 2.0f);
		this->tp_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);

		vn.VECTOR3D_INITXYZ(0.0f, -this->view_dist, -this->viewplane_width / 2.0f);
		this->bt_clip_plane.PLANE3D_Init(&pt_origin, &vn, 1);
	}

	vecZero(&_look);
	vecZero(&_right);
	vecZero(&_up);

	_look.z = 1.0f;
	_right.x = 1.0f;
	_up.y = 1.0f;

	vecZero(&this->pos_pan_based);
}

void struct3D::CAM4D_TYP::Build_CAM4D_Matrix_Euler(int CamRotSeq)
{
	MATRIX4X4 mtInv, mxInv, myInv, mzInv, mRot, mTmp;

	mtInv.Mat_Init_4X4(1.0f, 0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f,
					   -this->pos.x, -this->pos.y, -this->pos.z, 1.0f);

	float thetaX = this->dir.x;
	float thetaY = this->dir.y;
	float thetaZ = this->dir.z;

	float cosTheta = math3D::mathFunc.Fast_Cos(thetaX);
	float sinTheta = -math3D::mathFunc.Fast_Sin(thetaX);

	mxInv.Mat_Init_4X4(1.0f, 0.0f, 0.0f, 0.0f,
					   0.0f, cosTheta, sinTheta, 0.0f,
					   0.0f, -sinTheta, cosTheta, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
	cosTheta = math3D::mathFunc.Fast_Cos(thetaY);
	sinTheta = -math3D::mathFunc.Fast_Sin(thetaY);

	myInv.Mat_Init_4X4(cosTheta, 0.0f, -sinTheta, 0.0f,
					   0.0f, 1.0f, 0.0f, 0.0f,
					   sinTheta, 0.0f, cosTheta, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);

	cosTheta = math3D::mathFunc.Fast_Cos(thetaZ);
	sinTheta = -math3D::mathFunc.Fast_Sin(thetaZ);

	mzInv.Mat_Init_4X4(cosTheta, sinTheta, 0.0f, 0.0f,
					   -sinTheta, cosTheta, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);

	switch(CamRotSeq)
	{
	case struct3D::CAM_ROT_SEQ_XYZ:
		{
			mxInv.Mat_Mul_4X4(&myInv, &mTmp);
			mTmp.Mat_Mul_4X4(&mzInv, &mRot);
		}break;
	case struct3D::CAM_ROT_SEQ_XZY:
		{
			mxInv.Mat_Mul_4X4(&mzInv, &mTmp);
			mTmp.Mat_Mul_4X4(&myInv, &mRot);
		}break;
	case struct3D::CAM_ROT_SEQ_YXZ:
		{
			myInv.Mat_Mul_4X4(&mxInv, &mTmp);
			mTmp.Mat_Mul_4X4(&mzInv, &mRot);
		}break;
	case struct3D::CAM_ROT_SEQ_YZX:
		{
			myInv.Mat_Mul_4X4(&mzInv, &mTmp);
			mTmp.Mat_Mul_4X4(&mxInv, &mRot);
		}break;
	case struct3D::CAM_ROT_SEQ_ZXY:
		{
			mzInv.Mat_Mul_4X4(&mxInv, &mTmp);
			mTmp.Mat_Mul_4X4(&myInv, &mRot);
		}break;
	case struct3D::CAM_ROT_SEQ_ZYX:
		{
			mzInv.Mat_Mul_4X4(&myInv, &mTmp);
			mTmp.Mat_Mul_4X4(&mxInv, &mRot);
		}break;
	default: break;
	}

	mtInv.Mat_Mul_4X4(&mRot, &this->mcam);
}

void struct3D::CAM4D_TYP::Build_CAM4D_Matrix_UVN(int mode)
{
	MATRIX4X4 mtInv, mUVN, mTmp;

	mtInv.Mat_Init_4X4(1.0f, 0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f,
					   -this->pos.x, -this->pos.y, -this->pos.z, 1.0f);

	if(mode == struct3D::UVN_MODE_SPHERICAL)
	{
		float phi = this->dir.x;
		float theta = this->dir.y;

		float sinPhi = math3D::mathFunc.Fast_Sin(phi);
		float cosPhi = math3D::mathFunc.Fast_Cos(phi);

		float sinTheta = math3D::mathFunc.Fast_Sin(theta);
		float cosTheta = math3D::mathFunc.Fast_Cos(theta);

		this->target.x = -1.0f * sinPhi * sinTheta;
		this->target.y = 1.0f * cosPhi;
		this->target.z = 1.0f * sinPhi * cosTheta;
	}

	//this->target.VECTOR4D_Sub(&this->pos, &this->n); //Вычисляем вектор направления от позиции камеры к точке на которую она направлена.В книге используеться функция Build. Она выполняет тоже самое.
	this->n.VECTOR4D_Build(&this->pos, &this->target);

	this->v.VECTOR4D_INITXYZ(0.0f, 1.0f, 0.0f);

	this->v.VECTOR4D_Cross(&this->n, &this->u);

	this->n.VECTOR4D_Cross(&this->u, &this->v);

	this->u.VECTOR4D_Normalize();
	this->v.VECTOR4D_Normalize();
	this->n.VECTOR4D_Normalize();

	mUVN.Mat_Init_4X4(this->u.x, this->v.x, this->n.x, 0.0f,
					  this->u.y, this->v.y, this->n.y, 0.0f,
					  this->u.z, this->v.z, this->n.z, 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f);
	mtInv.Mat_Mul_4X4(&mUVN, &this->mcam);
}

void struct3D::CAM4D_TYP::Build_CAM4D_Matrix2()
{
	vecNormalize(&this->_look);
	vecNormalize(&this->_up);

	//vecCross(&this->_up, &this->_look, &this->_right);
	//vecNormalize(&this->_up);
	vecNormalize(&this->_right);

	//vecCross(&this->_right, &this->_up, &this->_look);
	//vecNormalize(&this->_right);

	vecCross(&this->_look, &this->_right, &this->_up);
	vecNormalize(&this->_up);

	vecCross(&this->_up, &this->_look, &this->_right);
	vecNormalize(&this->_right);

	float x = -vecDot(&_right, &pos);
    float y = -vecDot(&_up, &pos);
    float z = -vecDot(&_look, &pos);

	mcam.M00 = _right.x;
	mcam.M01 = _up.x;
	mcam.M02 = _look.x;
	mcam.M03 = 0.0f;

	mcam.M10 = _right.y;
	mcam.M11 = _up.y;
	mcam.M12 = _look.y;
	mcam.M13 = 0.0f;

	mcam.M20 = _right.z;
	mcam.M21 = _up.z;
	mcam.M22 = _look.z;
	mcam.M23 = 0.0f;

	mcam.M30 = x;
	mcam.M31 = y;
	mcam.M32 = z;
	mcam.M33 = 1.0f;
}

void struct3D::CAM4D_TYP::get_CAM4D_InvMatrix2(MATRIX4X4_PTR m)
{
	vecNormalize(&this->_look);
	vecNormalize(&this->_up);

	//vecCross(&this->_up, &this->_look, &this->_right);
	//vecNormalize(&this->_up);
	vecNormalize(&this->_right);

	//vecCross(&this->_right, &this->_up, &this->_look);
	//vecNormalize(&this->_right);

	vecCross(&this->_look, &this->_right, &this->_up);
	vecNormalize(&this->_up);

	vecCross(&this->_up, &this->_look, &this->_right);
	vecNormalize(&this->_right);

	float x = vecDot(&_right, &pos);
    float y = vecDot(&_up, &pos);
    float z = vecDot(&_look, &pos);

	m->M00 = -_right.x;
	m->M01 = -_up.x;
	m->M02 = -_look.x;
	m->M03 = 0.0f;

	m->M10 = -_right.y;
	m->M11 = -_up.y;
	m->M12 = -_look.y;
	m->M13 = 0.0f;

	m->M20 = -_right.z;
	m->M21 = -_up.z;
	m->M22 = -_look.z;
	m->M23 = 0.0f;

	m->M30 = x;
	m->M31 = y;
	m->M32 = z;
	m->M33 = 1.0f;
}

void struct3D::CAM4D_TYP::Build_Camera_To_Perspective_MATRIX4X4()
{
	this->mper.Mat_Init_4X4(this->view_dist, 0.0f, 0.0f, 0.0f,
							0.0f, this->view_dist * this->aspect_ratio, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 0.0f);
}

void struct3D::CAM4D_TYP::Build_Perspective_To_Screen_4D_MATRIX4X4()
{
	float alpha = (0.5f * this->viewplane_width - 0.5f);
	float beta = (0.5f * this->viewplane_height - 0.5f);

	this->mscr.Mat_Init_4X4(alpha, 0.0f, 0.0f, 0.0f,
							0.0f, -beta, 0.0f, 0.0f,
							alpha, beta, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);
}


void struct3D::CAM4D_TYP::getFwdVector(VECTOR4D_PTR vec)
{
	vecCopy(&_look, vec);
}
void struct3D::CAM4D_TYP::getRightVector(VECTOR4D_PTR vec)
{
	vecCopy(&_right, vec);
}
void struct3D::CAM4D_TYP::getUpVector(VECTOR4D_PTR vec)
{
	vecCopy(&_up, vec);
}

void struct3D::CAM4D_TYP::strafe(float units)
{
	VECTOR4D vTmp;

	vecCopy(&_right, &vTmp);
	vecScale(units, &vTmp);
	vecAdd(&pos, &vTmp, &pos);
}
void struct3D::CAM4D_TYP::fly(float units)
{
	VECTOR4D vTmp;

	vecCopy(&_up, &vTmp);
	vecScale(units, &vTmp);
	vecAdd(&pos, &vTmp, &pos);
}
void struct3D::CAM4D_TYP::walk(float units)
{
	VECTOR4D vTmp;

	vecCopy(&_look, &vTmp);
	vecScale(units, &vTmp);
	vecAdd(&pos, &vTmp, &pos);
}
void struct3D::CAM4D_TYP::pan(float units_up, float units_right)
{
	VECTOR4D vTmp;

	vecCopy(&_up, &vTmp);
	vecScale(units_up, &vTmp);
	vecAdd(&pos, &vTmp, &pos);
	vecAdd(&pos_pan_based, &vTmp, &pos_pan_based);

	vecCopy(&_right, &vTmp);
	vecScale(units_right, &vTmp);
	vecAdd(&pos, &vTmp, &pos);
	vecAdd(&pos_pan_based, &vTmp, &pos_pan_based);
}

void struct3D::CAM4D_TYP::pitch(float angle)
{
	MATRIX4X4 mat;
	VECTOR4D vResult;

	/*D3DXMATRIX T;
	D3DXVECTOR3 right;

	right.x = _right.x;
	right.y = _right.y;
	right.z = _right.z;*/

	//D3DXMatrixRotationAxis(&T, &right, math3D::mathFunc.DEG_TO_RAD(angle));

	matAxisRotation(&mat, &this->_right, -angle);

	//matIdentity(&mat);

	//mat.M00 = T._11;
	//mat.M01 = T._12;
	//mat.M02 = T._13;
	//mat.M03 = T._14;

	//mat.M10 = T._21;
	//mat.M11 = T._22;
	//mat.M12 = T._23;
	//mat.M13 = T._24;

	//mat.M20 = T._31;
	//mat.M21 = T._32;
	//mat.M22 = T._33;
	//mat.M23 = T._34;

	//mat.M30 = T._41;
	//mat.M31 = T._42;
	//mat.M32 = T._43;
	//mat.M33 = T._44;

	vecMulMat(&this->_look, &mat, &vResult);
	vecCopy(&vResult, &this->_look);

	vecMulMat(&this->_up, &mat, &vResult);
	vecCopy(&vResult, &this->_up);
}
void struct3D::CAM4D_TYP::yaw(float angle)
{
	MATRIX4X4 mat;
	VECTOR4D vResult;

	//VECTOR4D vY;
	//vecZero(&vY);

	//vY.y = 1.0f;

	mat.Build_XYZ_Rotation_MATRIX4X4(0, angle, 0);
	//matAxisRotation(&mat, &this->_up, angle);
	//matAxisRotation(&mat, &vY, angle);

	vecMulMat(&this->_right, &mat, &vResult);
	vecCopy(&vResult, &this->_right);

	vecMulMat(&this->_look, &mat, &vResult);
	vecCopy(&vResult, &this->_look);
}
void struct3D::CAM4D_TYP::roll(float angle)
{
	MATRIX4X4 mat;
	VECTOR4D vResult;

	//mat.Build_Axis_Rotation_MATRIX4X4(&this->_look, angle);
	matAxisRotation(&mat, &this->_look, angle);

	vecMulMat(&this->_right, &mat, &vResult);
	vecCopy(&vResult, &this->_right);

	vecMulMat(&this->_up, &mat, &vResult);
	vecCopy(&vResult, &this->_up);
}

void struct3D::CAM4D_TYP::arcRotate(float right, float up)
{
	MATRIX4X4 mRot, mRot2, mTrans, mTrans2, mRes;
	POINT4D pRes;
	//VECTOR4D vecRight;

	this->pitch(up);
	this->yaw(-right);

	//this->graphics->getMainCamera()->getRightVector(&vecRight);

	mRot.Build_XYZ_Rotation_MATRIX4X4(0, -right, 0);
	matAxisRotation(&mRot2, &this->_right, -up);
	matIdentity(&mTrans);
	matIdentity(&mTrans2);

	mTrans.M30 = -this->pos_pan_based.x;
	mTrans.M31 = -this->pos_pan_based.y;
	mTrans.M32 = -this->pos_pan_based.z;

	mTrans2.M30 = this->pos_pan_based.x;
	mTrans2.M31 = this->pos_pan_based.y;
	mTrans2.M32 = this->pos_pan_based.z;

	matMul(&mTrans, &mRot, &mRes);
	matMul(&mRes, &mRot2, &mTrans);
	matMul(&mTrans, &mTrans2, &mRes);

	vecMulMat(&this->pos, &mRes, &pRes);
	
	//vecCopy(&pRes, &this->pos);
	this->pos.x = pRes.x;
	this->pos.y = pRes.y;
	this->pos.z = pRes.z;
}

//MAT NAMESPACE================================================START
int mat::MaterialSystem::DestroyBitmap(BITMAP_IMAGE_PTR image)
{
	if (image && image->buffer)
	{
		// free memory and reset vars
		free(image->buffer);

		// set all vars in structure to 0
		memset(image,0,sizeof(BITMAP_IMAGE));

		// return success
		return 1;
	} // end if
	else  // invalid entry pointer of the object wasn't initialized
		return 0;
}

int mat::MaterialSystem::ResetMaterialsMAT()
{
	for(int currMat = 0; currMat < MAX_MATERIALS; currMat++)
	{
		DestroyBitmap(&this->materials[currMat].texture);
		memset(&materials[currMat], 0, sizeof(MAT));
	}

	return 1;
}

int mat::LightSystem::ResetLightsLIGHT()
{
	this->num_lights = 0;
	memset(this->lights, 0, sizeof(LIGHT) * this->MAX_LIGHTS);

	return 1;
}
int mat::LightSystem::InitLightLIGHT(int index, int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf)
{
	if(index < 0 || index >= this->MAX_LIGHTS)
		return -1;

	lights[index].state = state;
	lights[index].id = index;
	lights[index].attr = attr;

	lights[index].c_ambient = c_ambient;
	lights[index].c_diffuse = c_diffuse;
	lights[index].c_specular = c_specular;

	lights[index].kc = kc;
	lights[index].kl = kl;
	lights[index].kq = kq;

	if(pos)
		pos->VECTOR4D_COPY(&lights[index].pos);

	if(dir)
	{
		dir->VECTOR4D_COPY(&lights[index].dir);

		lights[index].dir.VECTOR4D_Normalize();
	}

	lights[index].spot_inner = spot_inner;
	lights[index].spot_outer = spot_outer;

	lights[index].pf = pf;

	this->num_lights++;

	return index;
}
mat::LightSystem::LightSystem()
{
	this->ResetLightsLIGHT();
}

int mat::LightSystem::getNumLights()
{
	return this->num_lights;
}
void mat::LightSystem::transformLights(MATRIX4X4_PTR mt)
{
	if(!mt) return;

	MATRIX4X4 mr;

	matCopy(mt, &mr);

	mr.M30 = mr.M31 = mr.M32 = 0.0f;

	for(int i = 0; i < this->num_lights; i++)
	{
		vecMulMat(&this->lights[i].pos, mt, &this->lights[i].tpos);
		vecMulMat(&this->lights[i].dir, &mr, &this->lights[i].tdir);
	}
}
//MAT NAMESPACE================================================END



//NEW. observer pattern
void struct3D::SelectionSubject::Attach(Observer *o)
{
	obs.push_back(o);
}
void struct3D::SelectionSubject::DetachAll()
{
	for(int i = 0; i < obs.size(); i++)
	{
		obs[i]->unsetSubject();
	}
	obs.clear();
}
bool struct3D::SelectionSubject::IsObserverHere(Observer *o)
{
	bool result = false;

	for(int i = 0; i < obs.size(); i++)
	{
		if(o == obs[i])
		{
			result = true;
			break;
		}
	}

	return result;
}
void struct3D::SelectionSubject::Notify()
{
	/*TCHAR mas[128];
	swprintf(mas, L"%d", obs.size());
	MessageBox(0,mas,0,0);*/

	/*TCHAR mas[128];
	swprintf(mas, L"%d", obs.size());
	MessageBox(0,mas,0,0);*/

	for(int i = 0; i < obs.size(); i++)
	{
		obs[i]->Update();
	}
}
void struct3D::OBJECT4D_TYP::getState(float *data, int *size_out)
{
	if(!size_out) return;

	*size_out = 10;
	//data = this->typeData;

	for(int i = 0; i < 10; i++)
	{
		data[i] = this->typeData[i];
	}
}
void struct3D::OBJECT4D_TYP::setState(float *data, int size)
{
	if(size <= 0 || size >= 10) return;

	for(int i = 0; i < size; i++)
		this->typeData[i] = data[i];

	this->Notify();
}

void struct3D::OBJECT4D_TYP::setState(float data, int dataidx)
{
	if(dataidx < 0 || dataidx >= 10) return;

	this->typeData[dataidx] = data;

	//this->Notify();
}

void struct3D::OBJECT4D_TYP::setState(std::string data, int dataidx)
{
	if(dataidx != DataSlotCommon::Name)
		return;

	this->strName = data;
}
void struct3D::OBJECT4D_TYP::setState(int *data, int dataidx)
{
	if(dataidx != DataSlotCommon::Color)
		return;

	this->color = data[0];

	if(this->type == ObjectType::EditableMesh)
	{
		for(int i = 0; i < this->num_polys; i++)
		{
			this->plist[i].color = this->plist[i].lit_color[0] = this->color;
		}
	}

	this->Notify();
}
void struct3D::OBJECT4D_TYP::getState(std::string data, int *size_out)
{
	if(!size_out)
		return;

	data = this->strName;
	*size_out = 1;
}
void struct3D::OBJECT4D_TYP::getState(int *data, int *size_out)
{
	if(!data || !size_out)
		return;

	data[0] = this->color;
	*size_out = 1;
}

//void struct3D::ObserverText::Update()
//{
//	int size;
//	float data[10];
//
//	this->subject->getState(data, &size);
//
//	//if(size != this->obj.size()) return;
//
//	/*TCHAR mas[128];
//	swprintf(mas, L"%d", size);
//	MessageBox(0,mas,0,0);*/
//
//	wxString str;
//	wxString *strName;
//
//	for(int i = 0; i < obj.size(); i++)
//	{
//		if(this->data_slots[i] > -1 && this->data_slots[i] < 10)
//		{
//			str = wxString::FromDouble(data[data_slots[i]]);//
//			obj[i]->ChangeValue(str);
//		}
//
//		if(this->data_slots[i] == DataSlotCommon::Name)
//		{
//			this->subject->getState(&str, &size);
//			obj[i]->ChangeValue(str);
//		}
//	}
//}
//
//void struct3D::ObserverText::setSubject(SelectionSubject *subj)
//{
//	this->subject = (OBJECT4D_TYP *)subj;
//}
//void struct3D::ObserverText::unsetSubject()
//{
//	this->subject = 0;
//}
//void struct3D::ObserverText::addTextCtrl(wxTextCtrl *t, int data_slot)
//{
//	this->obj.push_back(t);
//	this->data_slots.push_back(data_slot);
//}
//
//void struct3D::ObserverText::Release()
//{
//	this->obj.clear();
//}
//
//void struct3D::ObserverText::setMinNumber(float min)
//{
//	this->min.push_back(min);
//}
//void struct3D::ObserverText::setMaxNumber(float max)
//{
//	this->max.push_back(max);
//}
////void struct3D::ObserverText::setNumberModifier(float modifier)
////{
////	this->modifier.push_back(fabs(modifier));
////}
//
//void struct3D::ObserverText::updateSubjectData()
//{
//	float data[10];
//	double value;
//	wxString str;
//	int size_out = 0;
//
//	if(!this->subject) return;
//
//	this->subject->getState(data, &size_out);
//
//	for(int i = 0; i < this->obj.size(); i++)
//	{
//		str = obj[i]->GetLineText(0);
//
//		if(this->data_slots[i] > -1 && this->data_slots[i] < 10)
//		{
//			if(str.ToDouble(&value))
//			{
//				if(value >= min[i] && value <= max[i])
//				this->subject->setState(value, this->data_slots[i]);
//			}
//		}
//
//		if(this->data_slots[i] == DataSlotCommon::Name)
//		{
//			this->subject->setState(&str, this->data_slots[i]);
//		}
//	}
//
//	this->subject->Notify();
//}
//
//void struct3D::ObserverText::modifySubjectData(float modifier, int dataSlot)
//{
//	float data[10];
//	wxString str;
//	int size_out = 0;
//	//float modifier = 0;
//
//	if(!this->subject) return;
//
//	this->subject->getState(data, &size_out);
//
//	for(int i = 0; i < this->obj.size(); i++)
//	{
//		if(dataSlot == this->data_slots[i])
//		{
//			/*if(add)
//			{
//				modifier = this->modifier[i];
//			}
//			else
//			{
//				modifier = -this->modifier[i];
//			}*/
//
//			if((data[this->data_slots[i]] + modifier) >= min[i] && (data[this->data_slots[i]] + modifier) <= max[i])
//			this->subject->setState(data[this->data_slots[i]] + modifier, this->data_slots[i]);
//			break;
//		}
//	}
//
//	this->subject->Notify();
//}
//
//void struct3D::ObserverColor::Update()
//{
//	if(!this->subject) return;
//
//	wxColor data;
//	int datai;
//	int size = 0;
//
//	this->subject->getState(&datai, &size);
//
//	if(!size) return;
//
//	char r, g, b, a;
//
//	RGBFROM32BIT(datai, r, g, b);
//	a = 255;
//
//	data.Set(r, g, b, a);
//
//	this->obj->SetColour(data);
//}
//void struct3D::ObserverColor::setSubject(SelectionSubject *subj)
//{
//	this->subject = (OBJECT4D_TYP *)subj;
//}
//void struct3D::ObserverColor::Release()
//{
//	obj = 0;
//}
//void struct3D::ObserverColor::unsetSubject()
//{
//	this->subject = 0;
//}
//void struct3D::ObserverColor::updateSubjectData()
//{
//	if(!this->subject) return;
//
//	wxColor data;
//	int datai;
//
//	data = this->obj->GetColour();
//
//	datai = ARGB32BIT(255, (unsigned int)data.Red(), (unsigned int)data.Green(), (unsigned int)data.Blue());
//
//	this->subject->setState(&datai, DataSlotCommon::Color);
//}
//void struct3D::ObserverColor::setColorPicker(wxColourPickerCtrl *c)
//{
//	this->obj = c;
//}
//
//void struct3D::ObserverSizer::Update()
//{
//	hideAll();
//
//	if(!subject) return;
//
//	switch(this->subject->type)
//	{
//	case ObjectType::Box:
//		{
//			this->mainWnd->SetSizer(this->szBox, false);
//		}break;
//	case ObjectType::Cone:
//		{
//			this->mainWnd->SetSizer(this->szCone, false);
//		}break;
//	case ObjectType::Cylinder:
//		{
//			this->mainWnd->SetSizer(this->szCyl, false);
//		}break;
//	case ObjectType::Sphere:
//		{
//			this->mainWnd->SetSizer(this->szSph, false);
//		}break;
//	case ObjectType::EditableMesh:
//		{
//			this->mainWnd->SetSizer(this->szEM, false);
//			wxSizer *s = this->mainWnd->GetSizer();
//			size_t count = s->GetItemCount();
//
//			for(size_t i = 0; i < count; i++)
//			{
//				s->Hide(i);
//			}
//		}break;
//	default:
//		{
//			return;
//		}break;
//	};
//
//	this->mainWnd->GetSizer()->Show(size_t(0));
//	this->mainWnd->GetSizer()->Layout();
//	this->mainWnd->GetSizer()->SetSizeHints(this->mainWnd);
//	this->mainWnd->Show();
//	this->mainWnd->GetParent()->Layout();
//}
//
//void struct3D::ObserverSizer::hideAll()
//{
//	this->szBox->Hide(size_t(0));
//	this->szCone->Hide(size_t(0));
//	this->szCyl->Hide(size_t(0));
//	this->szSph->Hide(size_t(0));
//	this->szEM->Hide(size_t(0));
//}
//void struct3D::ObserverSizer::setSubject(SelectionSubject *subj)
//{
//	this->subject = (OBJECT4D_TYP *)subj;
//}
//void struct3D::ObserverSizer::Release()
//{
//
//}
//void struct3D::ObserverSizer::unsetSubject()
//{
//	this->subject = 0;
//}
//
//void struct3D::ObserverSizer::setWindow(wxWindow *w)
//{
//	this->mainWnd = w;
//}
//void struct3D::ObserverSizer::setCylinderSizer(wxSizer *s)
//{
//	this->szCyl = s;
//}
//void struct3D::ObserverSizer::setConeSizer(wxSizer *s)
//{
//	this->szCone = s;
//}
//void struct3D::ObserverSizer::setSphereSizer(wxSizer *s)
//{
//	this->szSph = s;
//}
//void struct3D::ObserverSizer::setBoxSizer(wxSizer *s)
//{
//	this->szBox = s;
//}
//void struct3D::ObserverSizer::setEMSizer(wxSizer *s)
//{
//	this->szEM = s;
//}
//void struct3D::ObseverBmpBtReset::Update()
//{
//	if(subject->type != ObjectType::EditableMesh) return;
//
//	for(int i = 0; i < this->obj.size(); i++)
//	{
//		this->obj[i]->SetValue(false);
//	}
//}
//void struct3D::ObseverBmpBtReset::setSubject(SelectionSubject *subj)
//{
//	this->subject = (OBJECT4D_TYP *)subj;
//}
//void struct3D::ObseverBmpBtReset::unsetSubject()
//{
//	this->subject = 0;
//}
//void struct3D::ObseverBmpBtReset::addBt(wxBitmapToggleButton *bt)
//{
//	this->obj.push_back(bt);
//}
//
//void struct3D::ObseverStatTxtCount::Update()
//{
//	int curr, max;
//
//	max = this->subject->getMax();
//	curr = this->subject->getCurrent();
//
//	for(int i = 0; i < this->obj.size(); i++)
//	{
//		obj[i]->SetLabel(wxString::FromDouble(curr) + "/" + wxString::FromDouble(max) + " elements selected");
//	}
//}
//void struct3D::ObseverStatTxtCount::setSubject(SelectionSubject *subj)
//{
//	this->subject = (SubSelectionCollection *)subj;
//}
//void struct3D::ObseverStatTxtCount::addSt(wxStaticText *st)
//{
//	this->obj.push_back(st);
//}
//void struct3D::ObseverStatTxtCount::unsetSubject()
//{
//	//this->subject = 0;
//}

int struct3D::OBJECT4D_LINE_TYP::initialize(int num_lines)
{
	this->vlocal_size = this->vtrans_size = num_lines;
	this->vlocal_curr = this->vtrans_curr = 0;

	this->vlocal = new PLINE4D[this->vlocal_size];
	this->vtrans = new PLINE4D[this->vtrans_size];

	return 1;
}

int struct3D::OBJECT4D_LINE_TYP::release()
{
	if(this->vlocal)
	{
		delete this->vlocal;
		this->vlocal = 0;
	}

	if(this->vtrans)
	{
		delete this->vtrans;
		this->vtrans = 0;
	}

	this->vlocal_size = this->vtrans_size = 0;
	this->vlocal_curr = this->vtrans_curr = 0;

	return 1;
}

int struct3D::OBJECT4D_LINE_TYP::add(PLINE4D *line)
{
	if(!line) return 0;
	if(this->vlocal_curr == this->vlocal_size) return 0;

	vecZero(&vlocal[this->vlocal_curr].v0.v);
	vecZero(&vlocal[this->vlocal_curr].v1.v);

	vecCopy(&line->v0.v, &vlocal[this->vlocal_curr].v0.v);
	vecCopy(&line->v1.v, &vlocal[this->vlocal_curr].v1.v);

	this->vlocal_curr++;
	return 1;
}

void struct3D::VERTEX4DT_TYP::move(float dx, float dy, float dz)
{
	this->x += dx;
	this->y += dy;
	this->z += dz;
}
void struct3D::VERTEX4DT_TYP::rotate(VECTOR4D_PTR vAngles, POINT4D_PTR pAround, MATRIX4X4_PTR mrot)
{
	MATRIX4X4 mrot2;
	POINT4D pTmp, pTmp2;

	if(!mrot)
	{
		mrot2.Build_XYZ_Rotation_MATRIX4X4(vAngles->x, vAngles->y, vAngles->z);
		mrot = &mrot2;
	}

	vecSub(&this->v, pAround, &pTmp);
	vecMulMat(&pTmp, mrot, &pTmp2);
	vecAdd(&pTmp2, pAround, &pTmp);
	vecCopy(&pTmp, &this->v);
}

void struct3D::VERTEX4DT_TYP::select()
{
	this->attr |= VERTEX4DT_ATTR_SELECTED;
	this->isSelected = true;
}
void struct3D::VERTEX4DT_TYP::unselect()
{
	this->attr &= ~VERTEX4DT_ATTR_SELECTED;
	this->isSelected = false;
}

void struct3D::VERTEX4DT_TYP::getPos(POINT4D_PTR p)
{
	if(!p) return;

	vecCopy(&this->v, p);
}

void struct3D::POLY4D_TYP::move(float dx, float dy, float dz)
{
	for(int i = 0; i < 3; i++)
	{
		this->vlist[vert[i]].x += dx;
		this->vlist[vert[i]].y += dy;
		this->vlist[vert[i]].z += dz;
	}
}

void struct3D::POLY4D_TYP::select()
{
	this->attr |= POLY4D_ATTR_SELECTED;
	this->isSelected = true;
}
void struct3D::POLY4D_TYP::unselect()
{
	this->attr &= ~POLY4D_ATTR_SELECTED;
	this->isSelected = false;
}

void struct3D::POLY4D_TYP::rotate(VECTOR4D_PTR vAngles, POINT4D_PTR pAround, MATRIX4X4_PTR mrot)
{
	MATRIX4X4 mrot2;
	POINT4D pTmp, pTmp2;

	if(!mrot)
	{
		mrot2.Build_XYZ_Rotation_MATRIX4X4(vAngles->x, vAngles->y, vAngles->z);
		mrot = &mrot2;
	}

	for(int i = 0; i < 3; i++)
	{
		vecSub(&this->vlist[vert[i]].v, pAround, &pTmp);
		vecMulMat(&pTmp, mrot, &pTmp2);
		vecAdd(&pTmp2, pAround, &pTmp);
		vecCopy(&pTmp, &this->vlist[vert[i]].v);
	}
}

void struct3D::POLY4D_TYP::getPos(POINT4D_PTR p)
{
	if(!p) return;

	float threeInv = 0.33333333333333333333333333333333f;

	vecCopy(&this->vlist[this->vert[0]].v, p);
	vecAdd(p, &this->vlist[this->vert[1]].v, p);
	vecAdd(p, &this->vlist[this->vert[2]].v, p);
	vecScale(threeInv, p);
}

void struct3D::SubSelectionCollection::add(SubSelection *obj)
{
	if(!obj->isSelected)
	{
		POINT4D pos;
		this->objects.push_back(obj);
		obj->getPos(&pos);
		vecAdd(&this->pCenter, &pos, &this->pCenter);
		obj->select();
		this->Notify();
	}
}

void struct3D::SubSelectionCollection::setPos(POINT4D_PTR p)
{
	vecCopy(p, &this->pWorld);
}

void struct3D::SubSelectionCollection::getPos(POINT4D_PTR p)
{
	if(this->objects.size() != 0)
	{
	float k = 1.0f / (float)this->objects.size();
	vecScale(k, &this->pCenter, p);
	vecAdd(&pWorld, p ,p);
	}
	else
	{
		vecCopy(&pWorld, p);
	}
}

void struct3D::SubSelectionCollection::setMax(int max)
{
	this->Notify();

	this->maxObjects = max;
}
int struct3D::SubSelectionCollection::getMax()
{
	return this->maxObjects;
}
void struct3D::SubSelectionCollection::reset()
{
	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects[i]->unselect();
	}

	this->objects.clear();
	vecZero(&this->pCenter);
	this->Notify();
}

void struct3D::SubSelectionCollection::move(float dx, float dy, float dz)
{
	if(this->objects.size() == 0) return;

	this->pWorld.x += dx;
	this->pWorld.y += dy;
	this->pWorld.z += dz;

	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects[i]->move(dx, dy, dz);
	}
}
void struct3D::SubSelectionCollection::rotate(VECTOR4D_PTR vAngles)
{
	MATRIX4X4 mrot;
	POINT4D pCenter2;
	float size = this->objects.size();
	size = 1 / size;

	vecScale(size, &this->pCenter, &pCenter2);

	mrot.Build_XYZ_Rotation_MATRIX4X4(vAngles->x, vAngles->y, vAngles->z);

	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects[i]->rotate(vAngles, &pCenter2, &mrot);
	}
}

void struct3D::SubSelectionCollection::rotate(MATRIX4X4_PTR mrot)
{
	POINT4D pCenter2;
	float size = this->objects.size();
	size = 1 / size;

	vecScale(size, &this->pCenter, &pCenter2);

	for(int i = 0; i < this->objects.size(); i++)
	{
		this->objects[i]->rotate(0, &pCenter2, mrot);
	}
}

int struct3D::SubSelectionCollection::getCurrent()
{
	return this->objects.size();
}

void struct3D::SubSelectionCollection::getState(std::vector<float> *data_out)
{
	if(!data_out) return;

	data_out->clear();

	data_out->push_back(this->objects.size());
}

void struct3D::frameSelect::setFirstPoint(POINT2D_PTR p_in)
{
	vecCopy(p_in, &this->pLT);
}
void struct3D::frameSelect::setNextPoint(POINT2D_PTR p_in)
{
	vecCopy(p_in, &this->pRB);
}

void struct3D::frameSelect::getLeftTop(POINT2D_PTR p_out)
{
	vecCopy(&this->pLT, p_out);
}
void struct3D::frameSelect::getLeftBottom(POINT2D_PTR p_out)
{
	POINT2D pTmp;

	pTmp.x = this->pLT.x;
	pTmp.y = this->pRB.y;

	vecCopy(&pTmp, p_out);
}
void struct3D::frameSelect::getRightTop(POINT2D_PTR p_out)
{
	POINT2D pTmp;

	pTmp.y = this->pLT.y;
	pTmp.x = this->pRB.x;

	vecCopy(&pTmp, p_out);
}
void struct3D::frameSelect::getRightBottom(POINT2D_PTR p_out)
{
	vecCopy(&this->pRB, p_out);
}
//NEW