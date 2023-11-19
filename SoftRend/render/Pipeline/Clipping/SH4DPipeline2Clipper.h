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

    struct SemanticValuesWeightedSum {
    public:
        SemanticValuesWeightedSum() = default;
        SemanticValuesWeightedSum(const VertexInterpolationWeigths& weights);

        float operator()(float a, float b, float c) const;
        Float2 operator()(const Float2& a, const Float2& b, const Float2& c) const;
        Float3 operator()(const Float3& a, const Float3& b, const Float3& c) const;
        Float4 operator()(const Float4& a, const Float4& b, const Float4& c) const;
        SVPosition operator()(const SVPosition& a, const SVPosition& b, const SVPosition& c) const;

    private:
        const VertexInterpolationWeigths weights;
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
