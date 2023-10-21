// 🐚
#include "StdAfx.h"
#include "Pipeline.h"

#include <algorithm>

Pipeline::Pipeline(void)
{
}


Pipeline::~Pipeline(void)
{
}

/*void Pipeline::ResetOBJECT4D(OBJECT4D_PTR obj)
{
	obj->state &= ~struct3D::OBJECT4D_STATE_CULLED; //reset bit

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4D_PTR currPoly = &obj->plist[poly];

		if(!(currPoly->state & struct3D::POLY4D_STATE_ACTIVE)) continue;

		currPoly->state &= ~struct3D::POLY4D_STATE_CLIPPED;
		currPoly->state &= ~struct3D::POLY4D_STATE_BACKFACE;
	}
}*/

void Pipeline::ConvertFromHomogenerous4DOBJECT4D(OBJECT4D_PTR obj)
{
	for(int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		obj->vlist_trans[vertex].x /= obj->vlist_trans[vertex].w;
		obj->vlist_trans[vertex].y /= obj->vlist_trans[vertex].w;
		obj->vlist_trans[vertex].z /= obj->vlist_trans[vertex].w;
	}
}

void Pipeline::ConvertFromHomogenerous4DRENDERLIST4D(RENDERLIST4D_PTR rendList)
{
	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

		for(int vertex = 0; vertex < 3; vertex++)
		{
			currPoly->tvlist[vertex].x /= currPoly->tvlist[vertex].w;
			currPoly->tvlist[vertex].y /= currPoly->tvlist[vertex].w;
			currPoly->tvlist[vertex].z /= currPoly->tvlist[vertex].w;
		}
	}
}

/*void Pipeline::TransformRENDERLIST4D(RENDERLIST4D_PTR rendList, MATRIX4X4_PTR mt, coordSelect enm)
{
	switch(enm)
	{
	case coordSelect::TRANSFORM_LOCAL_ONLY:
		{
			for(int poly = 0; poly < rendList->num_polys; poly++)
			{
				POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					POINT4D pResult;

					currPoly->vlist[vertex].Mat_Mul_VECTOR4D_4X4(mt, &pResult);

					pResult.VECTOR4D_COPY(&currPoly->vlist[vertex]);
				}
			}
		}break;
	case coordSelect::TRANSFORM_TRANS_ONLY:
		{
			for(int poly = 0; poly < rendList->num_polys; poly++)
			{
				POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					POINT4D pResult;

					currPoly->tvlist[vertex].Mat_Mul_VECTOR4D_4X4(mt, &pResult);

					pResult.VECTOR4D_COPY(&currPoly->tvlist[vertex]);
				}
			}
		}break;
	case coordSelect::TRANSFORM_LOCAL_TO_TRANS:
		{
			for(int poly = 0; poly < rendList->num_polys; poly++)
			{
				POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

				if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
					continue;

				for(int vertex = 0; vertex < 3; vertex++)
				{
					currPoly->vlist[vertex].Mat_Mul_VECTOR4D_4X4(mt, &currPoly->tvlist[vertex]);
				}
			}
		}break;
	default: break;
	}
}


void Pipeline::TransformOBJECT4D(OBJECT4D_PTR obj, MATRIX4X4_PTR mt, coordSelect enm, bool transformBasis)
{
	switch(enm)
	{
	case coordSelect::TRANSFORM_LOCAL_ONLY:
		{
			for(int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				POINT4D pResult;

				obj->vlist_local[vertex].Mat_Mul_VECTOR4D_4X4(mt, &pResult);
				pResult.VECTOR4D_COPY(&obj->vlist_local[vertex]);
			}
		}break;
	case coordSelect::TRANSFORM_TRANS_ONLY:
		{
			for(int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				POINT4D pResult;

				obj->vlist_trans[vertex].Mat_Mul_VECTOR4D_4X4(mt, &pResult);
				pResult.VECTOR4D_COPY(&obj->vlist_trans[vertex]);
			}
		}break;
	case coordSelect::TRANSFORM_LOCAL_TO_TRANS:
		{
			for(int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				obj->vlist_local[vertex].Mat_Mul_VECTOR4D_4X4(mt, &obj->vlist_trans[vertex]);
			}
		}break;
	default: break;
	}

	if(transformBasis)
	{
		VECTOR4D vResult;

		obj->ux.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&obj->ux);

		obj->uy.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&obj->uy);

		obj->uz.Mat_Mul_VECTOR4D_4X4(mt, &vResult);
		vResult.VECTOR4D_COPY(&obj->uz);
	}
}*/

void Pipeline::ModelToWorldOBJECT4D(OBJECT4D_PTR obj, int all_frames, coordSelect enm)
{
	if(!all_frames)
	{
		if(enm == coordSelect::TRANSFORM_LOCAL_TO_TRANS)
		{
			for(int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				obj->vlist_local[vertex].v.VECTOR4D_Add(&obj->world_pos, &obj->vlist_trans[vertex].v);

				obj->vlist_local[vertex].n.VECTOR4D_COPY(&obj->vlist_trans[vertex].n);
			}
		}
		else //coordSelect::TRANSFORM_TRANS_ONLY
		{
			for(int vertex = 0; vertex < obj->num_vertices; vertex++)
			{
				obj->vlist_trans[vertex].v.VECTOR4D_Add(&obj->world_pos, &obj->vlist_trans[vertex].v);
			}
		}
	}
	else
	{
		if(enm == coordSelect::TRANSFORM_LOCAL_TO_TRANS)
		{
			for(int vertex = 0; vertex < obj->total_vertices; vertex++)
			{
				obj->head_vlist_local[vertex].v.VECTOR4D_Add(&obj->world_pos, &obj->head_vlist_trans[vertex].v);

				obj->head_vlist_local[vertex].n.VECTOR4D_COPY(&obj->head_vlist_trans[vertex].n);
			}
		}
		else //coordSelect::TRANSFORM_TRANS_ONLY
		{
			for(int vertex = 0; vertex < obj->total_vertices; vertex++)
			{
				obj->head_vlist_trans[vertex].v.VECTOR4D_Add(&obj->world_pos, &obj->head_vlist_trans[vertex].v);
			}
		}
	}
}

/*void Pipeline::ModelToWorldRENDERLIST4D(RENDERLIST4D_PTR rendList, POINT4D_PTR worldPos, coordSelect enm)
{
	if(enm == coordSelect::TRANSFORM_LOCAL_TO_TRANS)
	{
		for(int poly = 0; poly < rendList->num_polys; poly++)
		{
			POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

			if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
				continue;

			for(int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->vlist[vertex].VECTOR4D_Add(worldPos, &currPoly->tvlist[vertex]);
			}
		}
	}
	else //coordSelect::TRANSFORM_TRANS_ONLY
	{
		for(int poly = 0; poly < rendList->num_polys; poly++)
		{
			POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

			if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
				continue;

			for(int vertex = 0; vertex < 3; vertex++)
			{
				currPoly->tvlist[vertex].VECTOR4D_Add(worldPos, &currPoly->tvlist[vertex]);
			}
		}
	}
}*/

int Pipeline::CullOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam, cullObject enm)
{
	POINT4D spherePos;

	obj->world_pos.Mat_Mul_VECTOR4D_4X4(&cam->mcam, &spherePos);

	if(enm & cullObject::CULL_OBJECT_Z_PLANE)
	{
		if(((spherePos.z - obj->max_radius[obj->curr_frame]) > cam->far_clip_z) || ((spherePos.z + obj->max_radius[obj->curr_frame]) < cam->near_clip_z))
		{
			obj->state |= struct3D::OBJECT4D_STATE_CULLED;
			return 1;
		}
	}

	if(enm & cullObject::CULL_OBJECT_X_PLANE)
	{
		float zTest = 0.5f * cam->viewplane_width * spherePos.z / cam->view_dist;

		if(((spherePos.x - obj->max_radius[obj->curr_frame]) > zTest) || ((spherePos.x + obj->max_radius[obj->curr_frame]) < -zTest))
		{
			obj->state |= struct3D::OBJECT4D_STATE_CULLED;
			return 1;
		}
	}

	if(enm & cullObject::CULL_OBJECT_Y_PLANE)
	{
		float zTest = 0.5f * cam->viewplane_height * spherePos.z / cam->view_dist;

		if(((spherePos.y - obj->max_radius[obj->curr_frame]) > zTest) || ((spherePos.y + obj->max_radius[obj->curr_frame]) < -zTest))
		{
			obj->state |= struct3D::OBJECT4D_STATE_CULLED;
			return 1;
		}
	}

	return 0;
}

void Pipeline::RemoveBackfacesOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam)
{
	if(obj->state & struct3D::OBJECT4D_STATE_CULLED) return;

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4D_PTR currPoly = &obj->plist[poly];

		if(!(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->attr & struct3D::POLY4D_ATTR_2SIDED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE)) continue;

		int vindex0 = currPoly->vert[0];
		int vindex1 = currPoly->vert[1];
		int vindex2 = currPoly->vert[2];

		VECTOR4D u, v, n;

		u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
		v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

		u.VECTOR4D_Cross(&v, &n);

		VECTOR4D view;

		view.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &cam->pos);

		float dp = n.VECTOR4D_Dot(&view);

		if(dp <= 0.0f)
			currPoly->state |= struct3D::POLY4D_STATE_BACKFACE;
	}
}

void Pipeline::RemoveBackfacesRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->attr & struct3D::POLY4D_ATTR_2SIDED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->attr & mat::MAT_ATTR_SHADE_MODE_1PLINE)) continue;

		VECTOR4D u, v, n;

		u.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v);
		v.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v);

		u.VECTOR4D_Cross(&v, &n);

		VECTOR4D view;

		view.VECTOR4D_Build(&currPoly->tvlist[0].v, &cam->pos);

		float dp = n.VECTOR4D_Dot(&view);

		if(dp <= 0.0f)
			currPoly->state |= struct3D::POLY4D_STATE_BACKFACE;
	}
}

