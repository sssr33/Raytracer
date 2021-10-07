#include "StdAfx.h"
#include "Math3DStructs.h"
#include <math.h>

void math3D::VECTOR4D_TYP::Mat_Mul_VECTOR4D_4X4(MATRIX4X4_PTR m, VECTOR4D_PTR vprod)
{
	for (int col=0; col < 4; col++)
    {
        // compute dot product from row of ma 
        // and column of mb
		float sum = 0; // used to hold result

        for (int row=0; row<4; row++)
        {
             // add in next product pair
			sum+=(this->M[row] * m->M[row][col]);
        } // end for index

        // insert resulting col element
        vprod->M[col] = sum;

     } // end for col

}

void math3D::VECTOR4D_TYP::VECTOR4D_Add(VECTOR4D_PTR vb, VECTOR4D_PTR vsum)
{
	vsum->x = this->x + vb->x;
	vsum->y = this->y + vb->y;
	vsum->z = this->z + vb->z;
	vsum->w = 1;
}

void math3D::VECTOR4D_TYP::VECTOR4D_Sub(VECTOR4D_PTR vb, VECTOR4D_PTR vdiff)
{
	vdiff->x = this->x - vb->x;
	vdiff->y = this->y - vb->y;
	vdiff->z = this->z - vb->z;
	vdiff->w = 1;
}

void math3D::VECTOR4D_TYP::VECTOR4D_Build(VECTOR4D_PTR init, VECTOR4D_PTR term)
{
	this->x = term->x - init->x;
	this->y = term->y - init->y;
	this->z = term->z - init->z;
	this->w = 1;
}

float math3D::VECTOR4D_TYP::VECTOR4D_Dot(VECTOR4D_PTR vb)
{
	return( (this->x * vb->x) + (this->y * vb->y) + (this->z * vb->z) );
}

void math3D::VECTOR4D_TYP::VECTOR4D_Cross(VECTOR4D_PTR vb, VECTOR4D_PTR vn)
{
	vn->x =  ( (this->y * vb->z) - (this->z * vb->y) );
	vn->y = -( (this->x * vb->z) - (this->z * vb->x) );
	vn->z =  ( (this->x * vb->y) - (this->y * vb->x) ); 
	vn->w = 1;
}

void math3D::VECTOR4D_TYP::VECTOR4D_Normalize()
{
	// compute length
	float length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5) 
	   return;

	float length_inv = 1.0f / length;

	// compute normalized version of vector
	this->x *= length_inv;
	this->y *= length_inv;
	this->z *= length_inv;
	this->w = 1;
}

/*float math3D::VECTOR4D_TYP::VECTOR4D_Length_Fast()
{
	return math3D::mathFunc.Fast_Distance_3D(this->x, this->y, this->z);
}*/

float math3D::VECTOR4D_TYP::VECTOR4D_Length()
{
	return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
}

void math3D::MATRIX4X4_TYP::Build_Model_To_World_MATRIX4X4(VECTOR4D_PTR vPos)
{
	this->Mat_Init_4X4(1, 0, 0, 0,
					   0, 1, 0, 0,
					   0, 0, 1, 0,
					   vPos->x, vPos->y, vPos->z, 1);
}

