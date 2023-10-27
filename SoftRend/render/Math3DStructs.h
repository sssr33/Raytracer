#pragma once
#include "math3DFunc.h"
#include "Sys.h"
#include <math.h>
#include <iostream>
#include <limits>

using namespace std;

namespace math3D
{
	/*class mathObjFunc;
	class mathObjFuncNOSIMD;*/

	const float PI = ((float)3.141592654f);
	const float PI2 = ((float)6.283185307f);
	const float PI_DIV_2 = ((float)1.570796327f);
	const float PI_DIV_4 = ((float)0.785398163f);
	const float PI_INV = ((float)0.318309886f);

	// defines for small numbers
	const float EPSILON_E3 = (float)(1E-3);
	const float EPSILON_E4 = (float)(1E-4);
	const float EPSILON_E5 = (float)(1E-5);
	const float EPSILON_E6 = (float)(1E-6);

	/*int Init(unsigned int *SSEInfo);
	int Release();

	static math3DFunc mathFunc;
	static mathObjFunc *mathObj = 0;*/

	typedef struct VECTOR4D_TYP VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;
	typedef struct VECTOR3D_TYP VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;
	typedef struct MATRIX4X4_TYP MATRIX4X4, *MATRIX4X4_PTR;

	/*const MATRIX4X4 IMAT_4X4 = {1,0,0,0, 
                            0,1,0,0, 
                            0,0,1,0, 
                            0,0,0,1};*/
	const float IMAT_4X4[4][4] = {1,0,0,0, 
                            0,1,0,0, 
                            0,0,1,0, 
                            0,0,0,1};

	typedef struct MATRIX4X4_TYP
	{
		union
		{
		float M[4][4]; // array indexed data storage

		// storage in row major form with explicit names
		struct
			 {
			 float M00, M01, M02, M03;
			 float M10, M11, M12, M13;
			 float M20, M21, M22, M23;
			 float M30, M31, M32, M33;
			 }; // end explicit names

		}; // end union

		void Build_Model_To_World_MATRIX4X4(VECTOR4D_PTR vPos);
		void Build_XYZ_Rotation_MATRIX4X4(float theta_x, float theta_y, float theta_z);// euler angles
		void Mat_Mul_4X4(MATRIX4X4_PTR mb, MATRIX4X4_PTR mprod);
		int Mat_Inverse_4X4(MATRIX4X4_PTR mi);
		inline void MAT_IDENTITY_4X4() 
		{
			memcpy((void *)M, (void *)&IMAT_4X4, sizeof(MATRIX4X4));
		}

		inline void Mat_Init_4X4( 
                        float m00, float m01, float m02, float m03,
                        float m10, float m11, float m12, float m13,
                        float m20, float m21, float m22, float m23,
                        float m30, float m31, float m32, float m33)
		{
			this->M00 = m00; this->M01 = m01; this->M02 = m02; this->M03 = m03;
			this->M10 = m10; this->M11 = m11; this->M12 = m12; this->M13 = m13;
			this->M20 = m20; this->M21 = m21; this->M22 = m22; this->M23 = m23;
			this->M30 = m30; this->M31 = m31; this->M32 = m32; this->M33 = m33;
		}
		inline void Mat_Copy(MATRIX4X4_PTR mDst)
		{
			memcpy(mDst, this, sizeof(MATRIX4X4_TYP));
		}

	} MATRIX4X4, *MATRIX4X4_PTR;