int Pipeline::LightOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam)
{
	unsigned int rBase, gBase, bBase;
	unsigned int rSum, gSum, bSum;
	unsigned int rSum1, gSum1, bSum1;
	unsigned int rSum2, gSum2, bSum2;
	unsigned int shadedColor;

	float dp, //Скалярное произведение
		dist, //Расстояние от источника до поверхности
		i,    //Общие итенсивности
		nl,   //Длина нормали
		atten;//Коэфициент затухания

	if(!(obj->state & struct3D::OBJECT4D_STATE_ACTIVE) || (obj->state & struct3D::OBJECT4D_STATE_CULLED) || !(obj->state & struct3D::OBJECT4D_STATE_VISIBLE))
		return 0;

	for(int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4D_PTR currPoly = &obj->plist[poly];

		if(!(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE))
			continue;
		
		currPoly->state |= struct3D::POLY4D_STATE_LIT;

		int vindex0 = currPoly->vert[0];
		int vindex1 = currPoly->vert[1];
		int vindex2 = currPoly->vert[2];

		if(currPoly->attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT)
		{
			//предпологаеться что используеться 32 битный цвет

			rBase = (currPoly->color >> 16) & 0x000000FF;
			gBase = (currPoly->color >> 8) & 0x000000FF;
			bBase = currPoly->color & 0x000000FF;

			rSum = 0;
			gSum = 0;
			bSum = 0;

			for(int currLight = 0; currLight < lights.getNumLights(); currLight++)
			{
				if(lights.lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
				{
					rSum += (lights.lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
					gSum += (lights.lights[currLight].c_ambient.g * gBase) >> 8;
					bSum += (lights.lights[currLight].c_ambient.b * bBase) >> 8;
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
				{
					VECTOR4D u, v, n;

					u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
					v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

					u.VECTOR4D_Cross(&v, &n);

					nl = n.VECTOR4D_Length_Fast();
					dp  = n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						i = 128 * dp / nl;

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_POINT)
				{
					VECTOR4D u, v, n, l;

					u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
					v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

					u.VECTOR4D_Cross(&v, &n);

					nl = n.VECTOR4D_Length_Fast();

					l.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (nl * dist * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT1)
				{
					VECTOR4D u, v, n, l;

					u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
					v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

					v.VECTOR4D_Cross(&u, &n);

					nl = n.VECTOR4D_Length_Fast();

					l.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (nl * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT2)
				{
					VECTOR4D u, v, n, /*d,*/ s;

					u.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex1].v);
					v.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &obj->vlist_trans[vindex2].v);

					v.VECTOR4D_Cross(&u, &n);

					nl = n.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].pos, &obj->vlist_trans[vindex0].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpslExp = dpsl;

							for(int eIndex = 0; eIndex < (int)lights.lights[currLight].pf; eIndex++)
							{
								dpslExp *= dpsl;
							}

							i = 128 * dp * dpslExp / (nl * atten);

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}
				}
			}//end for

			if(rSum > 255) rSum = 255;
			if(gSum > 255) gSum = 255;
			if(bSum > 255) bSum = 255;

			shadedColor = ARGB32BIT(255, rSum, gSum, bSum);
			currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = shadedColor;
		}
		else if(currPoly->attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD)
		{
			int ri, gi, bi;

			RGBFROM32BIT(currPoly->color, rBase, gBase, bBase);

			rSum = rSum1 = rSum2 = 0;
			gSum = gSum1 = gSum2 = 0;
			bSum = bSum1 = bSum2 = 0;

			for(int currLight = 0; currLight < lights.getNumLights(); currLight++)
			{
				if(lights.lights[currLight].state == mat::LIGHT_STATE_OFF)
					continue;

				if(lights.lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
				{
					ri = ((lights.lights[currLight].c_ambient.r * rBase) >> 8);
					gi = ((lights.lights[currLight].c_ambient.g * gBase) >> 8);
					bi = ((lights.lights[currLight].c_ambient.b * bBase) >> 8);

					rSum += ri; rSum1 += ri; rSum2 += ri;
					gSum += gi; gSum1 += gi; gSum2 += gi;
					bSum += bi; bSum1 += bi; bSum2 += bi;
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
				{
					dp = obj->vlist_trans[vindex0].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						i = 128 * dp;
						
						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex1].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						i = 128 * dp;

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex2].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						i = 128 * dp;

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_POINT)
				{
					VECTOR4D l;

					l.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = obj->vlist_trans[vindex0].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (dist * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex1].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (dist * atten);

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex2].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (dist * atten);

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}//POINT
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT1)
				{
					VECTOR4D l;

					l.VECTOR4D_Build(&obj->vlist_trans[vindex0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = obj->vlist_trans[vindex0].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex1].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = obj->vlist_trans[vindex2].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}//SPOTLIGHT1
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT2)
				{
					VECTOR4D s;
					
					dp = obj->vlist_trans[vindex0].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].pos, &obj->vlist_trans[vindex0].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}

					dp = obj->vlist_trans[vindex1].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].pos, &obj->vlist_trans[vindex1].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}

					dp = obj->vlist_trans[vindex2].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].pos, &obj->vlist_trans[vindex2].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}
				}//SPOTLIGHT2
			}//end for

			if(rSum > 255) rSum = 255;
			if(gSum > 255) gSum = 255;
			if(bSum > 255) bSum = 255;
			if(rSum1 > 255) rSum1 = 255;
			if(gSum1 > 255) gSum1 = 255;
			if(bSum1 > 255) bSum1 = 255;
			if(rSum2 > 255) rSum2 = 255;
			if(gSum2 > 255) gSum2 = 255;
			if(bSum2 > 255) bSum2 = 255;

			currPoly->lit_color[0] = ARGB32BIT(255, rSum, gSum, bSum);
			currPoly->lit_color[1] = ARGB32BIT(255, rSum1, gSum1, bSum1);
			currPoly->lit_color[2] = ARGB32BIT(255, rSum2, gSum2, bSum2);
		}
		else
		{
			currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = currPoly->color;
		}
	}

	return 1;
}

int Pipeline::LightRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	unsigned int rBase, gBase, bBase;
	unsigned int rSum, gSum, bSum;
	unsigned int rSum1, gSum1, bSum1;
	unsigned int rSum2, gSum2, bSum2;
	unsigned int shadedColor;

	float dp, //Скалярное произведение
		dist, //Расстояние от источника до поверхности
		i,    //Общие итенсивности
		nl,   //Длина нормали
		atten;//Коэфициент затухания

	MATRIX4X4 mr;

	matCopy(&cam->mcam, &mr);

	mr.M30 = mr.M31 = mr.M32 = 0.0f;

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if(!(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_LIT) || (currPoly->attr & mat::MAT_ATTR_SHADE_MODE_1PLINE))
			continue;

		currPoly->state |= struct3D::POLY4D_STATE_LIT;

		if(currPoly->attr & struct3D::POLY4D_ATTR_SHADE_MODE_FLAT)
		{
			//предпологаеться что используеться 32 битный цвет

			rBase = (currPoly->color >> 16) & 0x000000FF;
			gBase = (currPoly->color >> 8) & 0x000000FF;
			bBase = currPoly->color & 0x000000FF;

			rSum = 0;
			gSum = 0;
			bSum = 0;

			for(int currLight = 0; currLight < lights.getNumLights(); currLight++)
			{
				if(lights.lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
				{
					rSum += (lights.lights[currLight].c_ambient.r * rBase) >> 8; // (>> 8) == (/ 256)
					gSum += (lights.lights[currLight].c_ambient.g * gBase) >> 8;
					bSum += (lights.lights[currLight].c_ambient.b * bBase) >> 8;
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
				{
					VECTOR4D u, v, n, /*r,*/ /*c,*/ l;

					u.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v);
					v.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v);

					u.VECTOR4D_Cross(&v, &n);

					vecScale(-1, &lights.lights[currLight].tdir, &l);

					nl = n.VECTOR4D_Length_Fast();
					dp  = n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						i = /*128 * */dp / nl;

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 /** 128*/));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 /** 128*/));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 /** 128*/));
					}

					/*vecReflect(&lights.lights[currLight].tdir, &n, &r);
					vecBuild(

					float dpSpecular = vecDot(*/
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_POINT)
				{
					VECTOR4D u, v, n, l;

					u.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v);
					v.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v);

					u.VECTOR4D_Cross(&v, &n);

					nl = n.VECTOR4D_Length_Fast();

					l.VECTOR4D_Build(&currPoly->tvlist[0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (nl * dist * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT1)
				{
					VECTOR4D u, v, n, l;

					u.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v);
					v.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v);

					v.VECTOR4D_Cross(&u, &n);

					nl = n.VECTOR4D_Length_Fast();

					l.VECTOR4D_Build(&currPoly->tvlist[0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / (nl * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT2)
				{
					VECTOR4D u, v, n, /*d,*/ s;

					u.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v);
					v.VECTOR4D_Build(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v);

					v.VECTOR4D_Cross(&u, &n);

					nl = n.VECTOR4D_Length_Fast();

					dp = n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].tpos, &currPoly->tvlist[0].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpslExp = dpsl;

							for(int eIndex = 0; eIndex < (int)lights.lights[currLight].pf; eIndex++)
							{
								dpslExp *= dpsl;
							}

							i = 128 * dp * dpslExp / (nl * atten);

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}
				}
			}//end for

			if(rSum > 255) rSum = 255;
			if(gSum > 255) gSum = 255;
			if(bSum > 255) bSum = 255;

			shadedColor = ARGB32BIT(255, rSum, gSum, bSum);
			currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = shadedColor;

			//MessageBox(0,L"FLAT",0,0);
		}
		else if(currPoly->attr & struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD)
		{
			int ri, gi, bi;

			RGBFROM32BIT(currPoly->color, rBase, gBase, bBase);

			rSum = rSum1 = rSum2 = 0;
			gSum = gSum1 = gSum2 = 0;
			bSum = bSum1 = bSum2 = 0;

			/*for(int vertex = 0; vertex < 3; vertex++)
			{
				POINT4D pResult;

				currPoly->tvlist[vertex].n.Mat_Mul_VECTOR4D_4X4(&mr, &pResult);

				pResult.VECTOR4D_COPY(&currPoly->tvlist[vertex].n);
			}*/

			for(int currLight = 0; currLight < lights.getNumLights(); currLight++)
			{
				if(lights.lights[currLight].state == mat::LIGHT_STATE_OFF)
					continue;

				if(lights.lights[currLight].attr & mat::LIGHT_ATTR_AMBIENT)
				{
					ri = ((lights.lights[currLight].c_ambient.r * rBase) >> 8);
					gi = ((lights.lights[currLight].c_ambient.g * gBase) >> 8);
					bi = ((lights.lights[currLight].c_ambient.b * bBase) >> 8);

					rSum += ri; rSum1 += ri; rSum2 += ri;
					gSum += gi; gSum1 += gi; gSum2 += gi;
					bSum += bi; bSum1 += bi; bSum2 += bi;
				}//AMBIENT
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_INFINITE)
				{
					VECTOR4D c, r, l;
					float dpSpec = 0;

					vecScale(-1, &lights.lights[currLight].tdir, &l);

					dp = currPoly->tvlist[0].n.VECTOR4D_Dot(&l);
					//dpSpec = max(dpSpec, 0);

					if(dp > 0)
					{
						i = /*128 * */dp;
						
						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256/* * 128*/));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256/* * 128*/));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256/* * 128*/));

						vecReflect(&l, &currPoly->tvlist[0].n, &r);
						c.w = 0;
						c.x = /*0 -*/ currPoly->tvlist[0].v.x;
						c.y = /*0 -*/ currPoly->tvlist[0].v.y;
						c.z = /*0 -*/ currPoly->tvlist[0].v.z;
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp;

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / (256/* * 128*/));
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / (256/* * 128*/));
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / (256/* * 128*/));
						}
					}

					dp = currPoly->tvlist[1].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						i = dp;

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256));

						vecReflect(&l, &currPoly->tvlist[1].n, &r);
						c.w = 0;
						c.x = /*0 -*/ currPoly->tvlist[1].v.x;
						c.y = /*0 -*/ currPoly->tvlist[1].v.y;
						c.z = /*0 -*/ currPoly->tvlist[1].v.z;
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp;

							rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / (256/* * 128*/));
							gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / (256/* * 128*/));
							bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / (256/* * 128*/));
						}
					}

					dp = currPoly->tvlist[2].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						i = dp;

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256));

						vecReflect(&l, &currPoly->tvlist[2].n, &r);
						c.w = 0;
						c.x = /*0 -*/ currPoly->tvlist[2].v.x;
						c.y = /*0 -*/ currPoly->tvlist[2].v.y;
						c.z = /*0 -*/ currPoly->tvlist[2].v.z;
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp;

							rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / (256/* * 128*/));
							gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / (256/* * 128*/));
							bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / (256/* * 128*/));
						}
					}
				}//INFINITE
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_POINT)
				{
					VECTOR4D l;
					VECTOR4D c, r;
					float dpSpec = 0;

					l.VECTOR4D_Build(&currPoly->tvlist[0].v, &lights.lights[currLight].tpos);
					//dist = 1;
					//dist = currPoly->tvlist[0].v.z - lights.lights[currLight].tpos.z;
					//dist = abs(dist);
					dist = l.VECTOR4D_Length_Fast();
					vecNormalize(&l);
					dp = currPoly->tvlist[0].n.VECTOR4D_Dot(&l);

					float kd = 0.01f;

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						//i = 128 * dp / (/*dist **/ atten);
						i = dp / atten;//(dist * atten);

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256));

						vecReflect(&l, &currPoly->tvlist[0].n, &r);
						vecCopy(&currPoly->tvlist[0].v, &c);
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp / atten;/// (dist * atten);

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / 256);
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / 256);
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / 256);
						}
					}

					l.VECTOR4D_Build(&currPoly->tvlist[1].v, &lights.lights[currLight].tpos);

					//dist = currPoly->tvlist[1].v.z - lights.lights[currLight].tpos.z;
					//dist = abs(dist);
					dist = l.VECTOR4D_Length_Fast();
					vecNormalize(&l);
					dp = currPoly->tvlist[1].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						//i = 128 * dp / (/*dist **/ atten);
						i = dp / atten;//(dist * atten);

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256));

						vecReflect(&l, &currPoly->tvlist[1].n, &r);
						vecCopy(&currPoly->tvlist[1].v, &c);
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp / atten;/// (dist * atten);

							rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / 256);
							gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / 256);
							bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / 256);
						}
					}

					l.VECTOR4D_Build(&currPoly->tvlist[2].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();
					//dist = currPoly->tvlist[2].v.z - lights.lights[currLight].tpos.z;
					//dist = abs(dist);
					vecNormalize(&l);
					dp = currPoly->tvlist[2].n.VECTOR4D_Dot(&l);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						//i = 128 * dp / (/*dist **/ atten);
						//i = 128 * dp / (dist * atten);
						i = dp / atten;//(dist * atten);

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256));

						vecReflect(&l, &currPoly->tvlist[2].n, &r);
						vecCopy(&currPoly->tvlist[2].v, &c);
						vecNormalize(&r);
						vecNormalize(&c);
						dpSpec = vecDot(&r, &c);

						if(dpSpec > 0)
						{
							dpSpec = pow(dpSpec, lights.lights[currLight].pf);
							i = dpSpec * dp / atten;/// (dist * atten);

							rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.r * rBase * i) / 256);
							gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.g * gBase * i) / 256);
							bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_specular.b * bBase * i) / 256);
						}
					}
				}//POINT
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT1)
				{
					VECTOR4D l;

					l.VECTOR4D_Build(&currPoly->tvlist[0].v, &lights.lights[currLight].tpos);

					dist = l.VECTOR4D_Length_Fast();

					dp = currPoly->tvlist[0].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = currPoly->tvlist[1].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}

					dp = currPoly->tvlist[2].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

						i = 128 * dp / atten;

						rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
						gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
						bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
					}
				}//SPOTLIGHT1
				else if(lights.lights[currLight].attr & mat::LIGHT_ATTR_SPOTLIGHT2)
				{
					VECTOR4D s;
					
					dp = currPoly->tvlist[0].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].tpos, &currPoly->tvlist[0].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}

					dp = currPoly->tvlist[1].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].tpos, &currPoly->tvlist[1].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum1 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}

					dp = currPoly->tvlist[2].n.VECTOR4D_Dot(&lights.lights[currLight].tdir);

					if(dp > 0)
					{
						s.VECTOR4D_Build(&lights.lights[currLight].tpos, &currPoly->tvlist[2].v);

						dist = s.VECTOR4D_Length_Fast();

						float dpsl = s.VECTOR4D_Dot(&lights.lights[currLight].tdir) / dist;

						if(dpsl > 0)
						{
							atten = (lights.lights[currLight].kc + lights.lights[currLight].kl * dist + lights.lights[currLight].kq * dist * dist);

							float dpsl_exp = dpsl;

							for(int e_index = 1; e_index < (int)lights.lights[currLight].pf; e_index++)
								dpsl_exp *= dpsl;

							i = 128 * dp * dpsl_exp / atten;

							rSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.r * rBase * i) / (256 * 128));
							gSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.g * gBase * i) / (256 * 128));
							bSum2 += static_cast<uint32_t>((lights.lights[currLight].c_diffuse.b * bBase * i) / (256 * 128));
						}
					}
				}//SPOTLIGHT2
			}//end for

			if(rSum > 255) rSum = 255;
			if(gSum > 255) gSum = 255;
			if(bSum > 255) bSum = 255;
			if(rSum1 > 255) rSum1 = 255;
			if(gSum1 > 255) gSum1 = 255;
			if(bSum1 > 255) bSum1 = 255;
			if(rSum2 > 255) rSum2 = 255;
			if(gSum2 > 255) gSum2 = 255;
			if(bSum2 > 255) bSum2 = 255;

			currPoly->lit_color[0] = ARGB32BIT(255, rSum, gSum, bSum);
			currPoly->lit_color[1] = ARGB32BIT(255, rSum1, gSum1, bSum1);
			currPoly->lit_color[2] = ARGB32BIT(255, rSum2, gSum2, bSum2);
			//MessageBox(0,L"GOURAUD",0,0);
		}
		else if(currPoly->attr & struct3D::POLY4D_ATTR_SHADE_MODE_PHONG)
		{
			//MessageBox(0,L"PHONG",0,0);
			currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = currPoly->color;
			//Все вычисление будут производиться при растеризации, т.к. нужно вычислить(интерполировать) нормаль к каждому пикселю треугольника и осветить его
		}
		else
		{
			//MessageBox(0,L"NULL",0,0);
			currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = currPoly->color;
		}
	}

	return 1;
}

