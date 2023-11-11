#pragma once
#include "..\..\Math\Float.h"

#include <cstdint>
#include <tuple>

// SV stands for System Value
// SV's can be used by Pipeline2

struct SVPosition : public Float4 {
    using Float4::Float4;
};

template<size_t SemanticIdx>
struct SVColor : public Float4 {
    using Float4::Float4;
};

template<size_t SemanticIdx>
struct TexCoord : public Float2 {
    using Float2::Float2;
};
