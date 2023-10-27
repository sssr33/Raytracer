#include "SutherlandHodgmanTriangleClipper2D.h"

#include <cmath>
#include <algorithm>

std::vector<POINT2D> SutherlandHodgmanTriangleClipper2D::Clip(const Triangle2D& tri, const Screen2D& screen) {
    std::vector<POINT2D> current = { tri.a, tri.b, tri.c };
    std::vector<POINT2D> next;

    this->IntersectPlane(current, next, LeftPlane{}, screen);
    this->IntersectPlane(current, next, TopPlane{}, screen);
    this->IntersectPlane(current, next, RightPlane{}, screen);
    this->IntersectPlane(current, next, BottomPlane{}, screen);

    return current;
}

template<class PlaneT>
void SutherlandHodgmanTriangleClipper2D::IntersectPlane(
    std::vector<POINT2D>& current,
    std::vector<POINT2D>& next,
    const PlaneT& plane,
    const Screen2D& screen
) const
{
    if (!current.empty()) {
        auto a = current.back();

        for (const auto& b : current) {
            const bool aInside = !plane.IsOutside(a, screen);
            const bool bInside = !plane.IsOutside(b, screen);

            if (aInside != bInside) {
                // one inside, other outside, need to clip
                POINT2D intersection = plane.Clip(a, b, screen);

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
    }

    std::swap(current, next);
    next.clear();
}

POINT2D SutherlandHodgmanTriangleClipper2D::XPlane::ClipX(const POINT2D& aTmp, const POINT2D& bTmp, float planeX) const {
    auto ptrA = &aTmp;
    auto ptrB = &bTmp;

    if (std::fabs(bTmp.x) < std::fabs(aTmp.x)) {
        std::swap(ptrA, ptrB);
    }

    const auto& a = *ptrA;
    const auto& b = *ptrB;

    const POINT2D fromAtoB = b - a;
    const float fromAtoScreenX = planeX - a.x;

    const POINT2D intersection = {
        planeX,
        a.y + fromAtoB.y * fromAtoScreenX / fromAtoB.x
    };

    return intersection;
}

POINT2D SutherlandHodgmanTriangleClipper2D::YPlane::ClipY(const POINT2D& aTmp, const POINT2D& bTmp, float planeY) const {
    auto ptrA = &aTmp;
    auto ptrB = &bTmp;

    if (std::fabs(bTmp.y) < std::fabs(aTmp.y)) {
        std::swap(ptrA, ptrB);
    }

    const auto& a = *ptrA;
    const auto& b = *ptrB;

    const POINT2D fromAtoB = b - a;
    const float fromAtoScreenY = planeY - a.y;

    const POINT2D intersection = {
        a.x + fromAtoB.x * fromAtoScreenY / fromAtoB.y,
        planeY
    };

    return intersection;
}

bool SutherlandHodgmanTriangleClipper2D::LeftPlane::IsOutside(const POINT2D& pt, const Screen2D& screen) const {
    return pt.x < screen.min.x;
}

POINT2D SutherlandHodgmanTriangleClipper2D::LeftPlane::Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const {
    return this->ClipX(a, b, screen.min.x);
}

bool SutherlandHodgmanTriangleClipper2D::TopPlane::IsOutside(const POINT2D& pt, const Screen2D& screen) const {
    return pt.y < screen.min.y;
}

POINT2D SutherlandHodgmanTriangleClipper2D::TopPlane::Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const {
    return this->ClipY(a, b, screen.min.y);
}

bool SutherlandHodgmanTriangleClipper2D::RightPlane::IsOutside(const POINT2D& pt, const Screen2D& screen) const {
    return pt.x > screen.max.x;
}

POINT2D SutherlandHodgmanTriangleClipper2D::RightPlane::Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const {
    return this->ClipX(a, b, screen.max.x);
}

bool SutherlandHodgmanTriangleClipper2D::BottomPlane::IsOutside(const POINT2D& pt, const Screen2D& screen) const {
    return pt.y > screen.max.y;
}

POINT2D SutherlandHodgmanTriangleClipper2D::BottomPlane::Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const {
    return this->ClipY(a, b, screen.max.y);
}
