#include "MathTest.h"
#include "Math/vec.h"

void MathTestSwizzle();
void MathTestLinearAlgebra();

void MathTest()
{
    MathTestSwizzle();
    MathTestLinearAlgebra();
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
