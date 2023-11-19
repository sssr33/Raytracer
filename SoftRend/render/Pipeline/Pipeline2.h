#pragma once
#include "Pipeline2Traits.h"
#include "Semantic\SemanticValues.h"
#include "Semantic\SemanticValueHelpers.h"
#include "Clipping\SH4DPipeline2Clipper.h"

#include <vector>
#include <type_traits>
#include <Helpers\Meta\ArgType.h>

template<typename TraitsT = Pipeline2Traits<>>
class Pipeline2 {
    using VSType = typename TraitsT::VertexShaderType;
    using PSType = typename TraitsT::PixelShaderType;
    using BlendType = typename TraitsT::BlendType;

    using VSOperatorT = decltype(&VSType::operator());
    using PSOperatorT = decltype(&PSType::operator());
    using BlendOperatorT = decltype(&BlendType::operator());

public:
    using VSInputType = std::decay_t<typename ArgType<VSOperatorT>::template n_type<0>>;
    using VSOutputType = std::decay_t<typename ArgType<VSOperatorT>::return_type>;

    using PSInputType = std::decay_t<typename ArgType<PSOperatorT>::template n_type<0>>;
    using PSOutputType = std::decay_t<typename ArgType<PSOperatorT>::return_type>;

    void SetVertexShader(VSType vertexShader);
    void SetPixelShader(PSType pixelShader);
    void SetBlendState(BlendType blendState);

    /*
    * InputAssemblerT must get triangle count and read triangle vertices as VSInputType
    */
    template<typename InputAssemblerT>
    void Draw(const InputAssemblerT& inputAssembler);

private:
    struct VSOutputTriangle {
        VSOutputType v[3];
    };

    template<typename InputAssemblerT>
    void VertexShaderStage(const InputAssemblerT& inputAssembler);
    void ClipSpaceClipStage();
    void ClipSpaceToNdcStage();
    void RasterizeStage();

    VSType vertexShader;
    PSType pixelShader;
    BlendType blendState;

    SH4DPipeline2Clipper clipper;

    std::vector<VSOutputTriangle> vsOutTriangles;
    std::vector<VSOutputTriangle> vsOutClippedTriangles;
    std::vector<VSOutputTriangle> vsOutClippedNdcTriangles;
};

#include "Pipeline2.hpp"