	typedef struct VECTOR4D_TYP
	{
		union
		{
			//__declspec(align(16)) float M[4]; // array indexed storage
			float M[4];

			// explicit names
			struct
			{
				float x,y,z,w;
			}; // end struct
		}; // end union

		void Mat_Mul_VECTOR4D_4X4(MATRIX4X4_PTR m, VECTOR4D_PTR vprod);
		void VECTOR4D_Add(VECTOR4D_PTR vb, VECTOR4D_PTR vsum);
		void VECTOR4D_Sub(VECTOR4D_PTR vb, VECTOR4D_PTR vdiff);
		void VECTOR4D_Build(VECTOR4D_PTR init, VECTOR4D_PTR term);
		float VECTOR4D_Dot(VECTOR4D_PTR vb);
		void VECTOR4D_Cross(VECTOR4D_PTR vb, VECTOR4D_PTR vn);
		void VECTOR4D_Normalize();
		float VECTOR4D_Length();
		
		inline float VECTOR4D_Length_Fast()
		{
			int temp;  // used for swaping
			int x,y,z; // used for algorithm

			// make sure values are all positive
			x = static_cast<int>(fabs(this->x) * 1024.f);
			y = static_cast<int>(fabs(this->y) * 1024.f);
			z = static_cast<int>(fabs(this->z) * 1024.f);

			// sort values
			if (y < x) 
			{
				temp = x;
				x = y;
				y = temp;
			}

			if (z < y) 
			{
				temp = z;
				z = y;
				y = temp;
			}

			if (y < x)
			{
				temp = x;
				x = y;
				y = temp;
			}

			int dist = (z + 11 * (y >> 5) + (x >> 2) );

			// compute distance with 8% error
			return((float)(dist >> 10));
		}
		inline void VECTOR4D_COPY(VECTOR4D_PTR vdst)
		{
			(vdst)->x = (this)->x; (vdst)->y = (this)->y;  
			(vdst)->z = (this)->z; (vdst)->w = (this)->w;
		}
		inline void VECTOR4D_INITXYZ(float x,float y,float z) 
		{
			this->x = x; this->y = y; this->z = z; this->w = 1.0f;
		}
		inline void VECTOR4D_ZERO() 
		{
			this->x = this->y = this->z = 0.0; this->w = 1.0;
		}
		inline void VECTOR4D_SWAP(VECTOR4D_PTR vb)
		{
			VECTOR4D vTemp;

			this->VECTOR4D_COPY(&vTemp);

			vb->VECTOR4D_COPY(this);

			vTemp.VECTOR4D_COPY(vb);
		}

	} VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;

	typedef struct VECTOR3D_TYP
	{
	union
		{
		float M[3]; // array indexed storage

		// explicit names
		struct
			 {
			 float x,y,z;
			 }; // end struct

		}; // end union

		inline void VECTOR3D_INITXYZ(float x, float y, float z) 
		{
			this->x = x; this->y = y; this->z = z;
		}
		inline void VECTOR3D_COPY(VECTOR3D_PTR vdst) 
		{
			vdst->x = this->x; vdst->y = this->y;  vdst->z = this->z; 
		}
		void VECTOR3D_Normalize();
		void VECTOR3D_Normalize(VECTOR3D_PTR vn);
		float VECTOR3D_Length();
		inline void VECTOR3D_ZERO() 
		{
			this->x = this->y = this->z = 0.0f;
		}

	} VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

	typedef struct VECTOR2D_TYP
	{
		union
		{
			float M[2];
			struct
			{
				float x, y;
			};
		};
	} VECTOR2D, POINT2D, *VECTOR2D_PTR, *POINT2D_PTR;

	struct Triangle2D {
		POINT2D a;
		POINT2D b;
		POINT2D c;
	};

	struct Screen2D {
		POINT2D min;
		POINT2D max;
	};

	typedef struct VECTOR2DI_TYP
	{
		union
		{
			int M[2];
			struct
			{
				int x, y;
			};
		};
	} VECTOR2DI, POINT2DI, *VECTOR2DI_PTR, *POINT2DI_PTR;

	typedef struct VECOTR3DI_TYP
	{
		union
		{
			int M[3];
			struct
			{
				int x, y, z;
			};
		};
	} VECTOR3DI, POINT3DI, *VECTOR3DI_PTR, *POINT3DI_PTR;

	typedef struct VECOTR4DI_TYP
	{
		union
		{
			int M[4];
			struct
			{
				int x, y, z, w;
			};
		};
	} VECTOR4DI, POINT4DI, *VECTOR4DI_PTR, *POINT4DI_PTR;

	typedef struct PLANE3D_TYP
	{
		POINT3D p0; // point on the plane
		VECTOR3D n; // normal to the plane (not necessarily a unit vector)

		void PLANE3D_Init(POINT3D_PTR p0, VECTOR3D_PTR normal, int normalize);
	} PLANE3D, *PLANE3D_PTR;

	typedef struct PLANE4D_TYP
	{
		POINT4D p0; // point on the plane
		VECTOR4D n; // normal to the plane (not necessarily a unit vector)
	} PLANE4D, *PLANE4D_PTR;

	POINT2D operator-(const POINT2D& a, const POINT2D& b);

	/*inline float DEG_TO_RAD(float ang) 
	{ 
		return ((ang)*PI/180.0);
	}
	inline float RAD_TO_DEG(float rads) 
	{
		return ((rads)*180.0/PI);
	}
	inline int RAND_RANGE(int x, int y)
	{
		return ( (x) + (rand()%((y)-(x)+1)));
	}*/


