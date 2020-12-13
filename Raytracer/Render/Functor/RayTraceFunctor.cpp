#include "RayTraceFunctor.h"
#include "Math/vec.h"
#include "Render/Camera.h"

RayTraceFunctor::RayTraceFunctor(
	ImageView<BGRA<uint8_t>>& image,
	const RayTraceFunctorParams& params
)
	: image(image)
	, params(params)
{}

void RayTraceFunctor::operator()(const MassiveCompute::Block& block)
{
    const vec2<float> imageSize(static_cast<float>(block.imageWidth), static_cast<float>(block.imageHeight));
    Camera camera(imageSize.x / imageSize.y);

    camera.SetOrigin({ this->params.cameraX, 0.f, 0.f });

    for (size_t yRow = block.top; yRow < block.bottom; yRow++)
    {
        BGRA<uint8_t>* row = image.GetRow(yRow);
        const size_t y = block.imageHeight - 1 - yRow;

        for (size_t x = block.left; x < block.right; x++)
        {
            const vec2<float> uv = vec2<float>(static_cast<float>(x), static_cast<float>(y)) / imageSize;

            const ray<float> r = camera.GetRay(uv);
            const vec3<float> col = this->Color(r);

            const vec3<float> col8Bit = col * 255.99f;
            BGRA<uint8_t> pixel = {};

            pixel.r = static_cast<uint8_t>(col8Bit.r);
            pixel.g = static_cast<uint8_t>(col8Bit.g);
            pixel.b = static_cast<uint8_t>(col8Bit.b);
            pixel.a = 255;

            row[x] = pixel;
        }
    }
}

vec3<float> RayTraceFunctor::Color(const ray<float>& ray) const
{
    SphereHit sphereHit = this->HitSphere({ 0.f, 0.f, -1.f }, 0.5f, ray);
    TriangleHit triangleHit = this->HitTriangle({ -1.f, -0.5f, -1.f }, { -0.5f, 0.f, -1.f }, { -1.f, 0.5f, -1.f }, ray);

    if (sphereHit.hit && triangleHit.hit)
    {
        sphereHit.hit = sphereHit.t < triangleHit.t;
        triangleHit.hit = triangleHit.t < sphereHit.t;
    }

    if (sphereHit.hit)
    {
        vec3<float> color = 0.5f * (sphereHit.normal + 1.f);
        return color;
        //return { 1.f, 0.f, 0.f };
    }

    if (triangleHit.hit)
    {
        vec3<float> color = 0.5f * (triangleHit.normal + 1.f);
        return color;
        //return { 0.f, 1.f, 0.f };
    }

    vec3<float> unitDirection = ray.direction.normalized();
    float t = 0.5f * (unitDirection.y + 1.f);
    return (1.f - t) * vec3<float>(1.f) + t * vec3<float>(0.5f, 0.7f, 1.f);
}

RayTraceFunctor::SphereHit RayTraceFunctor::HitSphere(const vec3<float>& center, float radius, const ray<float>& ray) const
{
    vec3<float> fromSphereToRayOrigin = ray.origin - center;

    float a = ray.direction.dot(ray.direction);
    float b = 2.f * ray.direction.dot(fromSphereToRayOrigin);
    float c = fromSphereToRayOrigin.dot(fromSphereToRayOrigin) - radius * radius;

    float discriminant = b * b - 4.f * a * c;

    SphereHit hit;

    if (discriminant >= 0.f)
    {
        // farthest T
        //float t1 = (-b + std::sqrt(discriminant)) / (2.f * a);
        // closest T
        float t = (-b - std::sqrt(discriminant)) / (2.f * a);

        hit.t = t;
        hit.normal = (ray.pointAtParameter(t) - center).normalized();
        hit.hit = true;
    }

    return hit;
}

RayTraceFunctor::TriangleHit RayTraceFunctor::HitTriangle(const vec3<float>& v0, const vec3<float>& v1, const vec3<float>& v2, const ray<float>& ray) const
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection

    vec3<float> v0v1 = v1 - v0;
    vec3<float> v0v2 = v2 - v0;
    vec3<float> pvec = ray.direction.cross(v0v2);
    float det = v0v1.dot(pvec);
#ifdef CULLING 
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false;
#else 
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < std::numeric_limits<float>::epsilon()) return TriangleHit();
#endif 
    float invDet = 1 / det;

    vec3<float> tvec = ray.origin - v0;
    float u = tvec.dot(pvec) * invDet;
    if (u < 0.f || u > 1.f) return TriangleHit();

    vec3<float> qvec = tvec.cross(v0v1);
    float v = ray.direction.dot(qvec) * invDet;
    if (v < 0.f || u + v > 1.f) return TriangleHit();

    float t = v0v2.dot(qvec) * invDet;

    TriangleHit hit;

    hit.hit = true;
    hit.t = t;
    hit.u = u;
    hit.v = v;
    hit.normal = v0v1.cross(v0v2).normalized();

    return hit;
}
