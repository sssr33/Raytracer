#pragma once
#include "..\Registers\SemanticRegisters.h"

using PSInputDefault = std::tuple<SVPosition>;
using PSOutputDefault = std::tuple<SVColor<0>>;

struct PixelShaderDefault {
    PSOutputDefault operator()(const PSInputDefault& pixelInput);
};
