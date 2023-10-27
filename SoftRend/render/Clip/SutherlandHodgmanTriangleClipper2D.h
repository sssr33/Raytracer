#pragma once
#include "..\structures.h"

#include <vector>

class SutherlandHodgmanTriangleClipper2D {
public:
    std::vector<POINT2D> Clip(const Triangle2D& tri, const Screen2D& screen);

private:
    struct XPlane {
        POINT2D ClipX(const POINT2D& aTmp, const POINT2D& bTmp, float planeX) const;
    };

    struct YPlane {
        POINT2D ClipY(const POINT2D& aTmp, const POINT2D& bTmp, float planeY) const;
    };

    struct LeftPlane : private XPlane {
        bool IsOutside(const POINT2D& pt, const Screen2D& screen) const;
        POINT2D Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const;
    };

    struct TopPlane : private YPlane {
        bool IsOutside(const POINT2D& pt, const Screen2D& screen) const;
        POINT2D Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const;
    };

    struct RightPlane : private XPlane {
        bool IsOutside(const POINT2D& pt, const Screen2D& screen) const;
        POINT2D Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const;
    };

    struct BottomPlane : private YPlane {
        bool IsOutside(const POINT2D& pt, const Screen2D& screen) const;
        POINT2D Clip(const POINT2D& a, const POINT2D& b, const Screen2D& screen) const;
    };

    template<class PlaneT>
    void IntersectPlane(
        std::vector<POINT2D>& current,
        std::vector<POINT2D>& next,
        const PlaneT& plane,
        const Screen2D& screen
    ) const;
};