void math3D::MATRIX4X4_TYP::Build_XYZ_Rotation_MATRIX4X4(float theta_x, float theta_y, float theta_z)
{
MATRIX4X4 mx, my, mz, mtmp;       // working matrices
float sin_theta=0, cos_theta=0;   // used to initialize matrices
int rot_seq = 0;                  // 1 for x, 2 for y, 4 for z

// step 0: fill in with identity matrix
this->MAT_IDENTITY_4X4();

// step 1: based on zero and non-zero rotation angles, determine
// rotation sequence
if (fabs(theta_x) > EPSILON_E5) // x
   rot_seq = rot_seq | 1;

if (fabs(theta_y) > EPSILON_E5) // y
   rot_seq = rot_seq | 2;

if (fabs(theta_z) > EPSILON_E5) // z
   rot_seq = rot_seq | 4;

// now case on sequence
switch(rot_seq)
      {
      case 0: // no rotation
      {
      // what a waste!
      return;
      } break;

      case 1: // x rotation
      {
      // compute the sine and cosine of the angle
	  cos_theta = math3D::mathFunc.Fast_Cos(theta_x);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_x);

      // set the matrix up 
	  mx.Mat_Init_4X4(1,    0,          0,         0,
                        0,    cos_theta,  sin_theta, 0,
                        0,   -sin_theta, cos_theta, 0,
                        0,    0,          0,         1);

      // that's it, copy to output matrix
	  mx.Mat_Copy(this);
      return;

      } break;

      case 2: // y rotation
      {
      // compute the sine and cosine of the angle
      cos_theta = math3D::mathFunc.Fast_Cos(theta_y);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_y);

      // set the matrix up 
	  my.Mat_Init_4X4(cos_theta, 0, -sin_theta, 0,  
                       0,         1,  0,         0,
                       sin_theta, 0, cos_theta,  0,
                       0,         0, 0,          1);


      // that's it, copy to output matrix
	  my.Mat_Copy(this);
      return;

      } break;

      case 3: // xy rotation
      {
      // compute the sine and cosine of the angle for x
      cos_theta = math3D::mathFunc.Fast_Cos(theta_x);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_x);

      // set the matrix up 
	  mx.Mat_Init_4X4( 1,    0,          0,         0,
                        0,    cos_theta,  sin_theta, 0,
                        0,   -sin_theta, cos_theta, 0,
                        0,    0,          0,         1);

      // compute the sine and cosine of the angle for y
      cos_theta = math3D::mathFunc.Fast_Cos(theta_y);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_y);

      // set the matrix up 
	  my.Mat_Init_4X4(cos_theta, 0, -sin_theta, 0,  
                       0,         1,  0,         0,
                       sin_theta, 0, cos_theta,  0,
                       0,         0, 0,          1);

      // concatenate matrices
	  mx.Mat_Mul_4X4(&my, this);
      return;

      } break;

      case 4: // z rotation
      {
      // compute the sine and cosine of the angle
      cos_theta = math3D::mathFunc.Fast_Cos(theta_z);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_z);

      // set the matrix up 
	  mz.Mat_Init_4X4( cos_theta, sin_theta, 0, 0,  
                       -sin_theta, cos_theta, 0, 0,
                        0,         0,         1, 0,
                        0,         0,         0, 1);


      // that's it, copy to output matrix
	  mz.Mat_Copy(this);
      return;

      } break;

      case 5: // xz rotation
      {
      // compute the sine and cosine of the angle x
      cos_theta = math3D::mathFunc.Fast_Cos(theta_x);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_x);

      // set the matrix up 
	  mx.Mat_Init_4X4( 1,    0,          0,         0,
                        0,    cos_theta,  sin_theta, 0,
                        0,   -sin_theta, cos_theta, 0,
                        0,    0,          0,         1);

      // compute the sine and cosine of the angle z
      cos_theta = math3D::mathFunc.Fast_Cos(theta_z);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_z);

      // set the matrix up 
	  mz.Mat_Init_4X4( cos_theta, sin_theta, 0, 0,  
                       -sin_theta, cos_theta, 0, 0,
                        0,         0,         1, 0,
                        0,         0,         0, 1);

      // concatenate matrices 
	  mx.Mat_Mul_4X4(&mz, this);
      return;

      } break;

      case 6: // yz rotation
      {
      // compute the sine and cosine of the angle y
      cos_theta = math3D::mathFunc.Fast_Cos(theta_y);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_y);

      // set the matrix up 
	  my.Mat_Init_4X4(cos_theta, 0, -sin_theta, 0,  
                       0,         1,  0,         0,
                       sin_theta, 0, cos_theta,  0,
                       0,         0, 0,          1);

      // compute the sine and cosine of the angle z
      cos_theta = math3D::mathFunc.Fast_Cos(theta_z);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_z);

      // set the matrix up 
	  mz.Mat_Init_4X4( cos_theta, sin_theta, 0, 0,  
                       -sin_theta, cos_theta, 0, 0,
                        0,         0,         1, 0,
                        0,         0,         0, 1);

      // concatenate matrices 
	  my.Mat_Mul_4X4(&mz, this);
      return;

      } break;

      case 7: // xyz rotation
      {
      // compute the sine and cosine of the angle x
      cos_theta = math3D::mathFunc.Fast_Cos(theta_x);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_x);

      // set the matrix up 
	  mx.Mat_Init_4X4( 1,    0,         0,         0,
                        0,    cos_theta, sin_theta, 0,
                        0,   -sin_theta, cos_theta, 0,
                        0,    0,         0,         1);

      // compute the sine and cosine of the angle y
      cos_theta = math3D::mathFunc.Fast_Cos(theta_y);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_y);

      // set the matrix up 
	  my.Mat_Init_4X4(cos_theta, 0, -sin_theta, 0,  
                       0,         1,  0,         0,
                       sin_theta, 0,  cos_theta,  0,
                       0,         0,  0,          1);

      // compute the sine and cosine of the angle z
      cos_theta = math3D::mathFunc.Fast_Cos(theta_z);
      sin_theta = math3D::mathFunc.Fast_Sin(theta_z);

      // set the matrix up 
	  mz.Mat_Init_4X4( cos_theta, sin_theta, 0, 0,  
                       -sin_theta, cos_theta, 0, 0,
                        0,         0,         1, 0,
                        0,         0,         0, 1);

      // concatenate matrices, watch order!
	  mx.Mat_Mul_4X4(&my, &mtmp);
	  mtmp.Mat_Mul_4X4(&mz, this);

      } break;

      default: break;
      
      } // end switch
}

