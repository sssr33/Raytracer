#pragma once
#include "Math3DStructs.h"

#include <vector>
#include <string>

using namespace math3D;

namespace mat{
	const int MAT_ATTR_2SIDED = 0x0001;
	const int MAT_ATTR_TRANSPARENT = 0x0002;
	const int MAT_ATTR_8BITCOLOR = 0x0004;
	const int MAT_ATTR_RGB16 = 0x0008;
	const int MAT_ATTR_RGB24 = 0x00010;

	const int MAT_ATTR_SHADE_MODE_CONSTANT = 0x0020;
	const int MAT_ATTR_SHADE_MODE_EMMISIVE = 0x0020; //EMMISIVE == CONSTANT
	const int MAT_ATTR_SHADE_MODE_FLAT = 0x0040;
	const int MAT_ATTR_SHADE_MODE_GOURAUD = 0x0080;
	const int MAT_ATTR_SHADE_MODE_FASTPHONG = 0x0100;
	const int MAT_ATTR_SHADE_MODE_TEXTURE = 0x0200;
	const int MAT_ATTR_SHADE_MODE_1PLINE = 0x0400;

	const int MAT_STATE_ACTIVE = 0x0001;
	//const int MAX_MATERIALS = 256; // перенесено в класс MaterialSystem

	const int LIGHT_ATTR_AMBIENT = 0x0001;
	const int LIGHT_ATTR_INFINITE = 0x0002;
	const int LIGHT_ATTR_POINT = 0x0004;
	const int LIGHT_ATTR_SPOTLIGHT1 = 0x0008;
	const int LIGHT_ATTR_SPOTLIGHT2= 0x0010;

	const int LIGHT_STATE_ON = 1;
	const int LIGHT_STATE_OFF = 0;

	//const int MAX_LIGHTS = 8; //перенесено в класс LightSystem

	typedef struct RGBA_TYP
	{
		union
		{
			int rgba;
			UCHAR rgba_M[4];
			struct { UCHAR a, b, g, r; };
		};
	} RGBA, *RGBA_PTR;

	typedef struct BITMAP_IMAGE_TYP
    {
        int state;          // state of bitmap
        int attr;           // attributes of bitmap
        int x,y;            // position of bitmap
        int width, height;  // size of bitmap
        int num_bytes;      // total bytes of bitmap
        int bpp;            // bits per pixel
        UCHAR *buffer;      // pixels of bitmap

    } BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

	typedef struct TEXTURE2D_TYP
    {
        int state;          // state of bitmap
        int attr;           // attributes of bitmap
        int x,y;            // position of bitmap
        int width, height;  // size of bitmap
        int num_bytes;      // total bytes of bitmap
		int lpitch;
        int bpp;            // bits per pixel
        UCHAR *buffer;      // pixels of bitmap
		void Release()
		{
			delete buffer;
			buffer = 0;
		}
    } TEXTURE2D, *TEXTURE2D_PTR;

	typedef struct LIGHT_TYP
	{
		int state;
		int id;
		int attr;

		RGBA c_ambient;
		RGBA c_diffuse;
		RGBA c_specular;

		math3D::POINT4D pos;
		math3D::POINT4D tpos;
		//math3D::POINT4D posCam;//позиция источника света в координатах камеры
		math3D::VECTOR4D dir;
		math3D::VECTOR4D tdir;

		float kc, kl, kq;
		float spot_inner;
		float spot_outer;
		float pf;
	}LIGHT, *LIGHT_PTR;

	typedef struct MAT_TYP
	{
		int state;
		int id;

		char name[64];
		int attr;

		RGBA color;
		float ka, kd, ks, power;

		RGBA ra, rd, rs;

		char texture_file[80];
		BITMAP_IMAGE texture;
	}MAT, *MAT_PTR;

	class MaterialSystem
	{
	private:
		int num_materials;
		static const int MAX_MATERIALS = 256;

		int DestroyBitmap(BITMAP_IMAGE_PTR image);
	public:
		MAT materials[MAX_MATERIALS];

