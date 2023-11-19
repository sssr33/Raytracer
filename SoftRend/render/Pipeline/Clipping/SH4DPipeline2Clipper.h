#pragma once
#include "..\Semantic\SemanticValues.h"
#include "..\Semantic\SemanticValueHelpers.h"
#include "..\Semantic\SemanticValuesLerp.h"

#include <vector>

// SutherlandHodgmanTriangleClipper4D
class SH4DPipeline2Clipper {
public:
    template<class VertexT>
    std::vector<VertexT> Clip(const VertexT& a, const VertexT& b, const VertexT& c);

private:
    using VertexInterpolationWeigths = Float3;
    using ClipVertex = std::tuple<SVPosition, VertexInterpolationWeigths>;

    struct SortedVertices {
        const ClipVertex& a;
        const ClipVertex& b;
        const SVPosition& svPosA;
        const SVPosition& svPosB;
    };

    struct PositivePlane {
        static bool IsOutside(const SVPosition& pt, float SVPosition::*axis);
        static ClipVertex Clip(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis);
    };

    struct NegativePlane {
        static bool IsOutside(const SVPosition& pt, float SVPosition::* axis);
        static ClipVertex Clip(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis);
    };

    struct LeftPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& a, const ClipVertex& b) const;
    };

    struct RightPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& a, const ClipVertex& b) const;
    };

    struct TopPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& a, const ClipVertex& b) const;
    };

    struct BottomPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& a, const ClipVertex& b) const;
    };

    struct FarPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& a, const ClipVertex& b) const;
    };

    struct NearPlane {
        bool IsOutside(const ClipVertex& pt) const;
        ClipVertex Clip(const ClipVertex& aTmp, const ClipVertex& bTmp) const;
    };

    static SortedVertices SortVertices(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis);

    template<class PlaneT>
    static void IntersectPlane(
        std::vector<ClipVertex>& current,
        std::vector<ClipVertex>& next,
        const PlaneT& plane
    );

    template<class VertexT>
    static ClipVertex MakeClipVertex(const VertexT& v, const VertexInterpolationWeigths& weigths);
};

#include "SH4DPipeline2Clipper.hpp"
