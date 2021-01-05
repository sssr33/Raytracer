#include "Triangle.h"

Triangle::Triangle(
    const vec3<float>& v0, const vec3<float>& v1, const vec3<float>& v2,
    const vec2<float>& t0, const vec2<float>& t1, const vec2<float>& t2,
    ITextureSampler<float>* texSampler
)
	: v0(v0), v1(v1), v2(v2)
    , t0(t0), t1(t1), t2(t2)
    , texSampler(texSampler)
{}

std::optional<HitRecord> Triangle::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection

    vec3<float> v0v1 = v1 - v0;
    vec3<float> v0v2 = v2 - v0;
    vec3<float> pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);
#ifdef CULLING 
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return std::nullopt;
#else 
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < std::numeric_limits<float>::epsilon()) return std::nullopt;
#endif 
    float invDet = 1 / det;

    vec3<float> tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0.f || u > 1.f) return std::nullopt;

    vec3<float> qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0.f || u + v > 1.f) return std::nullopt;

    float t = v0v2.dot(qvec) * invDet;

    if (t > tMin && t < tMax)
    {
        HitRecord hit;

        hit.rayT = t;
        hit.point = ray.pointAtParameter(hit.rayT);
        hit.normal = v0v1.cross(v0v2).normalized();

        // triangle interpolation
        //hit.u = u; // for v0
        //hit.v = v; // for v1
        //hit.w = 1.f - u - v; // for v2

        float ut = u; // for v0
        float vt = v; // for v1
        float wt = 1.f - u - v; // for v2

        vec2<float> pt = wt * t0 + ut * t1 + vt * t2;

        //float color = this->texSampler->Next();


        vec3<float> vAbs = { std::fabsf(ray.direction.x), std::fabsf(ray.direction.y), std::fabsf(ray.direction.z) };

        vec2<float> texCoords;

        if (vAbs.x > vAbs.y && vAbs.x > vAbs.z)
        {
            float t = -ray.origin.x / ray.direction.x;
            texCoords = (ray.origin + t * ray.direction).swizzle.y.z;
        }
        else if (vAbs.y > vAbs.z)
        {
            float t = -ray.origin.y / ray.direction.y;
            texCoords = (ray.origin + t * ray.direction).swizzle.x.z;
        }
        else
        {
            float t = -1.f / ray.direction.z;
            texCoords = (ray.origin + t * ray.direction).swizzle.x.y;
        }


        float color = this->texSampler->Sample(texCoords);

        color += 1.0;
        color /= 2.0;

        hit.color = color;

        return hit;
    }

    return std::nullopt;
}