		int ResetMaterialsMAT();
	};

	class LightSystem
	{
	private:
		int num_lights;
		static const int MAX_LIGHTS = 8;
	public:
		LIGHT lights[MAX_LIGHTS];

		LightSystem();
		int ResetLightsLIGHT();
		int InitLightLIGHT(int index, int state, int attr, RGBA c_ambient, RGBA c_diffuse, RGBA c_specular, math3D::POINT4D_PTR pos, math3D::VECTOR4D_PTR dir, float kc, float kl, float kq, float spot_inner, float spot_outer, float pf);
		int getNumLights();
		void transformLights(MATRIX4X4_PTR mt);
	};
};

enum coordSelect{
	TRANSFORM_LOCAL_ONLY, TRANSFORM_TRANS_ONLY, TRANSFORM_LOCAL_TO_TRANS
};

namespace struct3D
{
const int POLY4D_ATTR_2SIDED = 0x0001;
const int POLY4D_ATTR_TRANSPARENT = 0x0002;
const int POLY4D_ATTR_8BITCOLOR = 0x0004;
const int POLY4D_ATTR_RGB16 = 0x0008;
const int POLY4D_ATTR_RGB24 = 0x0010;

const int POLY4D_ATTR_SHADE_MODE_PURE = 0x0020;
const int POLY4D_ATTR_SHADE_MODE_FLAT = 0x0040;
const int POLY4D_ATTR_SHADE_MODE_GOURAUD = 0x0080;
const int POLY4D_ATTR_SHADE_MODE_PHONG = 0x0100;
const int POLY4D_ATTR_SHADE_MODE_TEST = 0x0200;

const int POLY4D_ATTR_SELECTION = 0x2000;
const int POLY4D_ATTR_SELECTED = 0x4000;

const int POLY4D_ATTR_ENABLE_MATERIAL = 0x0800; // use a real material for lighting
const int POLY4D_ATTR_DISABLE_MATERIAL = 0x1000; // use basic color only for lighting (emulate version 1.0)

const int POLY4D_STATE_ACTIVE = 0x0001;
const int POLY4D_STATE_CLIPPED = 0x0002;
const int POLY4D_STATE_BACKFACE = 0x0004;
const int POLY4D_STATE_LIT = 0x0008;

const int OBJECT4D_MAX_VERTICES = 40960;
const int OBJECT4D_MAX_POLYS = 81920;
const int OBJECT4D_STATE_NULL = 0x0000;
const int OBJECT4D_STATE_ACTIVE = 0x0001;
const int OBJECT4D_STATE_VISIBLE = 0x0002;
const int OBJECT4D_STATE_CULLED = 0x0004;
const int OBJECT4D_ATTR_SINGLE_FRAME = 0x0001;
const int OBJECT4D_ATTR_MULTI_FRAME = 0x0002;
const int OBJECT4D_ATTR_TEXTURES = 0x0004;
const int OBJECT4D_ATTR_SYS_OBJECT = 0x0008;

const int RENDERLIST4D_MAX_POLYS =  16384;//327680;//

const int CAM_MODEL_EULER = 0x0008;
const int CAM_MODEL_UVN = 0x0010;
const int UVN_MODE_SIMPLE = 0;
const int UVN_MODE_SPHERICAL = 1;
const int CAM_ROT_SEQ_XYZ = 0;
const int CAM_ROT_SEQ_YXZ = 1;
const int CAM_ROT_SEQ_XZY = 2;
const int CAM_ROT_SEQ_YZX = 3;
const int CAM_ROT_SEQ_ZYX = 4;
const int CAM_ROT_SEQ_ZXY = 5;

const int VERTEX4DT_ATTR_NULL = 0;
const int VERTEX4DT_ATTR_POINT = 0x0001;
const int VERTEX4DT_ATTR_NORMAL = 0x0002;
const int VERTEX4DT_ATTR_TEXTURE = 0x0004;
const int VERTEX4DT_ATTR_SELECTED = 0x0008;
const int VERTEX4DT_ATTR_GENERATED = 0x0010;


