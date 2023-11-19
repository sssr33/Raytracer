#include "SH4DPipeline2Clipper.h"

#include <cmath>

bool SH4DPipeline2Clipper::PositivePlane::IsOutside(const SVPosition& pt, float SVPosition::* axis) {
    return pt.*axis > pt.w;
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::PositivePlane::Clip(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis) {
    const auto& [a, b, svPosA, svPosB] = SH4DPipeline2Clipper::SortVertices(aTmp, bTmp, axis);
    float t = (svPosA.w - svPosA.*axis) / (svPosB.*axis - svPosA.*axis - svPosB.w + svPosA.w);
    auto res = SemanticValueHelpers::ApplyValues(a, b, SemanticValuesLerp(t));
    return res;
}

bool SH4DPipeline2Clipper::NegativePlane::IsOutside(const SVPosition& pt, float SVPosition::* axis) {
    return pt.*axis < -pt.w;
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::NegativePlane::Clip(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis) {
    const auto& [a, b, svPosA, svPosB] = SH4DPipeline2Clipper::SortVertices(aTmp, bTmp, axis);
    float t = (svPosA.w + svPosA.*axis) / (svPosA.w - svPosB.w + svPosA.*axis - svPosB.*axis);
    auto res = SemanticValueHelpers::ApplyValues(a, b, SemanticValuesLerp(t));
    return res;
}

bool SH4DPipeline2Clipper::LeftPlane::IsOutside(const ClipVertex& pt) const {
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return NegativePlane::IsOutside(svPos, &SVPosition::x);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::LeftPlane::Clip(const ClipVertex& a, const ClipVertex& b) const {
    return NegativePlane::Clip(a, b, &SVPosition::x);
}

bool SH4DPipeline2Clipper::RightPlane::IsOutside(const ClipVertex& pt) const {
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return PositivePlane::IsOutside(svPos, &SVPosition::x);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::RightPlane::Clip(const ClipVertex& a, const ClipVertex& b) const {
    return PositivePlane::Clip(a, b, &SVPosition::x);
}

bool SH4DPipeline2Clipper::TopPlane::IsOutside(const ClipVertex& pt) const {
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return PositivePlane::IsOutside(svPos, &SVPosition::y);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::TopPlane::Clip(const ClipVertex& a, const ClipVertex& b) const {
    return PositivePlane::Clip(a, b, &SVPosition::y);
}

bool SH4DPipeline2Clipper::BottomPlane::IsOutside(const ClipVertex& pt) const {
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return NegativePlane::IsOutside(svPos, &SVPosition::y);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::BottomPlane::Clip(const ClipVertex& a, const ClipVertex& b) const {
    return NegativePlane::Clip(a, b, &SVPosition::y);
}

bool SH4DPipeline2Clipper::FarPlane::IsOutside(const ClipVertex& pt) const {
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return PositivePlane::IsOutside(svPos, &SVPosition::z);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::FarPlane::Clip(const ClipVertex& a, const ClipVertex& b) const {
    return PositivePlane::Clip(a, b, &SVPosition::z);
}

bool SH4DPipeline2Clipper::NearPlane::IsOutside(const ClipVertex& pt) const {
    // directx convention 0 <= Z <= W
    // opengl convention -W <= Z <= W
    // these conventions used by projection matrices in those APIs
    auto& svPos = SemanticValueHelpers::GetValue<SVPosition>(pt);
    return svPos.z < 0.f;

    // will work for opengl convention
    //return NegativePlane::IsOutside(pt, &POINT4D::z);
}

SH4DPipeline2Clipper::ClipVertex SH4DPipeline2Clipper::NearPlane::Clip(const ClipVertex& aTmp, const ClipVertex& bTmp) const {
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

    const auto& [a, b, svPosA, svPosB] = SH4DPipeline2Clipper::SortVertices(aTmp, bTmp, &SVPosition::z);

    float t = (0.f - svPosA.z) / (svPosB.z - svPosA.z);
    auto res = SemanticValueHelpers::ApplyValues(a, b, SemanticValuesLerp(t));
    return res;

    // will work for opengl convention
    //return NegativePlane::Clip(a, b, &POINT4D::z);
}

SH4DPipeline2Clipper::SemanticValuesWeightedSum::SemanticValuesWeightedSum(const VertexInterpolationWeigths& weights)
    : weights(weights)
{}

float SH4DPipeline2Clipper::SemanticValuesWeightedSum::operator()(float a, float b, float c) const {
    float res;

    res = a * this->weights.x + b * this->weights.y + c * this->weights.z;

    return res;
}

Float2 SH4DPipeline2Clipper::SemanticValuesWeightedSum::operator()(const Float2& a, const Float2& b, const Float2& c) const {
    Float2 res;

    res.x = a.x * this->weights.x + b.x * this->weights.y + c.x * this->weights.z;
    res.y = a.y * this->weights.x + b.y * this->weights.y + c.y * this->weights.z;

    return res;
}

Float3 SH4DPipeline2Clipper::SemanticValuesWeightedSum::operator()(const Float3& a, const Float3& b, const Float3& c) const {
    Float3 res;

    res.x = a.x * this->weights.x + b.x * this->weights.y + c.x * this->weights.z;
    res.y = a.y * this->weights.x + b.y * this->weights.y + c.y * this->weights.z;
    res.z = a.z * this->weights.x + b.z * this->weights.y + c.z * this->weights.z;

    return res;
}

Float4 SH4DPipeline2Clipper::SemanticValuesWeightedSum::operator()(const Float4& a, const Float4& b, const Float4& c) const {
    Float4 res;

    res.x = a.x * this->weights.x + b.x * this->weights.y + c.x * this->weights.z;
    res.y = a.y * this->weights.x + b.y * this->weights.y + c.y * this->weights.z;
    res.z = a.z * this->weights.x + b.z * this->weights.y + c.z * this->weights.z;
    res.w = a.w * this->weights.x + b.w * this->weights.y + c.w * this->weights.z;

    return res;
}

SVPosition SH4DPipeline2Clipper::SemanticValuesWeightedSum::operator()(const SVPosition& a, const SVPosition& b, const SVPosition& c) const {
    return {}; // ignore SVPosition because SH4DPipeline2Clipper already knows it
}

SH4DPipeline2Clipper::SortedVertices SH4DPipeline2Clipper::SortVertices(const ClipVertex& aTmp, const ClipVertex& bTmp, float SVPosition::* axis) {
    auto ptrA = &aTmp;
    auto ptrB = &bTmp;

    auto& svPosA = SemanticValueHelpers::GetValue<SVPosition>(aTmp);
    auto& svPosB = SemanticValueHelpers::GetValue<SVPosition>(bTmp);

    if (std::fabs(svPosA.*axis) < std::fabs(svPosB.*axis)) {
        std::swap(ptrA, ptrB);
    }

    return { *ptrA, *ptrB, SemanticValueHelpers::GetValue<SVPosition>(*ptrA), SemanticValueHelpers::GetValue<SVPosition>(*ptrB) };
}