void Pipeline::WorldToCameraOBJECT4D(CAM4D_PTR cam, OBJECT4D_PTR obj)
{
	for(int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		POINT4D pResult;

		obj->vlist_trans[vertex].v.Mat_Mul_VECTOR4D_4X4(&cam->mcam, &pResult);

		pResult.VECTOR4D_COPY(&obj->vlist_trans[vertex].v);
	}
}
void Pipeline::WorldToCameraRENDERLIST4D(CAM4D_PTR cam, RENDERLIST4D_PTR rendList)
{
	/*MATRIX4X4 mr;

	math3D::mathObj->matCopy(&cam->mcam, &mr);

	mr.M30 = mr.M31 = mr.M32 = 0.0f;*/

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

		for(int vertex = 0; vertex < 3; vertex++)
		{
			POINT4D pResult;

			currPoly->worldPos[vertex].x = currPoly->tvlist[vertex].v.x;
			currPoly->worldPos[vertex].y = currPoly->tvlist[vertex].v.y;
			currPoly->worldPos[vertex].z = currPoly->tvlist[vertex].v.z;

			currPoly->tvlist[vertex].v.Mat_Mul_VECTOR4D_4X4(&cam->mcam, &pResult);

			pResult.VECTOR4D_COPY(&currPoly->tvlist[vertex].v);

			/*currPoly->tvlist[vertex].n.Mat_Mul_VECTOR4D_4X4(&mr, &pResult);

			pResult.VECTOR4D_COPY(&currPoly->tvlist[vertex].n);*/
		}
	}
}

