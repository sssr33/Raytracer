#pragma once
#include "SH4DPipeline2Clipper.h"

#include <cassert>
#include <cmath>
#include <algorithm>

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

    if (current.size() > 3) {
        // intersection can produce duplicate vertices
        current.erase(std::unique(std::begin(current), std::end(current)), std::end(current));

        // std::unique doesn't check first and last but for polygon it is connected vertices
        if (current.front() == current.back()) {
            current.pop_back();
        }

        std::vector<VertexT> result;

        result.reserve(current.size());

        for (const auto& v : current) {
            const auto& weights = SemanticValueHelpers::GetValue<VertexInterpolationWeigths>(v);

            // if some weight == 1 then original vertex can be used without interpolation
            if (weights.x == 1.f) {
                assert(weights.y == 0.f, weights.z == 0.f);
                result.push_back(a);
            }
            else if (weights.y == 1.f) {
                assert(weights.x == 0.f, weights.z == 0.f);
                result.push_back(b);
            }
            else if (weights.z == 1.f) {
                assert(weights.x == 0.f, weights.y == 0.f);
                result.push_back(c);
            }
            else {
                // do weighting of parameters
                // TODO try to optimize more when some weight == 0, then only 2 or even 1 vertex can be weighted
                auto resVert = SemanticValueHelpers::ApplyValues(a, b, c, SemanticValuesWeightedSum(weights));
                SemanticValueHelpers::SetValue<SVPosition>(resVert, SemanticValueHelpers::GetValue<SVPosition>(v));
                result.push_back(resVert);
            }
        }

        return result;
    }
    else if (current.size() == 3) {
        // Count not changes, nothing is clipped, return same triangles
        // TODO optimize this case so the caller can handle it in an optimal way(just copy triangle it already has)
        return { a, b, c };
    }

    // Less than 3 vertices, algorithm error
    // If happens need to check how it clips
    assert(false);
    return {};
}

template<class PlaneT>
static void SH4DPipeline2Clipper::IntersectPlane(
    std::vector<ClipVertex>& current,
    std::vector<ClipVertex>& next,
    const PlaneT& plane
)
{
    // TODO try to start from .front() to save original a,b,c order if no clipping is made
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