void math3D::MATRIX4X4_TYP::Mat_Mul_4X4(MATRIX4X4_PTR mb, MATRIX4X4_PTR mprod)
{
	for (int row=0; row<4; row++)
    {
		for (int col=0; col<4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb
			float sum = 0; // used to hold result
			for (int index=0; index<4; index++)
			{
				 // add in next product pair
				 sum += (this->M[row][index] * mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			mprod->M[row][col] = sum;

		} // end for col
    } // end for row
}

int math3D::MATRIX4X4_TYP::Mat_Inverse_4X4(MATRIX4X4_PTR mi)
{
	float det =  ( this->M00 * ( this->M11 * this->M22 - this->M12 * this->M21 ) -
               this->M01 * ( this->M10 * this->M22 - this->M12 * this->M20 ) +
               this->M02 * ( this->M10 * this->M21 - this->M11 * this->M20 ) );

// test determinate to see if it's 0
if (fabs(det) < EPSILON_E5)
   return(0);

float det_inv  = 1.0f / det;

mi->M00 =  det_inv * ( this->M11 * this->M22 - this->M12 * this->M21 );
mi->M01 = -det_inv * ( this->M01 * this->M22 - this->M02 * this->M21 );
mi->M02 =  det_inv * ( this->M01 * this->M12 - this->M02 * this->M11 );
mi->M03 = 0.0f; // always 0

mi->M10 = -det_inv * ( this->M10 * this->M22 - this->M12 * this->M20 );
mi->M11 =  det_inv * ( this->M00 * this->M22 - this->M02 * this->M20 );
mi->M12 = -det_inv * ( this->M00 * this->M12 - this->M02 * this->M10 );
mi->M13 = 0.0f; // always 0

mi->M20 =  det_inv * ( this->M10 * this->M21 - this->M11 * this->M20 );
mi->M21 = -det_inv * ( this->M00 * this->M21 - this->M01 * this->M20 );
mi->M22 =  det_inv * ( this->M00 * this->M11 - this->M01 * this->M10 );
mi->M23 = 0.0f; // always 0

mi->M30 = -( this->M30 * mi->M00 + this->M31 * mi->M10 + this->M32 * mi->M20 );
mi->M31 = -( this->M30 * mi->M01 + this->M31 * mi->M11 + this->M32 * mi->M21 );
mi->M32 = -( this->M30 * mi->M02 + this->M31 * mi->M12 + this->M32 * mi->M22 );
mi->M33 = 1.0f; // always 0

// return success 
return(1);

}

void math3D::PLANE3D_TYP::PLANE3D_Init(POINT3D_PTR p0, VECTOR3D_PTR normal, int normalize=0)
{
// this function initializes a 3d plane

// copy the point
	p0->VECTOR3D_COPY(&this->p0);

// if normalize is 1 then the normal is made into a unit vector
	if (!normalize)
		normal->VECTOR3D_COPY(&this->n);
	else
	{
	   // make normal into unit vector
	   normal->VECTOR3D_Normalize(&this->n);
	} // end else

}

void math3D::VECTOR3D_TYP::VECTOR3D_Normalize()
{
	// normalizes the sent vector in placew

	// compute length
	float length = sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5) 
	   return;

	float length_inv = 1/length;

	// compute normalized version of vector
	this->x *= length_inv;
	this->y *= length_inv;
	this->z *= length_inv;

} // end VECTOR3D_Normalize

void math3D::VECTOR3D_TYP::VECTOR3D_Normalize(VECTOR3D_PTR vn)
{
	// normalizes the sent vector and returns the result in vn

	vn->VECTOR3D_ZERO();

	// compute length
	float length = this->VECTOR3D_Length();

	// test for zero length vector
	// if found return zero vector
	if (length < EPSILON_E5) 
	   return;

	float length_inv = 1.0f / length;

	// compute normalized version of vector
	vn->x = this->x * length_inv;
	vn->y = this->y * length_inv;
	vn->z = this->z * length_inv;
} // end VECTOR3D_Normalize

float math3D::VECTOR3D_TYP::VECTOR3D_Length()
{
	return( (float)sqrtf(this->x * this->x + this->y * this->y + this->z * this->z) );
}


int math3D::Init()
{
	/*if(Sys::getSIMDInfo(SIMD(0)))
	{MessageBox(0,0,0,0);}
	else
		math3D::mathObj = new mathObjFuncNOSIMD();
	
	if(!mathObj) return 0;
	
	mathObj->Init();*/
	
	return 1;
}
int math3D::Release()
{
	/*if(mathObj)
	{
		mathObj->Release();
		delete mathObj;
		mathObj = 0;
	}*/

	return 1;
}

/*int math3D::mathObjFuncNOSIMD::Init()
{
	return 1;
}
int math3D::mathObjFuncNOSIMD::Release()
{
	return 1;
}*/

//const math3D::MATRIX4X4 math3D::matIdentity4X4 = 
//{
//	1, 0, 0, 0,
//	0, 1, 0, 0,
//	0, 0, 1, 0,
//	0, 0, 0, 1
//};

void math3D::vecZero(VECTOR4D_PTR vec)
{
	vec->x = vec->y = vec->z = 0.0f;
	vec->w = 1.0f;
}
void math3D::vecZero(VECTOR3D_PTR vec)
{
	vec->x = vec->y = vec->z = 0.0f;
}
void math3D::vecZero(VECTOR2D_PTR vec)
{
	vec->x = vec->y = 0.0f;
}

void math3D::vecCopy(VECTOR4D_PTR sour, VECTOR4D_PTR dest)
{
	dest->x = sour->x;
	dest->y = sour->y;
	dest->z = sour->z;
	dest->w = sour->w;
}
void math3D::vecCopy(VECTOR3D_PTR sour, VECTOR3D_PTR dest)
{
	dest->x = sour->x;
	dest->y = sour->y;
	dest->z = sour->z;
}
void math3D::vecCopy(VECTOR2D_PTR sour, VECTOR2D_PTR dest)
{
	dest->x = sour->x;
	dest->y = sour->y;
}

void math3D::vecSwap(VECTOR4D_PTR a, VECTOR4D_PTR b)
{
	VECTOR4D vecTmp;

	math3D::vecCopy(a, &vecTmp);
	math3D::vecCopy(b, a);
	math3D::vecCopy(&vecTmp, b);
}
void math3D::vecSwap(VECTOR3D_PTR a, VECTOR3D_PTR b)
{
	VECTOR3D vecTmp;

	math3D::vecCopy(a, &vecTmp);
	math3D::vecCopy(b, a);
	math3D::vecCopy(&vecTmp, b);
}
void math3D::vecSwap(VECTOR2D_PTR a, VECTOR2D_PTR b)
{
	VECTOR2D vecTmp;

	math3D::vecCopy(a, &vecTmp);
	math3D::vecCopy(b, a);
	math3D::vecCopy(&vecTmp, b);
}

void math3D::vecBuild(POINT4D_PTR ptFrom, POINT4D_PTR ptTo, VECTOR4D_PTR vres)
{
	vres->x = ptTo->x - ptFrom->x;
	vres->y = ptTo->y - ptFrom->y;
	vres->z = ptTo->z - ptFrom->z;
	vres->w = 1.0f;
}
void math3D::vecBuild(POINT3D_PTR ptFrom, POINT3D_PTR ptTo, VECTOR3D_PTR vres)
{
	vres->x = ptTo->x - ptFrom->x;
	vres->y = ptTo->y - ptFrom->y;
	vres->z = ptTo->z - ptFrom->z;
}
void math3D::vecBuild(POINT2D_PTR ptFrom, POINT2D_PTR ptTo, VECTOR2D_PTR vres)
{
	vres->x = ptTo->x - ptFrom->x;
	vres->y = ptTo->y - ptFrom->y;
}

void math3D::vecAdd(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres)
{
	vres->x = va->x + vb->x;
	vres->y = va->y + vb->y;
	vres->z = va->z + vb->z;
	vres->w = 1.0f;
}
void math3D::vecAdd(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres)
{
	vres->x = va->x + vb->x;
	vres->y = va->y + vb->y;
	vres->z = va->z + vb->z;
}
void math3D::vecAdd(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres)
{
	vres->x = va->x + vb->x;
	vres->y = va->y + vb->y;
}

void math3D::vecSub(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres)
{
	vres->x = va->x - vb->x;
	vres->y = va->y - vb->y;
	vres->z = va->z - vb->z;
	vres->w = 1.0f;
}
void math3D::vecSub(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres)
{
	vres->x = va->x - vb->x;
	vres->y = va->y - vb->y;
	vres->z = va->z - vb->z;
}
void math3D::vecSub(VECTOR2D_PTR va, VECTOR2D_PTR vb, VECTOR2D_PTR vres)
{
	vres->x = va->x - vb->x;
	vres->y = va->y - vb->y;
}

void math3D::vecScale(float k, VECTOR4D_PTR vec)
{
	vec->x *= k;
	vec->y *= k;
	vec->z *= k;
	vec->w = 1.0f;
}
void math3D::vecScale(float k, VECTOR3D_PTR vec)
{
	vec->x *= k;
	vec->y *= k;
	vec->z *= k;
}
void math3D::vecScale(float k, VECTOR2D_PTR vec)
{
	vec->x *= k;
	vec->y *= k;
}
void math3D::vecScale(float k, VECTOR4D_PTR vec, VECTOR4D_PTR vres)
{
	vres->x = vec->x * k;
	vres->y = vec->y * k;
	vres->z = vec->z * k;
	vres->w = 1.0f;
}
void math3D::vecScale(float k, VECTOR3D_PTR vec, VECTOR3D_PTR vres)
{
	vres->x = vec->x * k;
	vres->y = vec->y * k;
	vres->z = vec->z * k;
}
void math3D::vecScale(float k, VECTOR2D_PTR vec, VECTOR2D_PTR vres)
{
	vres->x = vec->x * k;
	vres->y = vec->y * k;
}

float math3D::vecDot(VECTOR4D_PTR va, VECTOR4D_PTR vb)
{
	return (va->x * vb->x + va->y * vb->y + va->z * vb->z);
}
float math3D::vecDot(VECTOR3D_PTR va, VECTOR3D_PTR vb)
{
	return (va->x * vb->x + va->y * vb->y + va->z * vb->z);
}
float math3D::vecDot(VECTOR2D_PTR va, VECTOR2D_PTR vb)
{
	return (va->x * vb->x + va->y * vb->y);
}

void math3D::vecCross(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vres)
{
	VECTOR4D vecTmp;

	vecTmp.x =  ( (va->y * vb->z) - (va->z * vb->y) );
	vecTmp.y = -( (va->x * vb->z) - (va->z * vb->x) );
	vecTmp.z =  ( (va->x * vb->y) - (va->y * vb->x) ); 
	vecTmp.w = 1.0f;

	math3D::vecCopy(&vecTmp, vres);
}
void math3D::vecCross(VECTOR3D_PTR va, VECTOR3D_PTR vb, VECTOR3D_PTR vres)
{
	VECTOR3D vecTmp;

	vecTmp.x =  ( (va->y * vb->z) - (va->z * vb->y) );
	vecTmp.y = -( (va->x * vb->z) - (va->z * vb->x) );
	vecTmp.z =  ( (va->x * vb->y) - (va->y * vb->x) ); 

	math3D::vecCopy(&vecTmp, vres);
}

void math3D::vecNormalize(VECTOR4D_PTR vec)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vec->x *= length_inv;
	vec->y *= length_inv;
	vec->z *= length_inv;
	vec->w = 1;
}
void math3D::vecNormalize(VECTOR3D_PTR vec)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vec->x *= length_inv;
	vec->y *= length_inv;
	vec->z *= length_inv;
}
void math3D::vecNormalize(VECTOR2D_PTR vec)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vec->x *= length_inv;
	vec->y *= length_inv;
}
void math3D::vecNormalize(VECTOR4D_PTR vec, VECTOR4D_PTR vres)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vres->x = vec->x * length_inv;
	vres->y = vec->y * length_inv;
	vres->z = vec->z * length_inv;
	vres->w = 1;
}
void math3D::vecNormalize(VECTOR3D_PTR vec, VECTOR3D_PTR vres)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vres->x = vec->x * length_inv;
	vres->y = vec->y * length_inv;
	vres->z = vec->z * length_inv;
}
void math3D::vecNormalize(VECTOR2D_PTR vec, VECTOR2D_PTR vres)
{
	float length = sqrtf(vec->x*vec->x + vec->y*vec->y);
	
	if (length < EPSILON_E5) 
		return;

	float length_inv = 1.0f / length;

	vres->x = vec->x * length_inv;
	vres->y = vec->y * length_inv;
}

