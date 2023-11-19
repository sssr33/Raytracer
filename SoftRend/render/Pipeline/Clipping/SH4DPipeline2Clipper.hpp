#pragma once
#include "SH4DPipeline2Clipper.h"

#include <cmath>

template<class VertexT>
std::vector<VertexT> SH4DPipeline2Clipper::Clip(const VertexT& a, const VertexT& b, const VertexT& c) {
    std::vector<ClipVertex> current = {
        SH4DPipeline2Clipper::MakeClipVertex(a, { 1.f, 0.f, 0.f }),
        SH4DPipeline2Clipper::MakeClipVertex(b, { 0.f, 1.f, 0.f }),
        SH4DPipeline2Clipper::MakeClipVertex(c, { 0.f, 0.f, 1.f }),
    };
    std::vector<ClipVertex> next;

    this->IntersectPlane(current, next, LeftPlane{});
    this->IntersectPlane(current, next, RightPlane{});
    this->IntersectPlane(current, next, TopPlane{});
    this->IntersectPlane(current, next, BottomPlane{});
    this->IntersectPlane(current, next, FarPlane{});
    this->IntersectPlane(current, next, NearPlane{});

    return {};
}

template<class PlaneT>
static void SH4DPipeline2Clipper::IntersectPlane(
    std::vector<ClipVertex>& current,
    std::vector<ClipVertex>& next,
    const PlaneT& plane
)
{
    if (!current.empty()) {
        auto a = current.back();

        for (const auto& b : current) {
            const bool aInside = !plane.IsOutside(a);
            const bool bInside = !plane.IsOutside(b);

            if (aInside != bInside) {
                // one inside, other outside, need to clip
                ClipVertex intersection = plane.Clip(a, b);

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

template<class VertexT>
SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::MakeClipVertex(const VertexT& v, const VertexInterpolationWeigths& weigths) {
    return { SemanticValueHelpers::GetValue<SVPosition>(v), weigths };
}
