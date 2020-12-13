#include "MathTest.h"
#include "Math/vec.h"

void MathTestSwizzle();
void MathTestLinearAlgebra();
void MathTestTriangle();

void MathTest()
{
	MathTestSwizzle();
	MathTestLinearAlgebra();
	MathTestTriangle();
}

void MathTestSwizzle()
{
	{
		vec2<float> v2(1, 2);
		vec3<float> v3(3, 4, 5);
		vec4<float> v4(6, 7, 8, 9);

		sizeof(v2);
		sizeof(v3);
		sizeof(v4);

		v2 = v2.swizzle.y.x;
		v4 = v3.swizzle.x.z.z.y;
		v4 = v2.swizzle.y.y.x.x;

		int stop = 243;
	}

	{
		vec2<float> v1 = { 1.f, 2.f };
		vec2<float> v2 = { 3.f, 4.f };

		v1.swizzle.x.y = v1.swizzle.x.x;
		v1.swizzle.y.x = v2;

		v1 = v1 + v2;
		v1.swizzle.y.x = v1 + v2;

		vec3 v3;
		v3 = v1.swizzle.x.x.x;
		v3 = v1.y;
		v3 = v1.swizzle.y.x.y;

		v1 = { 1.f, 2.f };
		v2 = { 3.f, 4.f };

		v3.swizzle.z.x += v1 + v2;
		v3.swizzle.z.x = v1 + v2;

		v3 = v3.swizzle.g.b.r;

		v3.swizzle.z.x = -v3.swizzle.x.z;

		v1 += 2.f;
		v3.swizzle.z.x += 2.f;

		v2 = v3.swizzle.y.z + v3.swizzle.z.y;
		v3.swizzle.z.x += v3.swizzle.y.z + v3.swizzle.z.y;

		int stop = 243;
	}
}

void MathTestLinearAlgebra()
{
	vec3<float> v1 = { 14.f, 0.f, 0.f };
	vec3<float> v2 = { 0.f, 1111.f, 0.f };

	float v1Length = v1.length();
	float v2Length = v2.length();

	v1.normalize();
	v2 = v2.normalized();

	float v1LengthN = v1.length();
	float v2LengthN = v2.length();

	float v12dp = v1.dot(v2);

	vec3<float> v3 = v1.cross(v2);

	vec3<float> v11 = v2.cross(v3);
	vec3<float> v22 = v3.cross(v1);

	int stop = 234;
}

void MathTestTriangle()
{
	vec3<float> v0 = { 0.f, 0.f, 0.1f };
	vec3<float> v1 = { 1.f, 0.0f, 0.1f };
	vec3<float> v2 = { 0.0f, 1.0f, 0.0f };

	vec3<float> v0v1 = v1 - v0;
	vec3<float> v0v2 = v2 - v0;

	vec3<float> v1v2 = v2 - v1;
	vec3<float> v2v0 = v0 - v2;

	vec3<float> n = v0v1.cross(v0v2);

	n.x = v0v1.y * v0v2.z - v0v1.z * v0v2.y;
	n.y = v0v1.z * v0v2.x - v0v1.x * v0v2.z;
	n.z = v0v1.x * v0v2.y - v0v1.y * v0v2.x;

	float area1 = n.length();

	float angle = std::atan2(v2.y, v2.x) - std::atan2(v1.y, v1.x);
	float s = std::sin(angle);
	float v0v1L = v0v1.length();
	float v0v2L = v0v2.length();
	float area2 = v0v1L * v0v2L * s;

	vec3<float> p = { 0.f, -1.f, 0.f };
	//vec3<float> p = { 0.1f, 0.1f, 0.f };
	//vec3<float> p = { 0.5f, 0.0f, 0.f };
	//vec3<float> p = { 0.0f, 0.5f, 0.f };

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
	{
		vec3<float> v0p = p - v0;
		vec3<float> v1p = p - v1;
		vec3<float> v2p = p - v2;

		vec3<float> c1 = v0v1.cross(v0p);
		vec3<float> c2 = v1v2.cross(v1p);
		vec3<float> c3 = v2v0.cross(v2p);

		float d1 = n.dot(c1);
		float d2 = n.dot(c2);
		float d3 = n.dot(c3);

		int stop = 324;
	}

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates
	{
		float area = n.length() / 2; // area of the triangle 

		vec3<float> v0p = p - v0;
		vec3<float> v1p = p - v1;
		vec3<float> v2p = p - v2;

		vec3<float> c1 = v0v1.cross(v0p);
		vec3<float> c2 = v1v2.cross(v1p);
		vec3<float> c3 = v2v0.cross(v2p);

		float w = (c1.length() / 2.f) / area;
		float u = (c2.length() / 2.f) / area;
		float v = (c3.length() / 2.f) / area;

		auto pp = u * v0 + v * v1 + w * v2;

		int stop = 324;
	}

	{

	}
}
