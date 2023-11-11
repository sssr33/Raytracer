#pragma once
#include "..\Semantic\SemanticValues.h"

using PSInputDefault = std::tuple<SVPosition>;
using PSOutputDefault = std::tuple<SVColor<0>>;

struct PixelShaderDefault {
    PSOutputDefault operator()(const PSInputDefault& pixelInput);
};