	class SubSelection
	{
	public:
		virtual void move(float dx, float dy, float dz) = 0;
		virtual void rotate(VECTOR4D_PTR vAngles, POINT4D_PTR pAround, MATRIX4X4_PTR mrot = 0) = 0;
		virtual void getPos(POINT4D_PTR p = 0) = 0;
		virtual void select() = 0;
		virtual void unselect() = 0;
		bool isSelected;

		SubSelection()
		{
			isSelected = false;
		}
	private:
	};
	typedef struct VERTEX4DT_TYP : public SubSelection
	{
		union
		{
			float M[12];
			struct
			{
				float x, y, z, w;
				float nx, ny, nz, nw;
				float u0, v0;
				float i;//итенсивность света после освещения
				int attr;
			};

			struct
			{
				POINT4D v;
				VECTOR4D n;
				POINT2D t;
			};
		};

		inline void VERTEX4DT_COPY(VERTEX4DT_TYP *dst)
		{
			v.VECTOR4D_COPY(&dst->v);
			n.VECTOR4D_COPY(&dst->n);
			memcpy(&dst->t, &this->t, sizeof(POINT2D));
			dst->attr = this->attr;
		}

		void move(float dx, float dy, float dz);
		void rotate(VECTOR4D_PTR vAngles, POINT4D_PTR pAround, MATRIX4X4_PTR mrot = 0);
		void getPos(POINT4D_PTR p = 0);
		void select();
		void unselect();
	} VERTEX4DT, *VERTEX4DT_PTR;

	typedef struct POLY4D_TYP : public SubSelection
	{
		int state = 0;
		int attr = 0;
		int color = 0;
		int lit_color[3] = {};

		mat::TEXTURE2D_PTR texture = nullptr;
		int mati = 0;

		VERTEX4DT_PTR vlist = nullptr;
		POINT2D_PTR tlist = nullptr;
		int vert[3] = {};
		int text[3] = {};
		float nlength = 0.f;
		unsigned int objID = 0;//ID объекта к которому принадлежит полигон

		void move(float dx, float dy, float dz);
		void rotate(VECTOR4D_PTR vAngles, POINT4D_PTR pAround, MATRIX4X4_PTR mrot = 0);
		void getPos(POINT4D_PTR p = 0);
		void select();
		void unselect();

		void SetLitColor(int color);
	} POLY4D, *POLY4D_PTR;

	typedef struct POLYF4D_TYP
	{
		int state;//4
		int attr;//4
		int color;//4
		int lit_color[3];//12

		mat::TEXTURE2D_PTR texture;//4
		int mati;//4

		float nlength;//4
		VECTOR4D normal;//16
		unsigned int objID;//ID объекта к которому принадлежит полигон//4

		float avg_z;//4

		VERTEX4DT vlist[3];//48*3 = 144
		VERTEX4DT tvlist[3];//144
		POINT4D worldPos[3];//48
		int vert[3];//12

		POLYF4D_TYP *next;//4
		POLYF4D_TYP *prev;//4

		//416
	} POLYF4D, *POLYF4D_PTR;

	typedef struct PLINE4D_TYP
	{
		VERTEX4DT v0;
		VERTEX4DT v1;
	} PLINE4D, *PLINE4D_PTR;

	typedef struct OBJECT4D_LINE_TYP
	{
		int color;
		POINT4D world_pos;

		PLINE4D *vlocal;
		int vlocal_size;
		int vlocal_curr;

		PLINE4D *vtrans;
		int vtrans_size;
		int vtrans_curr;

		int initialize(int num_lines);
		int release();

		int add(PLINE4D *line = 0);
	}OBJECT4D_LINE, *OBJECT4D_LINE_PTR;

	typedef struct BBox_TYP
	{
		POINT4D c;
		VECTOR4D r;
	} BBox, *BBox_PTR;