void Pipeline::ClipPolysRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam, clipPoly enm)
{
	const int CLIP_CODE_GZ = 1;
	const int CLIP_CODE_LZ = 2;
	const int CLIP_CODE_IZ = 4;

	const int CLIP_CODE_GX = 1;
	const int CLIP_CODE_LX = 2;
	const int CLIP_CODE_IX = 4;

	const int CLIP_CODE_GY = 1;
	const int CLIP_CODE_LY = 2;
	const int CLIP_CODE_IY = 4;

	const int CLIP_CODE_NULL = 0;

	int vertex_ccodes[3];
	int num_verts_in;
	int v0, v1, v2;

	float z_factor, z_test;

	math3D::VECTOR2D XYi, XY01i, XY02i;
	math3D::VECTOR2D T;
	math3D::VECTOR2D UVi, UV01i, UV02i;

	math3D::VECTOR4D Ni, N01i, N02i;

	int last_poly_index, insert_poly_index;
	math3D::VECTOR4D u, v, n;
	struct3D::POLYF4D temp_poly;

	insert_poly_index = last_poly_index = rendList->num_polys;

	for(int poly = 0; poly < last_poly_index; poly++)
	{
		struct3D::POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == 0) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->attr & mat::MAT_ATTR_SHADE_MODE_1PLINE))
			continue;

		if(enm & clipPoly::CLIP_POLY_X_PLANE)
		{
			z_factor = (0.5f) * cam->viewplane_width / cam->view_dist;
			z_test = z_factor * currPoly->tvlist[0].z;

			if(currPoly->tvlist[0].x > z_test)
				vertex_ccodes[0] = CLIP_CODE_GX;
			else if(currPoly->tvlist[0].x < -z_test)
				vertex_ccodes[0] = CLIP_CODE_LX;
			else
				vertex_ccodes[0] = CLIP_CODE_IX;

			z_test = z_factor * currPoly->tvlist[1].z;

			if(currPoly->tvlist[1].x > z_test)
				vertex_ccodes[1] = CLIP_CODE_GX;
			else if(currPoly->tvlist[1].x < -z_test)
				vertex_ccodes[1] = CLIP_CODE_LX;
			else
				vertex_ccodes[1] = CLIP_CODE_IX;

			z_test = z_factor * currPoly->tvlist[2].z;

			if(currPoly->tvlist[2].x > z_test)
				vertex_ccodes[2] = CLIP_CODE_GX;
			else if(currPoly->tvlist[2].x < -z_test)
				vertex_ccodes[2] = CLIP_CODE_LX;
			else
				vertex_ccodes[2] = CLIP_CODE_IX;

			if(((vertex_ccodes[0] == CLIP_CODE_GX) && (vertex_ccodes[1] == CLIP_CODE_GX) && (vertex_ccodes[2] == CLIP_CODE_GX)) || ((vertex_ccodes[0] == CLIP_CODE_LX) && (vertex_ccodes[1] == CLIP_CODE_LX) && (vertex_ccodes[2] == CLIP_CODE_LX)))
			{
				currPoly->state |= struct3D::POLY4D_STATE_CLIPPED;
				continue;
			}
		}
		if(enm & clipPoly::CLIP_POLY_Y_PLANE)
		{
			z_factor = (0.5f) * cam->viewplane_height / cam->view_dist;
			z_test = z_factor * currPoly->tvlist[0].z;

			if(currPoly->tvlist[0].y > z_test)
				vertex_ccodes[0] = CLIP_CODE_GY;
			else if(currPoly->tvlist[0].y < -z_test)
				vertex_ccodes[0] = CLIP_CODE_LY;
			else
				vertex_ccodes[0] = CLIP_CODE_IY;

			z_test = z_factor * currPoly->tvlist[1].z;

			if(currPoly->tvlist[1].y > z_test)
				vertex_ccodes[1] = CLIP_CODE_GY;
			else if(currPoly->tvlist[1].y < -z_test)
				vertex_ccodes[1] = CLIP_CODE_LY;
			else
				vertex_ccodes[1] = CLIP_CODE_IY;

			z_test = z_factor * currPoly->tvlist[2].z;

			if(currPoly->tvlist[2].y > z_test)
				vertex_ccodes[2] = CLIP_CODE_GY;
			else if(currPoly->tvlist[2].y < -z_test)
				vertex_ccodes[2] = CLIP_CODE_LY;
			else
				vertex_ccodes[2] = CLIP_CODE_IY;

			if(((vertex_ccodes[0] == CLIP_CODE_GY) && (vertex_ccodes[1] == CLIP_CODE_GY) && (vertex_ccodes[2] == CLIP_CODE_GY)) || ((vertex_ccodes[0] == CLIP_CODE_LY) && (vertex_ccodes[1] == CLIP_CODE_LY) && (vertex_ccodes[2] == CLIP_CODE_LY)))
			{
				currPoly->state |= struct3D::POLY4D_STATE_CLIPPED;
				continue;
			}
		}
		if(enm & clipPoly::CLIP_POLY_Z_PLANE)
		{
			num_verts_in = 0;

			if(currPoly->tvlist[0].z > cam->far_clip_z)
				vertex_ccodes[0] = CLIP_CODE_GZ;
			else if(currPoly->tvlist[0].z < cam->near_clip_z)
				vertex_ccodes[0] = CLIP_CODE_LZ;
			else
			{
				vertex_ccodes[0] = CLIP_CODE_IZ;
				num_verts_in++;
			}

			if(currPoly->tvlist[1].z > cam->far_clip_z)
				vertex_ccodes[1] = CLIP_CODE_GZ;
			else if(currPoly->tvlist[1].z < cam->near_clip_z)
				vertex_ccodes[1] = CLIP_CODE_LZ;
			else
			{
				vertex_ccodes[1] = CLIP_CODE_IZ;
				num_verts_in++;
			}

			if(currPoly->tvlist[2].z > cam->far_clip_z)
				vertex_ccodes[2] = CLIP_CODE_GZ;
			else if(currPoly->tvlist[2].z < cam->near_clip_z)
				vertex_ccodes[2] = CLIP_CODE_LZ;
			else
			{
				vertex_ccodes[2] = CLIP_CODE_IZ;
				num_verts_in++;
			}

			if(((vertex_ccodes[0] == CLIP_CODE_GZ) && (vertex_ccodes[1] == CLIP_CODE_GZ) && (vertex_ccodes[2] == CLIP_CODE_GZ)) || ((vertex_ccodes[0] == CLIP_CODE_LZ) && (vertex_ccodes[1] == CLIP_CODE_LZ) && (vertex_ccodes[2] == CLIP_CODE_LZ)))
			{
				currPoly->state |= struct3D::POLY4D_STATE_CLIPPED;
				continue;
			}

			if((vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ)
			{
				if(num_verts_in == 1)
				{
					if(vertex_ccodes[0] == CLIP_CODE_IZ)
					{
						v0 = 0; v1 = 1; v2 = 2;
					}
					else if(vertex_ccodes[1] == CLIP_CODE_IZ)
					{
						v0 = 1; v1 = 2; v2 = 0;
					}
					else
					{
						v0 = 2; v1 = 0; v2 = 1;
					}

					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v1].v, &v);

					T.M[0] = (cam->near_clip_z - currPoly->tvlist[v0].z) / v.z;

					XYi.x = currPoly->tvlist[v0].x + v.x * T.M[0];
					XYi.y = currPoly->tvlist[v0].y + v.y * T.M[0];

					currPoly->tvlist[v1].x = XYi.x;
					currPoly->tvlist[v1].y = XYi.y;
					currPoly->tvlist[v1].z = cam->near_clip_z;
					currPoly->tvlist[v1].attr |= VERTEX4DT_ATTR_GENERATED;

					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v2].v, &v);

					T.M[1] = (cam->near_clip_z - currPoly->tvlist[v0].z) / v.z;

					XYi.x = currPoly->tvlist[v0].x + v.x * T.M[1];
					XYi.y = currPoly->tvlist[v0].y + v.y * T.M[1];

					currPoly->tvlist[v2].x = XYi.x;
					currPoly->tvlist[v2].y = XYi.y;
					currPoly->tvlist[v2].z = cam->near_clip_z;
					currPoly->tvlist[v2].attr |= VERTEX4DT_ATTR_GENERATED;

					if(currPoly->attr & mat::MAT_ATTR_SHADE_MODE_GOURAUD)
					{
						Ni.x = currPoly->tvlist[v0].nx + (currPoly->tvlist[v1].nx - currPoly->tvlist[v0].nx) * T.M[0];
						Ni.y = currPoly->tvlist[v0].ny + (currPoly->tvlist[v1].ny - currPoly->tvlist[v0].ny) * T.M[0];
						Ni.z = currPoly->tvlist[v0].nz + (currPoly->tvlist[v1].nz - currPoly->tvlist[v0].nz) * T.M[0];
						vecCopy(&Ni, &currPoly->tvlist[v1].n);

						Ni.x = currPoly->tvlist[v0].nx + (currPoly->tvlist[v2].nx - currPoly->tvlist[v0].nx) * T.M[1];
						Ni.y = currPoly->tvlist[v0].ny + (currPoly->tvlist[v2].ny - currPoly->tvlist[v0].ny) * T.M[1];
						Ni.z = currPoly->tvlist[v0].nz + (currPoly->tvlist[v2].nz - currPoly->tvlist[v0].nz) * T.M[1];
						vecCopy(&Ni, &currPoly->tvlist[v2].n);
					}

					if(currPoly->attr & mat::MAT_ATTR_SHADE_MODE_TEXTURE)
					{
						UVi.x = currPoly->tvlist[v0].u0 + (currPoly->tvlist[v1].u0 - currPoly->tvlist[v0].u0) * T.M[0];
						UVi.y = currPoly->tvlist[v0].v0 + (currPoly->tvlist[v1].v0 - currPoly->tvlist[v0].v0) * T.M[0];
						vecCopy(&UVi, &currPoly->tvlist[v1].t);

						UVi.x = currPoly->tvlist[v0].u0 + (currPoly->tvlist[v2].u0 - currPoly->tvlist[v0].u0) * T.M[1];
						UVi.y = currPoly->tvlist[v0].v0 + (currPoly->tvlist[v2].v0 - currPoly->tvlist[v0].v0) * T.M[1];
						vecCopy(&UVi, &currPoly->tvlist[v2].t);
					}

					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v1].v, &u);
					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v2].v, &v);

					vecCross(&u, &v, &n);

					currPoly->nlength = vecLengthFast(&n);
				}
				else if(num_verts_in == 2)
				{
					memcpy(&temp_poly, currPoly, sizeof(struct3D::POLYF4D));

					if(vertex_ccodes[0] == CLIP_CODE_LZ)
					{
						v0 = 0; v1 = 1; v2 = 2;
					}
					else if(vertex_ccodes[1] == CLIP_CODE_LZ)
					{
						v0 = 1; v1 = 2; v2 = 0;
					}
					else
					{
						v0 = 2; v1 = 0; v2 = 1;
					}

					vecBuild(&currPoly->tvlist[v1].v, &currPoly->tvlist[v0].v, &v);

					T.M[0] = (cam->near_clip_z - currPoly->tvlist[v1].z) / v.z;

					XY01i.x = currPoly->tvlist[v1].x + v.x * T.M[0];
					XY01i.y = currPoly->tvlist[v1].y + v.y * T.M[0];

					vecBuild(&currPoly->tvlist[v2].v, &currPoly->tvlist[v0].v, &v);

					T.M[1] = (cam->near_clip_z - currPoly->tvlist[v2].z) / v.z;

					XY02i.x = currPoly->tvlist[v2].x + v.x * T.M[1];
					XY02i.y = currPoly->tvlist[v2].y + v.y * T.M[1];

					currPoly->tvlist[v0].x = XY01i.x;
					currPoly->tvlist[v0].y = XY01i.y;
					currPoly->tvlist[v0].z = cam->near_clip_z;
					currPoly->tvlist[v0].attr |= VERTEX4DT_ATTR_GENERATED;

					temp_poly.tvlist[v1].x = XY01i.x;
					temp_poly.tvlist[v1].y = XY01i.y;
					temp_poly.tvlist[v1].z = cam->near_clip_z;
					temp_poly.tvlist[v1].attr |= VERTEX4DT_ATTR_GENERATED;

					temp_poly.tvlist[v0].x = XY02i.x;
					temp_poly.tvlist[v0].y = XY02i.y;
					temp_poly.tvlist[v0].z = cam->near_clip_z;
					temp_poly.tvlist[v0].attr |= VERTEX4DT_ATTR_GENERATED;

					if(currPoly->attr & mat::MAT_ATTR_SHADE_MODE_GOURAUD)
					{
						N01i.x = currPoly->tvlist[v1].nx + (currPoly->tvlist[v0].nx - currPoly->tvlist[v1].nx) * T.M[0];
						N01i.y = currPoly->tvlist[v1].ny + (currPoly->tvlist[v0].ny - currPoly->tvlist[v1].ny) * T.M[0];
						N01i.z = currPoly->tvlist[v1].nz + (currPoly->tvlist[v0].nz - currPoly->tvlist[v1].nz) * T.M[0];

						N02i.x = currPoly->tvlist[v2].nx + (currPoly->tvlist[v0].nx - currPoly->tvlist[v2].nx) * T.M[1];
						N02i.y = currPoly->tvlist[v2].ny + (currPoly->tvlist[v0].ny - currPoly->tvlist[v2].ny) * T.M[1];
						N02i.z = currPoly->tvlist[v2].nz + (currPoly->tvlist[v0].nz - currPoly->tvlist[v2].nz) * T.M[1];

						vecCopy(&N01i, &currPoly->tvlist[v0].n);

						vecCopy(&N02i, &temp_poly.tvlist[v0].n);
						vecCopy(&N01i, &temp_poly.tvlist[v1].n);
					}

					if(currPoly->attr & mat::MAT_ATTR_SHADE_MODE_TEXTURE)
					{
						UV01i.x = currPoly->tvlist[v1].u0 + (currPoly->tvlist[v0].u0 - currPoly->tvlist[v1].u0) * T.M[0];
						UV01i.y = currPoly->tvlist[v1].v0 + (currPoly->tvlist[v0].v0 - currPoly->tvlist[v1].v0) * T.M[0];

						UV02i.x = currPoly->tvlist[v2].u0 + (currPoly->tvlist[v0].u0 - currPoly->tvlist[v2].u0) * T.M[1];
						UV02i.y = currPoly->tvlist[v2].v0 + (currPoly->tvlist[v0].v0 - currPoly->tvlist[v2].v0) * T.M[1];

						vecCopy(&UV01i, &currPoly->tvlist[v0].t);

						vecCopy(&UV02i, &temp_poly.tvlist[v0].t);
						vecCopy(&UV01i, &temp_poly.tvlist[v1].t);
					}

					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v1].v, &u);
					vecBuild(&currPoly->tvlist[v0].v, &currPoly->tvlist[v2].v, &v);

					vecCross(&u, &v, &n);

					currPoly->nlength = vecLengthFast(&n);

					vecBuild(&temp_poly.tvlist[v0].v, &temp_poly.tvlist[v1].v, &u);
					vecBuild(&temp_poly.tvlist[v0].v, &temp_poly.tvlist[v2].v, &v);

					vecCross(&u, &v, &n);

					temp_poly.nlength = vecLengthFast(&n);

					//temp_poly.color = ARGB32BIT(255, 255, 0, 0);

					rendList->Insert_POLYF4D_RENDERLIST4D(&temp_poly);
				}
			}
		}
	}
}

