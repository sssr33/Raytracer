#pragma once
#include "..\..\Math\Float.h"

class SemanticValuesLerp {
public:
    SemanticValuesLerp() = default;
    SemanticValuesLerp(float t);

    float operator()(float a, float b) const;
    Float2 operator()(const Float2& a, const Float2& b) const;
    Float3 operator()(const Float3& a, const Float3& b) const;
    Float4 operator()(const Float4& a, const Float4& b) const;

private:
    const float t = 0.0f;
};