	enum class ObjectType
	{
		Unknown, Sphere, Box, Cylinder, Cone, EditableMesh
	};

	class Observer;
	class ObserverText;
	class ObserverColor;

	class SelectionSubject
	{
	public:
		void Attach(Observer *o);
		void DetachAll();
		void Notify();
		bool IsObserverHere(Observer *o);
	//protected:
	//	SelectionSubject()
	//	{}
	private:
		std::vector<Observer *> obs; 
	};

	class SubSelectionCollection : public SelectionSubject
	{
	public:
		void add(SubSelection *obj);
		void setMax(int max);
		int getMax();
		int getCurrent();
		void reset();
		//void getWorldPos(POINT4D_PTR p);
		void setPos(POINT4D_PTR p);
		void getPos(POINT4D_PTR p);

		void move(float dx, float dy, float dz);
		void rotate(VECTOR4D_PTR vAngles);
		void rotate(MATRIX4X4_PTR mrot);

		void getState(std::vector<float> *data_out);
	private:
		std::vector<SubSelection *> objects;
		int maxObjects;
		POINT4D pCenter, pWorld;
	};

	struct AdjacencyPoly
	{
		int adj[3];//adj[0] - vert 0, vert1 etc...
	};

	typedef struct OBJECT4D_TYP : public SelectionSubject
	{
		~OBJECT4D_TYP();

		ObjectType type = ObjectType::Unknown;
		float typeData[10] = {};
		BBox boundingBox = {};

		int id = 0;
		char name[64] = {};
		std::string strName;
		int color = 0;
		int state = 0;
		int attr = 0;
		int mati = 0;

		float *avg_radius = nullptr;
		float *max_radius = nullptr;

		POINT4D world_pos = {};

		VECTOR4D dir = {};
		VECTOR4D ux = {}, uy = {}, uz = {};

		int num_vertices = 0;
		int num_frames = 0;
		int total_vertices = 0;
		int curr_frame = 0;
		/*POINT4D vlist_local[OBJECT4D_MAX_VERTICES];
		POINT4D vlist_trans[OBJECT4D_MAX_VERTICES];*/

		
		VERTEX4DT_PTR vlist_local = nullptr;
		VERTEX4DT_PTR vlist_trans = nullptr;

		POINT2D_PTR scr_pts = nullptr;

		VERTEX4DT_PTR head_vlist_local = nullptr;
		VERTEX4DT_PTR head_vlist_trans = nullptr;
		

		POINT2D_PTR tlist = nullptr;
		//mat::BITMAP_IMAGE_PTR texture;
		mat::TEXTURE2D *texture = nullptr;

		int num_polys = 0;
		POLY4D_PTR plist = nullptr;
		AdjacencyPoly *adj = nullptr;

	//funstions:
		float Compute_OBJECT4D_Radius();
		void ComputeBBox();
		void ComputeAdjacency();
		void updateBBoxPosition();
		void ResetOBJECT4D();
		void TransformOBJECT4D(MATRIX4X4_PTR mt, coordSelect enm, bool transformBasis, int all_frames = 0);
		void ModelToWorld(int all_frames = 0);
		int SetFrame(int frame);

		int Destroy();
		int Init(int num_vertices, int num_polys, int num_frames, int destroy = 0);
		int ComputePolyNormals();
		int ComputeVertexNormals();

		void getState(float *data, int *size_out = 0);
		void setState(float *data, int size = 0);
		void setState(float data, int dataidx = -1);

		void setState(std::string data, int dataidx = -1);
		void setState(int *data, int dataidx = -1);
		void getState(std::string data, int *size_out = 0);
		void getState(int *data, int *size_out = 0);
	} OBJECT4D, *OBJECT4D_PTR;

	class Observer
	{
	public:
		virtual void Update() = 0;
		virtual void setSubject(SelectionSubject *subj) = 0;
		virtual void Release() = 0;
		virtual void unsetSubject() = 0;
		virtual void updateSubjectData() = 0;
		virtual void modifySubjectData(float modifier, int dataSlot) = 0;
	};

