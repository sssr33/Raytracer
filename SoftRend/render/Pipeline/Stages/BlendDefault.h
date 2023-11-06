#pragma once
#include "..\Registers\SemanticRegisters.h"

// TODO add imple and change interface so it can blend 2 colors
struct BlendDefault {
    int operator()(float pixelFromPipeline, float pixelFromRenderTarget);
};
