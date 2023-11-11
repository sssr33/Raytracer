#pragma once
#include "..\Semantic\SemanticValues.h"

using VSInputDefault = std::tuple<Float4>;
using VSOutputDefault = std::tuple<SVPosition>;

struct VertexShaderDefault {
    VSOutputDefault operator()(const VSInputDefault& vertexInput);
};
