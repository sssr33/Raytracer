#include "SutherlandHodgmanTriangleClipper4D.h"

#include <cmath>

std::vector<POINT4D> SutherlandHodgmanTriangleClipper4D::Clip(const POINT4D& a, const POINT4D& b, const POINT4D& c) {
    std::vector<POINT4D> current = { a, b, c };
    std::vector<POINT4D> next;

    this->IntersectPlane(current, next, LeftPlane{});
    this->IntersectPlane(current, next, RightPlane{});
    this->IntersectPlane(current, next, TopPlane{});
    this->IntersectPlane(current, next, BottomPlane{});
    this->IntersectPlane(current, next, FarPlane{});
    this->IntersectPlane(current, next, NearPlane{});

    return current;
}

template<class PlaneT>
void SutherlandHodgmanTriangleClipper4D::IntersectPlane(
    std::vector<POINT4D>& current,
    std::vector<POINT4D>& next,
    const PlaneT& plane
) const
{
    auto a = current.back();

    for (const auto& b : current) {
        const bool aInside = !plane.IsOutside(a);
        const bool bInside = !plane.IsOutside(b);

        if (aInside != bInside) {
            // one inside, other outside, need to clip
            POINT4D intersection = plane.Clip(a, b);

            if (!aInside) {
                // intersection is a
                next.push_back(intersection);
                next.push_back(b);
            }
            else {
                // intersection is b
                next.push_back(intersection);
            }
        }
        else if (aInside && bInside) {
            next.push_back(b);
        }

        a = b;
    }

    std::swap(current, next);
    next.clear();
}

POINT4D SutherlandHodgmanTriangleClipper4D::Lerp(const POINT4D& a, const POINT4D& b, float t) {
    POINT4D res;

    res.x = a.x + t * (b.x - a.x);
    res.y = a.y + t * (b.y - a.y);
    res.z = a.z + t * (b.z - a.z);
    res.w = a.w + t * (b.w - a.w);

    return res;
}




bool SutherlandHodgmanTriangleClipper4D::PositivePlane::IsOutside(const POINT4D& pt, float POINT4D::* axis) {
    return pt.*axis > pt.w;
}

POINT4D SutherlandHodgmanTriangleClipper4D::PositivePlane::Clip(POINT4D a, POINT4D b, float POINT4D::* axis) {
    if (std::fabs(b.*axis) < std::fabs(a.*axis)) {
        std::swap(a, b);
    }

    float t = (a.w - a.*axis) / (b.*axis - a.*axis - b.w + a.w);
    auto res = SutherlandHodgmanTriangleClipper4D::Lerp(a, b, t);
    return res;
}

bool SutherlandHodgmanTriangleClipper4D::NegativePlane::IsOutside(const POINT4D& pt, float POINT4D::* axis) {
    return pt.*axis < -pt.w;
}

POINT4D SutherlandHodgmanTriangleClipper4D::NegativePlane::Clip(POINT4D a, POINT4D b, float POINT4D::* axis) {
    if (std::fabs(b.*axis) < std::fabs(a.*axis)) {
        std::swap(a, b);
    }

    float t = (a.w + a.*axis) / (a.w - b.w + a.*axis - b.*axis);
    auto res = SutherlandHodgmanTriangleClipper4D::Lerp(a, b, t);
    return res;
}

bool SutherlandHodgmanTriangleClipper4D::LeftPlane::IsOutside(const POINT4D& pt) const {
    return NegativePlane::IsOutside(pt, &POINT4D::x);
}

POINT4D SutherlandHodgmanTriangleClipper4D::LeftPlane::Clip(const POINT4D& a, const POINT4D& b) const {
    return NegativePlane::Clip(a, b, &POINT4D::x);
}

bool SutherlandHodgmanTriangleClipper4D::RightPlane::IsOutside(const POINT4D& pt) const {
    return PositivePlane::IsOutside(pt, &POINT4D::x);
}

POINT4D SutherlandHodgmanTriangleClipper4D::RightPlane::Clip(const POINT4D& a, const POINT4D& b) const {
    return PositivePlane::Clip(a, b, &POINT4D::x);
}

bool SutherlandHodgmanTriangleClipper4D::TopPlane::IsOutside(const POINT4D& pt) const {
    return PositivePlane::IsOutside(pt, &POINT4D::y);
}

POINT4D SutherlandHodgmanTriangleClipper4D::TopPlane::Clip(const POINT4D& a, const POINT4D& b) const {
    return PositivePlane::Clip(a, b, &POINT4D::y);
}

bool SutherlandHodgmanTriangleClipper4D::BottomPlane::IsOutside(const POINT4D& pt) const {
    return NegativePlane::IsOutside(pt, &POINT4D::y);
}

POINT4D SutherlandHodgmanTriangleClipper4D::BottomPlane::Clip(const POINT4D& a, const POINT4D& b) const {
    return NegativePlane::Clip(a, b, &POINT4D::y);
}

bool SutherlandHodgmanTriangleClipper4D::FarPlane::IsOutside(const POINT4D& pt) const {
    return PositivePlane::IsOutside(pt, &POINT4D::z);
}

POINT4D SutherlandHodgmanTriangleClipper4D::FarPlane::Clip(const POINT4D& a, const POINT4D& b) const {
    return PositivePlane::Clip(a, b, &POINT4D::z);
}

bool SutherlandHodgmanTriangleClipper4D::NearPlane::IsOutside(const POINT4D& pt) const {
    // directx convention 0 <= Z <= W
    // opengl convention -W <= Z <= W
    // these conventions used by projection matrices in those APIs
    return pt.z < 0.f;

    // will work for opengl convention
    //return NegativePlane::IsOutside(pt, &POINT4D::z);
}

POINT4D SutherlandHodgmanTriangleClipper4D::NearPlane::Clip(POINT4D a, POINT4D b) const {
    // directx convention 0 <= Z <= W
    // opengl convention -W <= Z <= W
    // these conventions used by projection matrices in those APIs

    // https://learn.microsoft.com/en-us/windows/win32/dxtecharts/the-direct3d-transformation-pipeline
    /* https://learn.microsoft.com/en-us/windows/win32/direct3d9/viewports-and-clipping#set-up-the-viewport-for-clipping
    * https://learn.microsoft.com/en-us/windows/win32/api/d3d9/nf-d3d9-idirect3ddevice9-setclipplane
    * Clipping volume for all points Xp = (Xp, Yp, Zp, Wp) in the projection space is defined as:
    -Wp < Xp <= Wp 
    -Wp < Yp <= Wp 
    0 < Zp <= Wp - 0 < Zp this part is important here
    */

    if (std::fabs(b.z) < std::fabs(a.z)) {
        std::swap(a, b);
    }

    float t = (0.f - a.z) / (b.z - a.z);
    auto res = SutherlandHodgmanTriangleClipper4D::Lerp(a, b, t);
    return res;

    // will work for opengl convention
    //return NegativePlane::Clip(a, b, &POINT4D::z);
}