	enum DataSlotSphere
	{
		Segment, Radius
	};
	enum DataSlotCylinder
	{
		CylRadius, CylHeight, CylHSegs, CylCapSegs, CylSides
	};
	enum DataSlotCone
	{
		ConeRadius1, ConeRadius2, ConeHeight, ConeHSegs, ConeCapSegs, ConeSides
	};
	enum DataSlotBox
	{
		Length, Width, Height, LSegs, WSegs, HSegs
	};
	enum DataSlotEM
	{
		NumElementsInSelection
	};
	enum DataSlotCommon
	{
		Name = 10, Color
	};

	//class ObserverSizer : public Observer
	//{
	//public:
	//	void Update();
	//	void setSubject(SelectionSubject *subj);
	//	void Release();
	//	void unsetSubject();
	//	void updateSubjectData(){}
	//	void modifySubjectData(float modifier, int dataSlot){}

	//	void setWindow(wxWindow *w);
	//	void setCylinderSizer(wxSizer *s);
	//	void setConeSizer(wxSizer *s);
	//	void setSphereSizer(wxSizer *s);
	//	void setBoxSizer(wxSizer *s);
	//	void setEMSizer(wxSizer *s);
	//private:
	//	wxWindow *mainWnd;
	//	wxSizer *szCyl;
	//	wxSizer *szCone;
	//	wxSizer *szBox;
	//	wxSizer *szSph;
	//	wxSizer *szEM;
	//	OBJECT4D_TYP *subject;

	//	void hideAll();
	//};

	//class ObserverText : public Observer
	//{
	//public:
	//	void Update();
	//	void setSubject(SelectionSubject *subj);
	//	void Release();
	//	void unsetSubject();
	//	void updateSubjectData();
	//	void modifySubjectData(float modifier, int dataSlot);
	//	//void modifySubjectData(bool add, int dataSlot);

	//	void addTextCtrl(wxTextCtrl *t, int data_slot = -1);

	//	void setMinNumber(float min);
	//	void setMaxNumber(float max);
	//	//void setNumberModifier(float modifier);
	//private:
	//	std::vector<wxTextCtrl *> obj;
	//	std::vector<float> min;
	//	std::vector<float> max;
	//	//std::vector<float> modifier;
	//	std::vector<int> data_slots;
	//	OBJECT4D_TYP *subject;
	//};

	//class ObserverColor : public Observer
	//{
	//public:
	//	void Update();
	//	void setSubject(SelectionSubject *subj);
	//	void Release();
	//	void unsetSubject();
	//	void updateSubjectData();
	//	void modifySubjectData(float modifier, int dataSlot){}

	//	void setColorPicker(wxColourPickerCtrl *c);
	//private:
	//	wxColourPickerCtrl *obj;
	//	//int data_slot;
	//	OBJECT4D_TYP *subject;
	//};

	//class ObseverBmpBtReset : public Observer
	//{
	//public:
	//	void Update();
	//	void setSubject(SelectionSubject *subj);
	//	void Release(){}
	//	void unsetSubject();
	//	void updateSubjectData(){}
	//	void modifySubjectData(float modifier, int dataSlot){}


	//	void addBt(wxBitmapToggleButton *bt);
	//private:
	//	std::vector<wxBitmapToggleButton *> obj;
	//	OBJECT4D_TYP *subject;
	//};

	//class ObseverStatTxtCount : public Observer
	//{
	//public:
	//	void Update();
	//	void setSubject(SelectionSubject *subj);
	//	void Release(){}
	//	void unsetSubject();
	//	void updateSubjectData(){}
	//	void modifySubjectData(float modifier, int dataSlot){}


	//	void addSt(wxStaticText *st);
	//private:
	//	std::vector<wxStaticText *> obj;
	//	SubSelectionCollection *subject;
	//};

