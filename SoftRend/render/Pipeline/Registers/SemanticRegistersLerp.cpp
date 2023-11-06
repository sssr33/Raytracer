#include "SemanticRegistersLerp.h"

SemanticRegistersLerp::SemanticRegistersLerp(float t)
    : t(t)
{}

float SemanticRegistersLerp::operator()(float a, float b) const {
    float res = a + t * (b - a);
    return res;
}

Float2 SemanticRegistersLerp::operator()(const Float2& a, const Float2& b) const {
    Float2 res;

    res.x = (*this)(a.x, b.x);
    res.y = (*this)(a.y, b.y);

    return res;
}

Float4 SemanticRegistersLerp::operator()(const Float4& a, const Float4& b) const {
    Float4 res;

    res.x = (*this)(a.x, b.x);
    res.y = (*this)(a.y, b.y);
    res.z = (*this)(a.z, b.z);
    res.w = (*this)(a.w, b.w);

    return res;
}
