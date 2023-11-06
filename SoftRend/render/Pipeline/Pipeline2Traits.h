#pragma once
#include "Stages\VertexShaderDefault.h"
#include "Stages\PixelShaderDefault.h"
#include "Stages\BlendDefault.h"

template<
    typename VertexShaderT = VertexShaderDefault,
    typename PixelShaderT = PixelShaderDefault,
    typename BlendT = BlendDefault
>
struct Pipeline2Traits {
    using VertexShaderType = VertexShaderT;
    using PixelShaderType = PixelShaderT;
    using BlendType = BlendT;
};