int Pipeline::compareAvgZPOLYF4D(const void *arg1, const void *arg2)
{
	float z1, z2;
	POLYF4D_PTR poly1, poly2;

	poly1 = *((POLYF4D_PTR *)arg1);
	poly2 = *((POLYF4D_PTR *)arg2);

	z1 = 0.33333f * (poly1->tvlist[0].z + poly1->tvlist[1].z + poly1->tvlist[2].z);
	z2 = 0.33333f * (poly2->tvlist[0].z + poly2->tvlist[1].z + poly2->tvlist[2].z);

	return (int)(z2 - z1);
}

void Pipeline::SortRENDERLIST4D(RENDERLIST4D_PTR rendList, sortMethod enm)
{
	//enm не используеться т.к. треугольники сортируються по среднему значению

	qsort((void *)rendList->poly_ptrs, rendList->num_polys, sizeof(POLYF4D_PTR), this->compareAvgZPOLYF4D);
}

void Pipeline::CameraToPerspectiveOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam)
{
	for(int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		float z = obj->vlist_trans[vertex].z;

		obj->vlist_trans[vertex].x = cam->view_dist * obj->vlist_trans[vertex].x / z;
		obj->vlist_trans[vertex].y = cam->view_dist * obj->vlist_trans[vertex].y * cam->aspect_ratio / z;
	}
}

#include <DirectXMath.h>

void Pipeline::CameraToPerspectiveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	float camFov = DirectX::XMConvertToRadians(cam->fov);
	float    SinFov;
	float    CosFov;
	DirectX::XMScalarSinCos(&SinFov, &CosFov, 0.5f * camFov);
	float Height22 = CosFov / SinFov;

	DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(cam->fov), cam->aspect_ratio, cam->near_clip_z, cam->far_clip_z);
	DirectX::XMMATRIX proj2 = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(cam->fov), cam->aspect_ratio, 3.f, cam->far_clip_z);

	proj2 = DirectX::XMMatrixPerspectiveLH(cam->viewplane_width, cam->viewplane_height, cam->view_dist, cam->far_clip_z);

	auto test_xmproj = [proj = proj](VERTEX4DT pt)
	{
		auto dxv = DirectX::XMVectorSet(pt.x, pt.y, pt.z, pt.w);
		auto dxvProj = DirectX::XMVector4Transform(dxv, proj);
		auto W = DirectX::XMVectorSplatW(dxvProj);
		auto dxvProj2 = DirectX::XMVectorDivide(dxvProj, W);

		auto res = pt;

		res.v.x = DirectX::XMVectorGetX(dxvProj2);
		res.v.y = DirectX::XMVectorGetY(dxvProj2);
		res.v.z = DirectX::XMVectorGetZ(dxvProj2);
		res.v.w = DirectX::XMVectorGetW(dxvProj2);

		return res;
	};
	auto test_xmproj2 = [proj = proj2](VERTEX4DT pt)
	{
		auto dxv = DirectX::XMVectorSet(pt.x, pt.y, pt.z, pt.w);
		auto dxvProj = DirectX::XMVector4Transform(dxv, proj);
		auto W = DirectX::XMVectorSplatW(dxvProj);
		auto dxvProj2 = DirectX::XMVectorDivide(dxvProj, W);

		auto res = pt;

		res.v.x = DirectX::XMVectorGetX(dxvProj2);
		res.v.y = DirectX::XMVectorGetY(dxvProj2);
		res.v.z = DirectX::XMVectorGetZ(dxvProj2);
		res.v.w = DirectX::XMVectorGetW(dxvProj2);

		return res;
	};
	auto test_xmclip2 = [proj = proj2](VERTEX4DT pt)
	{
		auto dxv = DirectX::XMVectorSet(pt.x, pt.y, pt.z, pt.w);
		auto dxvProj = DirectX::XMVector4Transform(dxv, proj);

		auto res = pt;

		res.v.x = DirectX::XMVectorGetX(dxvProj);
		res.v.y = DirectX::XMVectorGetY(dxvProj);
		res.v.z = DirectX::XMVectorGetZ(dxvProj);
		res.v.w = DirectX::XMVectorGetW(dxvProj);

		return res;
	};
	auto test_ndc = [proj = proj2](VERTEX4DT pt)
	{
		auto dxv = DirectX::XMVectorSet(pt.x, pt.y, pt.z, pt.w);
		auto W = DirectX::XMVectorSplatW(dxv);
		auto dxvProj2 = DirectX::XMVectorDivide(dxv, W);

		auto res = pt;

		res.v.x = DirectX::XMVectorGetX(dxvProj2);
		res.v.y = DirectX::XMVectorGetY(dxvProj2);
		res.v.z = DirectX::XMVectorGetZ(dxvProj2);
		res.v.w = DirectX::XMVectorGetW(dxvProj2);

		return res;
	};

	auto test_proj = [&cam](VERTEX4DT pt)
	{
		float z = 1.0f / pt.z;
		auto res = pt;

		res.x = cam->view_dist * pt.x * z;
		res.y = cam->view_dist * pt.y * cam->aspect_ratio * z;

		return res;
	};
	auto test_toscreen = [&cam](VERTEX4DT pt)
	{
		float alpha = (0.5f * cam->viewport_width - 0.5f);
		float beta = (0.5f * cam->viewport_height - 0.5f);
		auto res = pt;

		res.x = alpha + alpha * pt.x;
		res.y = beta - beta * pt.y;

		return res;
	};
	auto test_xmtoscreen = [&cam](VERTEX4DT pt)
	{
		const float HalfViewportWidth = cam->viewport_width * 0.5f;
		const float HalfViewportHeight = cam->viewport_height * 0.5f;
		DirectX::XMVECTOR Scale = DirectX::XMVectorSet(HalfViewportWidth, -HalfViewportHeight, 1.f, 0.0f);
		DirectX::XMVECTOR Offset = DirectX::XMVectorSet(HalfViewportWidth, HalfViewportHeight, 1.f, 0.0f);

		auto dxv = DirectX::XMVectorSet(pt.x, pt.y, pt.z, pt.w);
		auto Result = DirectX::XMVectorMultiplyAdd(dxv, Scale, Offset);

		auto res = pt;

		res.x = DirectX::XMVectorGetX(Result);
		res.y = DirectX::XMVectorGetY(Result);

		return res;
	};

	{
		VERTEX4DT testVertex = {};

		testVertex.x = cam->aspect_ratio;
		testVertex.y = 1.f;
		testVertex.z = 4.f;
		testVertex.w = 1.f;

		auto xmprojed = test_xmproj(testVertex);
		auto xmprojed2 = test_xmproj2(testVertex);

		auto xmprojed3 = test_xmclip2(testVertex);
		auto xmprojed4 = test_ndc(xmprojed3);

		auto projed = test_proj(testVertex);

		auto xmscreened = test_xmtoscreen(xmprojed);
		auto xmscreened_22 = test_toscreen(xmprojed);
		auto xmscreened2 = test_xmtoscreen(xmprojed2);
		auto xmscreened2_22 = test_toscreen(xmprojed2);
		auto screened = test_toscreen(projed);

		auto xmxmprojed4 = test_toscreen(xmprojed4);

		int stop = 234;
	}

	{
		std::vector<POLYF4D> curPolys;
		curPolys.reserve(rendList->num_polys);

		for (int poly = 0; poly < rendList->num_polys; poly++)
		{
			POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

			curPolys.push_back(*currPoly);
		}

		rendList->Reset_RENDERLIST4D();

		for (auto& poly : curPolys) {
			for (int vertex = 0; vertex < 3; vertex++) {
				auto toClipSpace = test_xmclip2(poly.tvlist[vertex]);
				poly.tvlist[vertex].v = toClipSpace.v;
			}
		}

		for (auto& poly : curPolys) {
			// TEST for skipping clip
			/*rendList->Insert_POLYF4D_RENDERLIST4D(&poly);
			continue;*/
			auto clipped = this->clipper.Clip(poly.tvlist[0].v, poly.tvlist[1].v, poly.tvlist[2].v);

			{
				auto p2 = poly;
				auto clippedNDC = clipped;

				for (auto& i : clippedNDC) {
					p2.tvlist[0].v = i;
					auto res = test_ndc(p2.tvlist[0]);
					i = res.v;
				}

				auto clippedScreen = clippedNDC;

				for (auto& i : clippedScreen) {
					float alpha = (0.5f * cam->viewport_width - 0.5f);
					float beta = (0.5f * cam->viewport_height - 0.5f);

					i.x = alpha + alpha * i.x;
					i.y = beta - beta * i.y;
				}

				int stop = 234;
			}

			if (clipped.size() > 2) {
				// triangulate event if 3 vertices because they may be clipped too
				for (size_t vertex = 2; vertex < clipped.size(); ++vertex) {
					auto newPoly = poly;

					newPoly.tvlist[0].v = clipped[0];
					newPoly.tvlist[1].v = clipped[vertex - 1];
					newPoly.tvlist[2].v = clipped[vertex - 0];

					rendList->Insert_POLYF4D_RENDERLIST4D(&newPoly);
				}
			}
			// TODO add logic for clipper to detect triangles inside
			//else if (clipped.size() == 3) {
			//	// inside, copy as is
			//	rendList->Insert_POLYF4D_RENDERLIST4D(&poly);
			//}
			else if (clipped.size() == 0) {
				// fully outside, do nothing
				int stop = 234;
			}
			else {
				// clipper error
				assert(false);
			}
		}

		for (int poly = 0; poly < rendList->num_polys; poly++) {
			POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED))
				continue;

			for (int vertex = 0; vertex < 3; vertex++) {
				auto v = currPoly->tvlist[vertex];
				auto res = test_ndc(currPoly->tvlist[vertex]);
				currPoly->tvlist[vertex] = res;
			}

			int stop = 234;
		}
	}

	if (false) {
		for (int poly = 0; poly < rendList->num_polys; poly++)
		{
			POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

			if ((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				/*double z = currPoly->tvlist[vertex].z;

				currPoly->tvlist[vertex].x = ((double)cam->view_dist * (double)currPoly->tvlist[vertex].x / z);
				currPoly->tvlist[vertex].y = ((double)cam->view_dist * (double)currPoly->tvlist[vertex].y * (double)cam->aspect_ratio / z);*/

				auto vCopy = currPoly->tvlist[vertex];
				auto dxv = DirectX::XMVectorSet(vCopy.x, vCopy.y, vCopy.z, vCopy.w);
				auto dxvProj = DirectX::XMVector4Transform(dxv, proj);
				auto W = DirectX::XMVectorSplatW(dxvProj);
				auto dxvProj2 = DirectX::XMVectorDivide(dxvProj, W);

				float z = 1.0f / currPoly->tvlist[vertex].z;

				float part_a_1 = cam->view_dist * currPoly->tvlist[vertex].y;
				float part_a_2 = cam->aspect_ratio * z;
				float part_a_3 = part_a_1 * part_a_2;

				float part_b_1 = cam->view_dist * currPoly->tvlist[vertex].y;
				float part_b_2 = part_b_1 * cam->aspect_ratio;
				float part_b_3 = part_b_2 * z;

				uint32_t z_binRep = *reinterpret_cast<uint32_t*>(&z);
				uint32_t ar_binRep = *reinterpret_cast<uint32_t*>(&cam->aspect_ratio);
				uint32_t y_binRep = *reinterpret_cast<uint32_t*>(&currPoly->tvlist[vertex].y);

				currPoly->tvlist[vertex].x = cam->view_dist * currPoly->tvlist[vertex].x * z;
				currPoly->tvlist[vertex].y = cam->view_dist * currPoly->tvlist[vertex].y * cam->aspect_ratio * z;

				int stop = 234;

				//cam->

				/*double z = 1.0 / currPoly->tvlist[vertex].z;

				currPoly->tvlist[vertex].x = ((double)cam->view_dist * (double)currPoly->tvlist[vertex].x * z);
				currPoly->tvlist[vertex].y = ((double)cam->view_dist * (double)currPoly->tvlist[vertex].y * (double)cam->aspect_ratio * z);*/
			}
		}
	}
}

