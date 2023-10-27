#pragma once
#include "..\structures.h"

#include <vector>

class SutherlandHodgmanTriangleClipper4D {
public:
    std::vector<POINT4D> Clip(const POINT4D& a, const POINT4D& b, const POINT4D& c);

private:
    struct PositivePlane {
        static bool IsOutside(const POINT4D& pt, float POINT4D::*axis);
        static POINT4D Clip(const POINT4D& aTmp, const POINT4D& bTmp, float POINT4D::* axis);
    };

    struct NegativePlane {
        static bool IsOutside(const POINT4D& pt, float POINT4D::* axis);
        static POINT4D Clip(const POINT4D& aTmp, const POINT4D& bTmp, float POINT4D::* axis);
    };

    struct LeftPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& a, const POINT4D& b) const;
    };

    struct RightPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& a, const POINT4D& b) const;
    };

    struct TopPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& a, const POINT4D& b) const;
    };

    struct BottomPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& a, const POINT4D& b) const;
    };

    struct FarPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& a, const POINT4D& b) const;
    };

    struct NearPlane {
        bool IsOutside(const POINT4D& pt) const;
        POINT4D Clip(const POINT4D& aTmp, const POINT4D& bTmp) const;
    };

    template<class PlaneT>
    void IntersectPlane(
        std::vector<POINT4D>& current,
        std::vector<POINT4D>& next,
        const PlaneT& plane
    ) const;

    static POINT4D Lerp(const POINT4D& a, const POINT4D& b, float t);
};