float math3D::vecLength(VECTOR4D_PTR vec)
{
	return sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
}
float math3D::vecLength(VECTOR3D_PTR vec)
{
	return sqrtf(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
}
float math3D::vecLength(VECTOR2D_PTR vec)
{
	return sqrtf(vec->x*vec->x + vec->y*vec->y);
}

float math3D::vecLengthFast(VECTOR4D_PTR vec)
{
	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = static_cast<int>(fabs(vec->x) * 1024.f);
	y = static_cast<int>(fabs(vec->y) * 1024.f);
	z = static_cast<int>(fabs(vec->z) * 1024.f);

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
float math3D::vecLengthFast(VECTOR3D_PTR vec)
{
	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = static_cast<int>(fabs(vec->x) * 1024.f);
	y = static_cast<int>(fabs(vec->y) * 1024.f);
	z = static_cast<int>(fabs(vec->z) * 1024.f);

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
float math3D::vecLengthFast(VECTOR2D_PTR vec)
{
	int x = static_cast<int>(abs(vec->x));
	int y = static_cast<int>(abs(vec->y));

	// compute the minimum of x,y
	int mn = min(x,y);

	// return the distance
	return static_cast<float>(x+y-(mn>>1)-(mn>>2)+(mn>>4));
}

void math3D::vecReflect(VECTOR4D_PTR va, VECTOR4D_PTR vn, VECTOR4D_PTR vres)
{
	//R = 2 * N * dot(V, N) / dot(N, N) - V

	float dp = vecDot(va, vn);
	float dp2 = vecDot(vn, vn);

	vres->x = va->x - (vn->x * dp) * 2;
	vres->y = va->y - (vn->y * dp) * 2;
	vres->z = va->z - (vn->z * dp) * 2;
	/*vres->x = 2 * vn->x * dp1 / dp2 - va->x;
	vres->y = 2 * vn->y * dp1 / dp2 - va->y;
	vres->z = 2 * vn->z * dp1 / dp2 - va->z;*/
	vres->w = 0;
}

void math3D::vecMulMat(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vres)
{
	for (int col=0; col < 4; col++)
    {
        // compute dot product from row of ma 
        // and column of mb
		float sum = 0; // used to hold result

        for (int row=0; row<4; row++)
        {
             // add in next product pair
			sum+=(va->M[row] * mb->M[row][col]);
        } // end for index

        // insert resulting col element
        vres->M[col] = sum;

     } // end for col
}

void math3D::matCopy(MATRIX4X4_PTR mSour, MATRIX4X4_PTR mDest)
{
	memcpy(&mDest->M, &mSour->M, sizeof(MATRIX4X4));
}

void math3D::matMul(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mres)
{
	for (int row=0; row<4; row++)
    {
		for (int col=0; col<4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb
			float sum = 0; // used to hold result
			for (int index=0; index<4; index++)
			{
				 // add in next product pair
				 sum += (ma->M[row][index] * mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			mres->M[row][col] = sum;

		} // end for col
    } // end for row
}

int math3D::matInverse(MATRIX4X4_PTR mat, MATRIX4X4_PTR mres)
{
	float det =  ( mat->M00 * ( mat->M11 * mat->M22 - mat->M12 * mat->M21 ) -
               mat->M01 * ( mat->M10 * mat->M22 - mat->M12 * mat->M20 ) +
               mat->M02 * ( mat->M10 * mat->M21 - mat->M11 * mat->M20 ) );

	// test determresnate to see if it's 0
	if (fabs(det) < EPSILON_E5)
	   return(0);

	float det_inv  = 1.0f / det;

	mres->M00 =  det_inv * ( mat->M11 * mat->M22 - mat->M12 * mat->M21 );
	mres->M01 = -det_inv * ( mat->M01 * mat->M22 - mat->M02 * mat->M21 );
	mres->M02 =  det_inv * ( mat->M01 * mat->M12 - mat->M02 * mat->M11 );
	mres->M03 = 0.0f;

	mres->M10 = -det_inv * ( mat->M10 * mat->M22 - mat->M12 * mat->M20 );
	mres->M11 =  det_inv * ( mat->M00 * mat->M22 - mat->M02 * mat->M20 );
	mres->M12 = -det_inv * ( mat->M00 * mat->M12 - mat->M02 * mat->M10 );
	mres->M13 = 0.0f;

	mres->M20 =  det_inv * ( mat->M10 * mat->M21 - mat->M11 * mat->M20 );
	mres->M21 = -det_inv * ( mat->M00 * mat->M21 - mat->M01 * mat->M20 );
	mres->M22 =  det_inv * ( mat->M00 * mat->M11 - mat->M01 * mat->M10 );
	mres->M23 = 0.0f;

	mres->M30 = -( mat->M30 * mres->M00 + mat->M31 * mres->M10 + mat->M32 * mres->M20 );
	mres->M31 = -( mat->M30 * mres->M01 + mat->M31 * mres->M11 + mat->M32 * mres->M21 );
	mres->M32 = -( mat->M30 * mres->M02 + mat->M31 * mres->M12 + mat->M32 * mres->M22 );
	mres->M33 = 1.0f;

	return(1);
}

void math3D::matIdentity(MATRIX4X4_PTR mat)
{
	memcpy(mat, &math3D::matIdentity4X4, sizeof(math3D::MATRIX4X4));
}

void math3D::planeInit(PLANE4D_PTR p, POINT4D_PTR p0, VECTOR4D_PTR normal, int normalize)
{
	if(normalize)
		vecNormalize(normal);

	vecCopy(p0, &p->p0);
	vecCopy(normal, &p->n);
}
int math3D::planeGetIntersectPoint(PLANE4D_PTR p, POINT4D_PTR p0, VECTOR4D_PTR v, POINT4D_PTR pres)
{
	POINT4D p1;

	vecNormalize(v);

	//vecScale(100, v);

	vecAdd(p0, v, &p1);

	vecCopy(&p1, pres);

	vecNormalize(&p->n);

	float t = 0;
	float dpVN = (p->n.x * (p1.x - p0->x) + p->n.y * (p1.y - p0->y) + p->n.z * (p1.z - p0->z));
	float dpMagic = vecDot(&p->n, p0);

	//
	if(dpVN == 0.0f) return 0;
	if((dpVN * dpMagic) > 0.0f) return 0;

	t = (p->n.x * (p->p0.x - p0->x) + p->n.y * (p->p0.y - p0->y) + p->n.z * (p->p0.z - p0->z)) / dpVN;

	if(t < 0) return 0;
	if(t == numeric_limits<float>::infinity()) return 0;

	pres->x = p0->x + t * (p1.x - p0->x);
	pres->y = p0->y + t * (p1.y - p0->y);
	pres->z = p0->z + t * (p1.z - p0->z);

	return 1;
}

void math3D::matAxisRotation(MATRIX4X4_PTR mat, VECTOR4D_PTR axis, float angle)
{
	float cos_theta = math3D::mathFunc.Fast_Cos(angle);
    float sin_theta = math3D::mathFunc.Fast_Sin(angle);
	float one_minus_cos = 1.0f - cos_theta;

	vecNormalize(axis, axis);

	mat->M00 = cos_theta + one_minus_cos * (axis->x * axis->x);
	mat->M01 = one_minus_cos * axis->x * axis->y - sin_theta * axis->z;
	mat->M02 = one_minus_cos * axis->x * axis->z + sin_theta * axis->y;
	mat->M03 = 0.0f;

	mat->M10 = one_minus_cos * axis->x * axis->y + sin_theta * axis->z;
	mat->M11 = cos_theta + one_minus_cos * (axis->y * axis->y);
	mat->M12 = one_minus_cos * axis->y * axis->z - sin_theta * axis->x;
	mat->M13 = 0.0f;

	mat->M20 = one_minus_cos * axis->x * axis->z - sin_theta * axis->y;
	mat->M21 = one_minus_cos * axis->y * axis->z + sin_theta * axis->x;
	mat->M22 = cos_theta + one_minus_cos * (axis->z * axis->z);
	mat->M23 = 0.0f;

	mat->M30 = mat->M31 = mat->M32 = 0.0f;
	mat->M33 = 1.0f;
}