void Pipeline::PerspectiveToScreenOBJECT4D(OBJECT4D_PTR obj, CAM4D_PTR cam)
{
	float alpha = (0.5f * cam->viewport_width - 0.5f);
	float beta = (0.5f * cam->viewport_height - 0.5f);

	for(int vertex = 0; vertex < obj->num_vertices; vertex++)
	{
		obj->vlist_trans[vertex].x = alpha + alpha * obj->vlist_trans[vertex].x;
		obj->vlist_trans[vertex].y = beta - beta * obj->vlist_trans[vertex].y;
	}
}

void Pipeline::PerspectiveToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	const float halfViewportWidth = 0.5f * cam->viewport_width;
	const float halfViewportHeight = 0.5f * cam->viewport_height;

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;


		for(int vertex = 0; vertex < 3; vertex++)
		{
			currPoly->tvlist[vertex].x = halfViewportWidth + halfViewportWidth * currPoly->tvlist[vertex].x;
			currPoly->tvlist[vertex].y = halfViewportHeight - halfViewportHeight * currPoly->tvlist[vertex].y;
		}
	}
}

void Pipeline::CameraToScreenRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	/*OBJECT4D_PTR obj;*/

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

		//obj = rendList->objRefs[currPoly->objID];

		for(int vertex = 0; vertex < 3; vertex++)
		{
			/*obj->scr_pts[currPoly->vert[vertex]].x =*/ currPoly->tvlist[vertex].x = cam->viewport_center_x + cam->view_dist2 * currPoly->tvlist[vertex].x / currPoly->tvlist[vertex].z;
			/*obj->scr_pts[currPoly->vert[vertex]].y =*/ currPoly->tvlist[vertex].y = cam->viewport_center_y - cam->view_dist2 * currPoly->tvlist[vertex].y / currPoly->tvlist[vertex].z;
		}
	}
}

void Pipeline::WorldToCamera_and_BackfaceRemoveRENDERLIST4D(RENDERLIST4D_PTR rendList, CAM4D_PTR cam)
{
	VECTOR4D u, v, n;
	VECTOR4D view;
	POINT4D pResult;
	MATRIX4X4 mr;

	//cam->Build_CAM4D_Matrix_Euler(struct3D::CAM_ROT_SEQ_ZYX);
	cam->Build_CAM4D_Matrix2();

	matCopy(&cam->mcam, &mr);

	mr.M30 = mr.M31 = mr.M32 = 0.0f;

	for(int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];
		float dp = 0.f;

		if((currPoly->attr & struct3D::POLY4D_ATTR_2SIDED)) goto vertPROC;

		if((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED) /*|| (currPoly->attr & struct3D::POLY4D_ATTR_2SIDED)*/ || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->attr & mat::MAT_ATTR_SHADE_MODE_1PLINE)) continue;

		vecBuild(&currPoly->tvlist[0].v, &currPoly->tvlist[1].v, &u);
		vecBuild(&currPoly->tvlist[0].v, &currPoly->tvlist[2].v, &v);

		vecCross(&u, &v, &n);

		vecBuild(&currPoly->tvlist[0].v, &cam->pos, &view);

		dp = vecDot(&n, &view);
		if(dp <= 0.0f)
			currPoly->state |= struct3D::POLY4D_STATE_BACKFACE;
		else
		{
vertPROC:
			for(int vertex = 0; vertex < 3; vertex++)
			{
				vecCopy(&currPoly->tvlist[vertex].v, &currPoly->worldPos[vertex]);

				vecMulMat(&currPoly->tvlist[vertex].v, &cam->mcam, &pResult);

				vecCopy(&pResult, &currPoly->tvlist[vertex].v);

				if(currPoly->attr & mat::MAT_ATTR_SHADE_MODE_GOURAUD)
				{
					vecMulMat(&currPoly->tvlist[vertex].n, &mr, &pResult);
					vecCopy(&pResult, &currPoly->tvlist[vertex].n);
					currPoly->lit_color[0] = currPoly->lit_color[1] = currPoly->lit_color[2] = currPoly->color; //Если освещение отключено!!!
				}

				
			}
		}
	}
}

void Pipeline::LocalToCameraAndClipOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam)
{
	const int CLIP_CODE_GZ = 1;
	const int CLIP_CODE_LZ = 2;
	const int CLIP_CODE_IZ = 4;

	const int CLIP_CODE_GX = 1;
	const int CLIP_CODE_LX = 2;
	const int CLIP_CODE_IX = 4;

	const int CLIP_CODE_GY = 1;
	const int CLIP_CODE_LY = 2;
	const int CLIP_CODE_IY = 4;

	const int CLIP_CODE_NULL = 0;

	int vertex_ccodes[2];
	int num_verts_in;
	MATRIX4X4 mt, mres;
	PLINE4D lTmp;
	VECTOR4D vTmp;
	int size = obj->vlocal_curr;

	/*ZeroMemory(&lTmp, sizeof(PLINE4D));*/
	vecZero(&lTmp.v0.v);
	vecZero(&lTmp.v1.v);

	matIdentity(&mt);
	mt.M30 = obj->world_pos.x;
	mt.M31 = obj->world_pos.y;
	mt.M32 = obj->world_pos.z;

	cam->Build_CAM4D_Matrix2();

	matMul(&mt, &cam->mcam, &mres);

	for(int i = 0; i < size; i++)
	{
		vecMulMat(&obj->vlocal[i].v0.v, &mres, &lTmp.v0.v);
		vecMulMat(&obj->vlocal[i].v1.v, &mres, &lTmp.v1.v);

		/*memcpy(&obj->vtrans[obj->vtrans_curr], &lTmp, sizeof(PLINE4D));
		obj->vtrans_curr++;*/
		//near Z clip
		num_verts_in = 0;

		if(lTmp.v0.z > cam->far_clip_z)
			vertex_ccodes[0] = CLIP_CODE_GZ;
		else if(lTmp.v0.z < cam->near_clip_z)
			vertex_ccodes[0] = CLIP_CODE_LZ;
		else
		{
			vertex_ccodes[0] = CLIP_CODE_IZ;
			num_verts_in++;
		}

		if(lTmp.v1.z > cam->far_clip_z)
			vertex_ccodes[1] = CLIP_CODE_GZ;
		else if(lTmp.v1.z < cam->near_clip_z)
			vertex_ccodes[1] = CLIP_CODE_LZ;
		else
		{
			vertex_ccodes[1] = CLIP_CODE_IZ;
			num_verts_in++;
		}

		if(((vertex_ccodes[0] == CLIP_CODE_GZ) && (vertex_ccodes[1] == CLIP_CODE_GZ)) || ((vertex_ccodes[0] == CLIP_CODE_LZ) && (vertex_ccodes[1] == CLIP_CODE_LZ)))
		{
			continue;
		}

		if((vertex_ccodes[0] | vertex_ccodes[1]) & CLIP_CODE_LZ)
		{
			if(num_verts_in == 1)
			{
				if(vertex_ccodes[0] == CLIP_CODE_IZ)
				{
					vecBuild(&lTmp.v0.v, &lTmp.v1.v, &vTmp); 

					float t = (cam->near_clip_z - lTmp.v0.z) / vTmp.z;

					lTmp.v1.x = lTmp.v0.x + vTmp.x * t;
					lTmp.v1.y = lTmp.v0.y + vTmp.y * t;
					lTmp.v1.z = cam->near_clip_z;
				}
				else if(vertex_ccodes[1] == CLIP_CODE_IZ)
				{
					vecBuild(&lTmp.v1.v, &lTmp.v0.v, &vTmp); 

					float t = (cam->near_clip_z - lTmp.v1.z) / vTmp.z;

					lTmp.v0.x = lTmp.v1.x + vTmp.x * t;
					lTmp.v0.y = lTmp.v1.y + vTmp.y * t;
					lTmp.v0.z = cam->near_clip_z;
				}


				//memcpy(&obj->vtrans[obj->vtrans_curr], &lTmp, sizeof(PLINE4D));
				vecZero(&obj->vtrans[obj->vtrans_curr].v0.v);
				vecZero(&obj->vtrans[obj->vtrans_curr].v1.v);
				vecCopy(&lTmp.v0.v, &obj->vtrans[obj->vtrans_curr].v0.v);
				vecCopy(&lTmp.v1.v, &obj->vtrans[obj->vtrans_curr].v1.v);
				obj->vtrans_curr++;
			}
			//else if(num_verts_in == 2)
			//{
			//	//memcpy(&obj->vtrans[obj->vtrans_curr], &lTmp, sizeof(PLINE4D));
			//	vecZero(&obj->vtrans[obj->vtrans_curr].v0.v);
			//	vecZero(&obj->vtrans[obj->vtrans_curr].v1.v);
			//	vecCopy(&lTmp.v0.v, &obj->vtrans[obj->vtrans_curr].v0.v);
			//	vecCopy(&lTmp.v1.v, &obj->vtrans[obj->vtrans_curr].v1.v);
			//	obj->vtrans_curr++;
			//}
		}
		else
			{
				//memcpy(&obj->vtrans[obj->vtrans_curr], &lTmp, sizeof(PLINE4D));
				vecZero(&obj->vtrans[obj->vtrans_curr].v0.v);
				vecZero(&obj->vtrans[obj->vtrans_curr].v1.v);
				vecCopy(&lTmp.v0.v, &obj->vtrans[obj->vtrans_curr].v0.v);
				vecCopy(&lTmp.v1.v, &obj->vtrans[obj->vtrans_curr].v1.v);
				obj->vtrans_curr++;
			}
	}
}
void Pipeline::CameraToScreenOBJECT4DLINE(OBJECT4D_LINE_PTR obj, CAM4D_PTR cam)
{
	for(int i = 0; i < obj->vtrans_curr; i++)
	{
		obj->vtrans[i].v0.x = cam->viewport_center_x + cam->view_dist2 * obj->vtrans[i].v0.x / obj->vtrans[i].v0.z;
		obj->vtrans[i].v0.y = cam->viewport_center_y - cam->view_dist2 * obj->vtrans[i].v0.y / obj->vtrans[i].v0.z;
		obj->vtrans[i].v0.z = 1.0f / obj->vtrans[i].v0.z;

		obj->vtrans[i].v1.x = cam->viewport_center_x + cam->view_dist2 * obj->vtrans[i].v1.x / obj->vtrans[i].v1.z;
		obj->vtrans[i].v1.y = cam->viewport_center_y - cam->view_dist2 * obj->vtrans[i].v1.y / obj->vtrans[i].v1.z;
		obj->vtrans[i].v1.z = 1.0f / obj->vtrans[i].v1.z;

		/*TCHAR mas[128];
		swprintf(mas, L"%f %f %f", obj->vtrans[i].v0.x, obj->vtrans[i].v0.y, obj->vtrans[i].v0.z);
		MessageBox(0,mas,0,0);*/
	}
}