	/*class mathObjFunc
	{
	public:
		virtual int Init() = 0;
		virtual int Release() = 0;

		//VECTORS
		virtual void vecZero(VECTOR4D_PTR vec) = 0;
		virtual void vecZero(VECTOR3D_PTR vec) = 0;
		virtual void vecZero(VECTOR2D_PTR vec) = 0;

		virtual void vecCopy(VECTOR4D_PTR sour, VECTOR4D_PTR dest) = 0;
		virtual void vecCopy(VECTOR3D_PTR sour, VECTOR3D_PTR dest) = 0;
		virtual void vecCopy(VECTOR2D_PTR sour, VECTOR2D_PTR dest) = 0;

		virtual void vecSwap(VECTOR4D_PTR a, VECTOR4D_PTR b) = 0;
		virtual void vecSwap(VECTOR3D_PTR a, VECTOR3D_PTR b) = 0;
		virtual void vecSwap(VECTOR2D_PTR a, VECTOR2D_PTR b) = 0;

		virtual void vecBuild(POINT4D_PTR ptFrom, POINT4D_PTR ptTo, VECTOR4D_PTR vres) = 0;
		virtual void vecBuild(POINT3D_PTR ptFrom, POINT3D_PTR ptTo, VECTOR3D_PTR vres) = 0;
		virtual void vecBuild(POINT2D_PTR ptFrom, POINT2D_PTR ptTo, VECTOR2D_PTR vres) = 0;

		virtual void vecAdd(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres) = 0;
		virtual void vecAdd(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres) = 0;
		virtual void vecAdd(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres) = 0;

		virtual void vecSub(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres) = 0;
		virtual void vecSub(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres) = 0;
		virtual void vecSub(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres) = 0;

		virtual void vecScale(float k, VECTOR4D_PTR vec) = 0;
		virtual void vecScale(float k, VECTOR3D_PTR vec) = 0;
		virtual void vecScale(float k, VECTOR2D_PTR vec) = 0;
		virtual void vecScale(float k, VECTOR4D_PTR vec, VECTOR4D_PTR vres) = 0;
		virtual void vecScale(float k, VECTOR3D_PTR vec, VECTOR3D_PTR vres) = 0;
		virtual void vecScale(float k, VECTOR2D_PTR vec, VECTOR2D_PTR vres) = 0;

		virtual float vecDot(VECTOR4D_PTR va, VECTOR4D_PTR vb) = 0;
		virtual float vecDot(VECTOR3D_PTR va, VECTOR3D_PTR vb) = 0;
		virtual float vecDot(VECTOR2D_PTR va, VECTOR2D_PTR vb) = 0;

		virtual void vecCross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres) = 0;
		virtual void vecCross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres) = 0;

		virtual void vecNormalize(VECTOR4D_PTR vec) = 0;
		virtual void vecNormalize(VECTOR3D_PTR vec) = 0;
		virtual void vecNormalize(VECTOR2D_PTR vec) = 0;
		virtual void vecNormalize(VECTOR4D_PTR vec, VECTOR4D_PTR res) = 0;
		virtual void vecNormalize(VECTOR3D_PTR vec, VECTOR3D_PTR res) = 0;
		virtual void vecNormalize(VECTOR2D_PTR vec, VECTOR2D_PTR res) = 0;

		virtual float vecLength(VECTOR4D_PTR vec) = 0;
		virtual float vecLength(VECTOR3D_PTR vec) = 0;
		virtual float vecLength(VECTOR2D_PTR vec) = 0;

		virtual float vecLengthFast(VECTOR4D_PTR vec) = 0;
		virtual float vecLengthFast(VECTOR3D_PTR vec) = 0;
		virtual float vecLengthFast(VECTOR2D_PTR vec) = 0;

		virtual void vecMulMat(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vres) = 0;
		//VECTORS

		//MATRIX
		virtual void matCopy(MATRIX4X4_PTR mSour, MATRIX4X4_PTR mDest) = 0;

		virtual void matMul(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mres) = 0;

		virtual int matInverse(MATRIX4X4_PTR mat, MATRIX4X4_PTR mres) = 0;

		virtual void matIdentity(MATRIX4X4_PTR mat) = 0;
		//MATRIX
	protected:
		mathObjFunc(){}
	};

	class mathObjFuncNOSIMD : public mathObjFunc
	{
	private:
		static const MATRIX4X4 matIdentity4X4;
	public:
		mathObjFuncNOSIMD(){}
		int Init();
		int Release();*/
		static const MATRIX4X4 matIdentity4X4 = 
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
		//VECTORS
		void vecZero(VECTOR4D_PTR vec);
		void vecZero(VECTOR3D_PTR vec);
		void vecZero(VECTOR2D_PTR vec);