	typedef struct RENDERLIST4D_TYP
	{
		int state;
		int attr;

		POLYF4D_PTR poly_ptrs[RENDERLIST4D_MAX_POLYS];

		POLYF4D poly_data[RENDERLIST4D_MAX_POLYS];

		int num_polys;

		//OBJECT4D_PTR objRefs[512];//указатели на объекты
		int num_objects;

		void Reset_RENDERLIST4D();
		int Insert_POLYF4D_RENDERLIST4D(POLYF4D_PTR poly);
		int Insert_POLY4D_RENDERLIST4D(POLY4D_PTR poly, bool selection = 0);
		int Insert_OBJECT4D_RENDERLIST4D(OBJECT4D_PTR obj, bool selection = 0, int insert_local = 0);
		void TransformRENDERLIST4D(MATRIX4X4_PTR mt, coordSelect enm);

		int selectionVertex(RECT *r, VERTEX4DT_PTR *verts, int *num);
		//int getObject(POLYF4D_)
	} RENDERLIST4D, *RENDERLIST4D_PTR;

	typedef struct CAM4D_TYP
	{
		int state;
		int attr;

		POINT4D pos;

		VECTOR4D dir;

		VECTOR4D u;
		VECTOR4D v;
		VECTOR4D n;

		POINT4D target;

		float view_dist;
		float view_dist2;

		float fov;

		float near_clip_z;
		float far_clip_z;

		PLANE3D rt_clip_plane;
		PLANE3D lt_clip_plane;
		PLANE3D tp_clip_plane;
		PLANE3D bt_clip_plane;

		float viewplane_width;
		float viewplane_height;

		float viewport_width;
		float viewport_height;
		float viewport_center_x;
		float viewport_center_y;

		float aspect_ratio;

		MATRIX4X4 mcam;
		MATRIX4X4 mper;
		MATRIX4X4 mscr;

		VECTOR4D _right;
		VECTOR4D _up;
		VECTOR4D _look;

		void Init_CAM4D(int cam_attr, POINT4D_PTR cam_pos, VECTOR4D_PTR cam_dir, POINT4D_PTR cam_target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height);
		void Build_CAM4D_Matrix_Euler(int CamRotSeq = struct3D::CAM_ROT_SEQ_ZYX);
		void Build_CAM4D_Matrix_UVN(int mode);
		void Build_CAM4D_Matrix2();
		void get_CAM4D_InvMatrix2(MATRIX4X4_PTR m);

		void Build_Camera_To_Perspective_MATRIX4X4();
		void Build_Perspective_To_Screen_4D_MATRIX4X4();

		void getFwdVector(VECTOR4D_PTR vec);
		void getRightVector(VECTOR4D_PTR vec);
		void getUpVector(VECTOR4D_PTR vec);

		void strafe(float units); // влево/вправо
		void fly(float units);    // вверх/вниз
		void walk(float units);   // вперед/назад
		void pan(float units_up, float units_right);

		void pitch(float angle); // вращение относительно правого вектора
		void yaw(float angle);   // вращение относительно верхнего вектора
		void roll(float angle);  // вращение относительно вектора взгляда
		void arcRotate(float right, float up);

		POINT4D pos_pan_based;	//точка вокруг которой будет вращаться камера при использовании инструмента ArcRotation
	} CAM4D, *CAM4D_PTR;

	class frameSelect
	{
	public:
		frameSelect()
		{
			vecZero(&this->pLT);
			vecZero(&this->pRB);
		};
		frameSelect(frameSelect &b)
		{
			vecCopy(&this->pLT, &b.pLT);
			vecCopy(&this->pRB, &b.pRB);
		};

		void setFirstPoint(POINT2D_PTR p_in);
		void setNextPoint(POINT2D_PTR p_in);

		void getLeftTop(POINT2D_PTR p_out);
		void getLeftBottom(POINT2D_PTR p_out);
		void getRightTop(POINT2D_PTR p_out);
		void getRightBottom(POINT2D_PTR p_out);
	private:
		POINT2D pLT, pRB;
	};
};