void Pipeline::ClipPolygonScreenBounds(RENDERLIST4D_PTR rendList, CAM4D_PTR cam) {
	std::vector<POLYF4D> curPolys;

	curPolys.reserve(rendList->num_polys);

	for (int poly = 0; poly < rendList->num_polys; poly++)
	{
		POLYF4D_PTR currPoly = rendList->poly_ptrs[poly];

		if ((currPoly == NULL) || !(currPoly->state & struct3D::POLY4D_STATE_ACTIVE) || (currPoly->state & struct3D::POLY4D_STATE_BACKFACE) || (currPoly->state & struct3D::POLY4D_STATE_CLIPPED)) continue;

		curPolys.push_back(*currPoly);
	}

	rendList->Reset_RENDERLIST4D();

	int polyIdx = 0;

	struct Lines {
		Line2D ab, bc, ca;
	};

	const bool useShortLenForTest = true;
	const bool useShortLenForRealClip = true;

	if(false)
	{
		std::vector<Lines> polyLines;
		std::vector<Lines> polyLinesClipped;

		for (auto& poly : curPolys) {
			Point2D a, b, c;

			a.x = poly.tvlist[0].x;
			a.y = poly.tvlist[0].y;
			b.x = poly.tvlist[1].x;
			b.y = poly.tvlist[1].y;
			c.x = poly.tvlist[2].x;
			c.y = poly.tvlist[2].y;

			Lines lines;

			lines.ab.start = a;
			lines.ab.end = b;

			lines.bc.start = b;
			lines.bc.end = c;

			lines.ca.start = c;
			lines.ca.end = a;

			polyLines.push_back(lines);

			Lines linesClippes = lines;

			Point2D clipMin = { 0.f, 0.f };
			Point2D clipMax = { cam->viewport_width, cam->viewport_height };

			if (auto clipped = CohenSutherlandLineClip(linesClippes.ab, clipMin, clipMax, useShortLenForTest)) {
				linesClippes.ab = *clipped;
			}

			if (auto clipped = CohenSutherlandLineClip(linesClippes.bc, clipMin, clipMax, useShortLenForTest)) {
				linesClippes.bc = *clipped;
			}

			if (auto clipped = CohenSutherlandLineClip(linesClippes.ca, clipMin, clipMax, useShortLenForTest)) {
				linesClippes.ca = *clipped;
			}

			polyLinesClipped.push_back(linesClippes);
		}

		{
			/*Point2D clipMin = { 0.f, 0.f };
			Point2D clipMax = { cam->viewport_width, cam->viewport_height };

			auto orig_0bc = polyLines[0].bc;
			auto orig_2ab = polyLines[2].ab;
			auto clipped_0bc = orig_0bc;
			auto clipped_2ab = orig_2ab;

			auto vec_0bc = (orig_0bc.start - orig_0bc.end).Normalized();
			auto vec_2ab = (orig_2ab.end - orig_2ab.start).Normalized();

			if (auto clipped = CohenSutherlandLineClip(orig_0bc, clipMin, clipMax, true)) {
				clipped_0bc = *clipped;
			}
			if (auto clipped = CohenSutherlandLineClip(orig_2ab, clipMin, clipMax, true)) {
				clipped_2ab = *clipped;
			}*/

			bool test1 = polyLines[0].bc.start.y <= polyLines[2].ab.end.y;
			bool test2 = polyLinesClipped[0].bc.start.y <= polyLinesClipped[2].ab.end.y;
			int stop = 234;
		}
	}

	for (auto& poly : curPolys) {
		Point2D a, b, c;

		a.x = poly.tvlist[0].x;
		a.y = poly.tvlist[0].y;
		b.x = poly.tvlist[1].x;
		b.y = poly.tvlist[1].y;
		c.x = poly.tvlist[2].x;
		c.y = poly.tvlist[2].y;

		Line2D ab, bc, ca;

		ab.start = a;
		ab.end = b;

		bc.start = b;
		bc.end = c;

		ca.start = c;
		ca.end = a;

		/*if (polyIdx != 1) {
			++polyIdx;
			continue;
			int stop = 24;
		}*/

		bool yTest1 = ab.start.y >= bc.end.y;

		if (polyIdx == 1) {
			int stop = 24;
		}

		ab = SortSwap(ab);
		bc = SortSwap(bc);
		ca = SortSwap(ca);

		if (polyIdx == 1) {
			int stop = 24;
		}

		Point2D clipMin = { 0.f, 0.f };
		Point2D clipMax = { cam->viewport_width, cam->viewport_height };

		if (auto clipped = CohenSutherlandLineClip(ab, clipMin, clipMax, useShortLenForRealClip)) {
			ab = *clipped;
			ab.clipped = ab.startEdge != LinePointEdge::NoEdge || ab.endEdge != LinePointEdge::NoEdge;
		}
		else {
			ab.declined = true;
		}

		if (auto clipped = CohenSutherlandLineClip(bc, clipMin, clipMax, useShortLenForRealClip)) {
			bc = *clipped;
			bc.clipped = bc.startEdge != LinePointEdge::NoEdge || bc.endEdge != LinePointEdge::NoEdge;
		}
		else {
			bc.declined = true;
		}

		if (auto clipped = CohenSutherlandLineClip(ca, clipMin, clipMax, useShortLenForRealClip)) {
			ca = *clipped;
			ca.clipped = ca.startEdge != LinePointEdge::NoEdge || ca.endEdge != LinePointEdge::NoEdge;
		}
		else {
			ca.declined = true;
		}

		bool yTest2 = ab.end.y >= bc.end.y;

		bool skip = false;
		std::vector<Point2D> screenPoly;

		if (!ab.declined && !ab.clipped && !bc.declined && !bc.clipped && !ca.declined && !ca.clipped) {
			// fully inside screen
			rendList->Insert_POLYF4D_RENDERLIST4D(&poly);
		}
		else if (ab.declined && bc.declined && ca.declined) {
			// triangle not inside screen
			if (ab.outcodeStart & ab.outcodeEnd & bc.outcodeStart & bc.outcodeEnd & ca.outcodeStart & ca.outcodeEnd) {
				// share same zone out of screen
				// triangle fully not visible
				skip = true;
			}
			else {
				auto abPlane = HalfPlane::BuildFromPoints(ab.start, ab.end, ab.swapped);
				auto bcPlane = HalfPlane::BuildFromPoints(bc.start, bc.end, bc.swapped);
				auto caPlane = HalfPlane::BuildFromPoints(ca.start, ca.end, ca.swapped);

				// screen CW order
				Point2D clipPt[] = { clipMin, { clipMax.x, clipMin.y }, clipMax, { clipMin.x, clipMax.y } };

				size_t insideCount = 0;

				for (auto& pt : clipPt) {
					if (abPlane.IsInside(pt) && bcPlane.IsInside(pt) && caPlane.IsInside(pt)) {
						++insideCount;
					}
				}

				// screen vertices(clipPt) must be or fully inside or fully outside
				assert(insideCount == std::size(clipPt) || insideCount == 0);

				if (insideCount == std::size(clipPt)) {
					screenPoly.insert(std::end(screenPoly), std::begin(clipPt), std::end(clipPt));
				}
			}
		}
		else {
			assert(ab.clipped || bc.clipped || ca.clipped);
			// some edge clipped
			// some may be declined, not use declined

			ab = Unswap(ab);
			bc = Unswap(bc);
			ca = Unswap(ca);

			std::vector<Point2D> inside;
			std::vector<Point2DOnEdge> edgePt;

			auto addAnyIfInside = [](const Line2D& line, std::vector<Point2D>& vec)
			{
				if (!line.declined) {
					if (line.outcodeStart == INSIDE) {
						vec.push_back(line.start);
						return true;
					}
					else if (line.outcodeEnd == INSIDE) {
						vec.push_back(line.end);
						return true;
					}
				}

				return false;
			};
			auto addIfEdgeFullyInside = [](const Line2D& line, std::vector<Point2D>& vec)
			{
				if (!line.declined && line.outcodeStart == INSIDE && line.outcodeEnd == INSIDE) {
					vec.push_back(line.start);
					vec.push_back(line.end);
					return true;
				}

				return false;
			};
			auto addIfEdge = [&clipMin, &clipMax](const Line2D& line1, const Line2D& line2, std::vector<Point2DOnEdge>& vec)
			{
				if (auto pt = line1.GetEndPointOnEdge()) {
					auto clamped = pt->ClampedByEdge(clipMin, clipMax);
					clamped.fromTriangle = true;
					vec.push_back(clamped);
				}
				if (auto pt = line2.GetStartPointOnEdge()) {
					auto clamped = pt->ClampedByEdge(clipMin, clipMax);
					clamped.fromTriangle = true;
					vec.push_back(clamped);
				}
			};

			{
				// some edges are clipped
				// inside can be 0, 1 or 2 vertices
				// 2 vertices can be when single edge fully inside
				// else if 1 vertex inside then inside vertex from any edge will do
				// else if there is no inside vertices

				bool twoAdded = addIfEdgeFullyInside(ab, inside) || addIfEdgeFullyInside(bc, inside) || addIfEdgeFullyInside(ca, inside);
				if (!twoAdded) {
					(addAnyIfInside(ab, inside) || addAnyIfInside(bc, inside) || addAnyIfInside(ca, inside));
				}
			}

			addIfEdge(ab, bc, edgePt);
			addIfEdge(bc, ca, edgePt);
			addIfEdge(ca, ab, edgePt);

			// screen CW order
			{
				Point2DOnEdge clipEdgePt;

				clipEdgePt.pt = clipMin;
				clipEdgePt.edge = LinePointEdge::ClipMinY;
				clipEdgePt.fromTriangle = false;

				edgePt.push_back(clipEdgePt);
			}
			{
				Point2DOnEdge clipEdgePt;

				clipEdgePt.pt.x = clipMax.x;
				clipEdgePt.pt.y = clipMin.y;
				clipEdgePt.edge = LinePointEdge::ClipMinY;
				clipEdgePt.fromTriangle = false;

				edgePt.push_back(clipEdgePt);
			}
			{
				Point2DOnEdge clipEdgePt;

				clipEdgePt.pt = clipMax;
				clipEdgePt.edge = LinePointEdge::ClipMaxX;
				clipEdgePt.fromTriangle = false;

				edgePt.push_back(clipEdgePt);
			}
			{
				Point2DOnEdge clipEdgePt;

				clipEdgePt.pt.x = clipMin.x;
				clipEdgePt.pt.y = clipMax.y;
				clipEdgePt.edge = LinePointEdge::ClipMaxX;
				clipEdgePt.fromTriangle = false;

				edgePt.push_back(clipEdgePt);
			}

			auto startEdge = edgePt[0].edge;

			std::sort(std::begin(edgePt), std::end(edgePt), [&clipMax, startEdge](const Point2DOnEdge& a, const Point2DOnEdge& b)
				{
					return a.GetEdgeOrder(clipMax, startEdge) < b.GetEdgeOrder(clipMax, startEdge);
				});

			std::vector<float> testOrders;

			for (auto& i : edgePt) {
				testOrders.push_back(i.GetEdgeOrder(clipMax, startEdge));
			}

			{
				auto findFromTriangle = [](const Point2DOnEdge& i) {
					return i.fromTriangle;
				};

				size_t startIdx = 0; // to track processed segments
				auto beg = std::begin(edgePt);

				while (beg != std::end(edgePt)) {
					// if not inside segment - find begin of next segment and delete vertices not in segment
					// if in segment - do nothing
					auto end = std::find_if(beg, std::end(edgePt), findFromTriangle);
					edgePt.erase(beg, end);

					if (startIdx == edgePt.size()) {
						break;
					}

					if (!edgePt.empty()) {
						// go to segment end
						end = std::find_if(std::begin(edgePt) + startIdx + 1, std::end(edgePt), findFromTriangle); // end of cur segment
						beg = end;
						if (beg != std::end(edgePt)) {
							// go past segment end
							beg = beg + 1;
							startIdx = beg - std::begin(edgePt);
						}
					}
				}
			}

			assert(inside.size() == 1 || inside.size() == 2);
			assert(edgePt.size() >= 1);

			screenPoly.reserve(inside.size() + edgePt.size());

			screenPoly.insert(std::end(screenPoly), std::begin(inside), std::end(inside));
			std::transform(std::begin(edgePt), std::end(edgePt), std::back_inserter(screenPoly), [](const Point2DOnEdge& edgePt)
				{
					return edgePt.pt;
				});

			/*if (polyIdx == 1) {
				std::swap(screenPoly[1], screenPoly[2]);
			}*/

			polyIdx;

			assert(screenPoly.size() >= 3); // at least 3 vertices for triangle
		}

		if (!screenPoly.empty()) {
			// screenPoly must be CW in screen(pixel) space
			// screenPoly must be convex
			// TODO add asserts to check for CW order
			// TODO add asserts to check for convex screenPoly

			// CW(screen space) + convex gives easy way to triangulate it
			// just take vertices in order(already CW order) and connect first, previous and current(starting from 2-idx)
			// it will do fan triangulation, easy implementation
			// TODO look for other triangulations of convex polygon - https://en.wikipedia.org/wiki/Minimum-weight_triangulation
			for (size_t i = 2; i < screenPoly.size(); ++i) {
				auto pt0 = screenPoly[0];
				auto pt1 = screenPoly[i - 1];
				auto pt2 = screenPoly[i];

				auto newPoly = poly;

				newPoly.tvlist[0].x = pt0.x;
				newPoly.tvlist[0].y = pt0.y;
				newPoly.tvlist[1].x = pt1.x;
				newPoly.tvlist[1].y = pt1.y;
				newPoly.tvlist[2].x = pt2.x;
				newPoly.tvlist[2].y = pt2.y;

				// fully inside screen
				rendList->Insert_POLYF4D_RENDERLIST4D(&newPoly);
			}
		}

		// if all declined maybe screen inside triangle

		//rendList->Insert_POLYF4D_RENDERLIST4D(&poly);

		++polyIdx;
	}
}