		void vecCopy(VECTOR4D_PTR sour, VECTOR4D_PTR dest);
		void vecCopy(VECTOR3D_PTR sour, VECTOR3D_PTR dest);
		void vecCopy(VECTOR2D_PTR sour, VECTOR2D_PTR dest);

		void vecSwap(VECTOR4D_PTR a, VECTOR4D_PTR b);
		void vecSwap(VECTOR3D_PTR a, VECTOR3D_PTR b);
		void vecSwap(VECTOR2D_PTR a, VECTOR2D_PTR b);

		void vecBuild(POINT4D_PTR ptFrom, POINT4D_PTR ptTo, VECTOR4D_PTR vres);
		void vecBuild(POINT3D_PTR ptFrom, POINT3D_PTR ptTo, VECTOR3D_PTR vres);
		void vecBuild(POINT2D_PTR ptFrom, POINT2D_PTR ptTo, VECTOR2D_PTR vres);

		void vecAdd(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres);
		void vecAdd(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres);
		void vecAdd(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres);

		void vecSub(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres);
		void vecSub(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres);
		void vecSub(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres);

		void vecScale(float k, VECTOR4D_PTR vec);
		void vecScale(float k, VECTOR3D_PTR vec);
		void vecScale(float k, VECTOR2D_PTR vec);
		void vecScale(float k, VECTOR4D_PTR vec, VECTOR4D_PTR vres);
		void vecScale(float k, VECTOR3D_PTR vec, VECTOR3D_PTR vres);
		void vecScale(float k, VECTOR2D_PTR vec, VECTOR2D_PTR vres);

		float vecDot(VECTOR4D_PTR va, VECTOR4D_PTR vb);
		float vecDot(VECTOR3D_PTR va, VECTOR3D_PTR vb);
		float vecDot(VECTOR2D_PTR va, VECTOR2D_PTR vb);

		void vecCross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres);
		void vecCross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres);

		void vecNormalize(VECTOR4D_PTR vec);
		void vecNormalize(VECTOR3D_PTR vec);
		void vecNormalize(VECTOR2D_PTR vec);
		void vecNormalize(VECTOR4D_PTR vec, VECTOR4D_PTR vres);
		void vecNormalize(VECTOR3D_PTR vec, VECTOR3D_PTR vres);
		void vecNormalize(VECTOR2D_PTR vec, VECTOR2D_PTR vres);

		float vecLength(VECTOR4D_PTR vec);
		float vecLength(VECTOR3D_PTR vec);
		float vecLength(VECTOR2D_PTR vec);

		float vecLengthFast(VECTOR4D_PTR vec);
		float vecLengthFast(VECTOR3D_PTR vec);
		float vecLengthFast(VECTOR2D_PTR vec);

		void vecReflect(VECTOR4D_PTR va, VECTOR4D_PTR vn, VECTOR4D_PTR vres);

		void vecMulMat(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vres);
		//VECTORS

		//MATRIX
		void matCopy(MATRIX4X4_PTR mSour, MATRIX4X4_PTR mDest);
		
		void matMul(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mres);

		int matInverse(MATRIX4X4_PTR mat, MATRIX4X4_PTR mres);

		void matIdentity(MATRIX4X4_PTR mat);

		void matAxisRotation(MATRIX4X4_PTR mat, VECTOR4D_PTR axis, float angle);
		//MATRIX
		//PLANES
		void planeInit(PLANE4D_PTR p, POINT4D_PTR p0, VECTOR4D_PTR normal, int normalize);
		int planeGetIntersectPoint(PLANE4D_PTR p, POINT4D_PTR p0, VECTOR4D_PTR v, POINT4D_PTR pres);
		//PLANES
	/*};

	class mathObjFuncSSE1_2 : public mathObjFunc
	{
		
	};*/

	int Init();
	int Release();

	static math3DFunc mathFunc;
	//static mathObjFunc *mathObj = new mathObjFuncNOSIMD();
};