Pipeline::Line2D Pipeline::SortSwap(Line2D line) {
	if (std::abs(line.start.x) > std::abs(line.end.x)) {
		line.swapped = true;
	}
	else if (std::abs(line.start.x) == std::abs(line.end.x) && std::abs(line.start.y) > std::abs(line.end.y)) {
		line.swapped = true;
	}

	if (line.swapped) {
		std::swap(line.start.x, line.end.x);
		std::swap(line.start.y, line.end.y);
		std::swap(line.outcodeStart, line.outcodeEnd);
		std::swap(line.startEdge, line.endEdge);
	}

	return line;
}

Pipeline::Line2D Pipeline::Unswap(Line2D line) {
	if (line.swapped) {
		line.swapped = false;
		std::swap(line.start.x, line.end.x);
		std::swap(line.start.y, line.end.y);
		std::swap(line.outcodeStart, line.outcodeEnd);
		std::swap(line.startEdge, line.endEdge);
	}

	return line;
}

Pipeline::OutCode Pipeline::ComputeOutCode(const Point2D& pt, const Point2D& clipMin, const Point2D& clipMax) {
	OutCode code = INSIDE;  // initialised as being inside of clip window

	if (pt.x < clipMin.x)           // to the left of clip window
		code |= LEFT;
	else if (pt.x > clipMax.x)      // to the right of clip window
		code |= RIGHT;
	if (pt.y < clipMin.y)           // below the clip window
		code |= BOTTOM;
	else if (pt.y > clipMax.y)      // above the clip window
		code |= TOP;

	return code;
}

std::optional<Pipeline::Line2D> Pipeline::CohenSutherlandLineClip(Line2D line, const Point2D& clipMin, const Point2D& clipMax, bool tryShortLen /*= false*/) {
	/*float x0_orig = x0;
		float y0_orig = y0;
		float x1_orig = x1;
		float y1_orig = y1;*/

	// TODO need more tests with out of screen triangles
	// experimental
	if (tryShortLen) {
		//constexpr float shortLenValue = 1000000.f;
		constexpr float shortLenValue = 10000000000.f; // for 100000000000000000000000000000000000000.f
		float xlen = std::abs(line.start.x - line.end.x);
		float ylen = std::abs(line.start.y - line.end.y);

		if (xlen < ylen) {
			if (xlen > shortLenValue) {
				float toShortLenK = shortLenValue / xlen;

				if (std::abs(line.start.x) < std::abs(line.end.x)) {
					auto v = line.end - line.start;

					line.end.x = line.start.x + v.x * toShortLenK;
					line.end.y = line.start.y + v.y * toShortLenK;
				}
				else {
					auto v = line.start - line.end;

					line.start.x = line.end.x + v.x * toShortLenK;
					line.start.y = line.end.y + v.y * toShortLenK;
				}

				int stop = 234;
			}
		}
		else {
			if (ylen > shortLenValue) {
				float toShortLenK = shortLenValue / ylen;

				if (std::abs(line.start.y) < std::abs(line.end.y)) {
					auto v = line.end - line.start;

					line.end.x = line.start.x + v.x * toShortLenK;
					line.end.y = line.start.y + v.y * toShortLenK;
				}
				else {
					auto v = line.start - line.end;

					line.start.x = line.end.x + v.x * toShortLenK;
					line.start.y = line.end.y + v.y * toShortLenK;
				}

				int stop = 234;
			}
		}
	}

	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	OutCode outcode0 = ComputeOutCode(line.start, clipMin, clipMax);
	OutCode outcode1 = ComputeOutCode(line.end, clipMin, clipMax);
	bool accept = false;

	line.outcodeStart = outcode0;
	line.outcodeEnd = outcode1;

	while (true) {
		if (!(outcode0 | outcode1)) {
			// bitwise OR is 0: both points inside window; trivially accept and exit loop
			accept = true;
			break;
		}
		else if (outcode0 & outcode1) {
			// bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
			// or BOTTOM), so both must be outside window; exit loop (accept is false)
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			float x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = outcode1 > outcode0 ? outcode1 : outcode0;
			LinePointEdge edge = LinePointEdge::NoEdge;

			auto len = line.end - line.start;
			len.x = std::fabs(len.x);
			len.y = std::fabs(len.y);

			// Now find the intersection point;
			// use formulas:
			//   slope = (y1 - y0) / (x1 - x0)
			//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
			//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
			// No need to worry about divide-by-zero because, in each case, the
			// outcode bit being tested guarantees the denominator is non-zero
			if (outcodeOut & TOP) {           // point is above the clip window
				if (std::fabs(line.start.x) < std::fabs(line.end.x)) {
					x = line.start.x + len.x * (clipMax.y - line.start.y) / len.y;
				}
				else {
					x = line.end.x + len.x * (clipMax.y - line.end.y) / len.y;
				}

				y = clipMax.y;
				edge = LinePointEdge::ClipMaxY;
			}
			else if (outcodeOut & BOTTOM) { // point is below the clip window
				if (std::fabs(line.start.x) < std::fabs(line.end.x)) {
					x = line.start.x + len.x * (clipMin.y - line.start.y) / len.y;
				}
				else {
					x = line.end.x + len.x * (clipMin.y - line.end.y) / len.y;
				}

				y = clipMin.y;
				edge = LinePointEdge::ClipMinY;
			}
			else if (outcodeOut & RIGHT) {  // point is to the right of clip window
				if (std::fabs(line.start.y) < std::fabs(line.end.y)) {
					y = line.start.y + len.y * (clipMax.x - line.start.x) / len.x;
				}
				else {
					y = line.end.y + len.y * (clipMax.x - line.end.x) / len.x;
				}

				x = clipMax.x;
				edge = LinePointEdge::ClipMaxX;
			}
			else if (outcodeOut & LEFT) {   // point is to the left of clip window
				if (std::fabs(line.start.y) < std::fabs(line.end.y)) {
					y = line.start.y + len.y * (clipMin.x - line.start.x) / len.x;
				}
				else {
					y = line.end.y + len.y * (clipMin.x - line.end.x) / len.x;
				}

				x = clipMin.x;
				edge = LinePointEdge::ClipMinX;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				line.start.x = x;
				line.start.y = y;
				line.startEdge = edge;
				outcode0 = ComputeOutCode(line.start, clipMin, clipMax);
			}
			else {
				line.end.x = x;
				line.end.y = y;
				line.endEdge = edge;
				outcode1 = ComputeOutCode(line.end, clipMin, clipMax);
			}
		}
	}

	if (accept) {
		/*line.outcodeStart = outcode0;
		line.outcodeEnd = outcode1;*/
		return line;
	}

	return {};
}

Pipeline::Point2DOnEdge Pipeline::Point2DOnEdge::ClampedByEdge(const Point2D& clipMin, const Point2D& clipMax) const {
	Point2DOnEdge res;

	res = *this;

	switch (this->edge) {
	case Pipeline::LinePointEdge::ClipMaxX:
	case Pipeline::LinePointEdge::ClipMinX:
		res.pt.y = std::clamp(res.pt.y, clipMin.y, clipMax.y);
		break;
	case Pipeline::LinePointEdge::ClipMaxY:
	case Pipeline::LinePointEdge::ClipMinY:
		res.pt.x = std::clamp(res.pt.x, clipMin.x, clipMax.x);
		break;
	}

	return res;
}

float Pipeline::Point2DOnEdge::GetEdgeOrder(const Point2D& clipMax, LinePointEdge startEdge) const {
	float order = std::numeric_limits<float>::infinity();
	const float edgeStep = (std::max)(clipMax.x, clipMax.y);
	constexpr LinePointEdge edges[4] = { LinePointEdge::ClipMinY, LinePointEdge::ClipMaxX, LinePointEdge::ClipMaxY, LinePointEdge::ClipMinX };
	size_t edgeIdx = 0;

	auto startIt = std::find(std::begin(edges), std::end(edges), startEdge);
	auto startIdIdx = startIt - std::begin(edges);

	for (size_t i = 0; i < std::size(edges); ++i, ++edgeIdx) {
		if (edges[(startIdIdx + i) % std::size(edges)] == this->edge) {
			break;
		}
	}

	float actualEdgeStep = edgeStep * edgeIdx;

	switch (this->edge) {
	case LinePointEdge::ClipMinY:
		// bottom edge
		order = actualEdgeStep + this->pt.x;
		break;
	case LinePointEdge::ClipMaxX:
		// right edge
		order = actualEdgeStep + this->pt.y;
		break;
	case LinePointEdge::ClipMaxY:
		// top edge
		order = actualEdgeStep + (clipMax.x - this->pt.x);
		break;
	case LinePointEdge::ClipMinX:
		// left edge
		order = actualEdgeStep + (clipMax.y - this->pt.y);
		break;
	}

	return order;
}

std::optional<Pipeline::Point2DOnEdge> Pipeline::Line2D::GetStartPointOnEdge() const {
	if (this->startEdge == LinePointEdge::NoEdge) {
		return {};
	}

	Point2DOnEdge ptOnEdge;

	ptOnEdge.pt = this->start;
	ptOnEdge.edge = this->startEdge;

	return ptOnEdge;
}

std::optional<Pipeline::Point2DOnEdge> Pipeline::Line2D::GetEndPointOnEdge() const {
	if (this->endEdge == LinePointEdge::NoEdge) {
		return {};
	}

	Point2DOnEdge ptOnEdge;

	ptOnEdge.pt = this->end;
	ptOnEdge.edge = this->endEdge;

	return ptOnEdge;
}
