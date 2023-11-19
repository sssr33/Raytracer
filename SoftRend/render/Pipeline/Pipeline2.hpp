#pragma once
#include "Pipeline2.h"
#include "Semantic\SemanticValuesLerp.h"

template<typename TraitsT>
void Pipeline2<TraitsT>::SetVertexShader(VSType vertexShader) {
    this->vertexShader = std::move(vertexShader);
}

template<typename TraitsT>
void Pipeline2<TraitsT>::SetPixelShader(PSType pixelShader) {
    this->pixelShader = std::move(pixelShader);
}

template<typename TraitsT>
void Pipeline2<TraitsT>::SetBlendState(BlendType blendState) {
    this->blendState = std::move(blendState);
}

template<typename TraitsT>
template<typename InputAssemblerT>
void Pipeline2<TraitsT>::Draw(const InputAssemblerT& inputAssembler) {
    this->VertexShaderStage(inputAssembler);
    this->ClipSpaceClipStage();
    this->ClipSpaceToNdcStage();
    this->RasterizeStage();
}

template<typename TraitsT>
template<typename InputAssemblerT>
void Pipeline2<TraitsT>::VertexShaderStage(const InputAssemblerT& inputAssembler) {
    const size_t triangleCount = inputAssembler.GetTriangleCount();

    this->vsOutTriangles.clear();
    this->vsOutTriangles.reserve(triangleCount);

    for (size_t i = 0; i < triangleCount; ++i) {
        this->vsOutTriangles.emplace_back();
        VSOutputTriangle& outTri = vsOutTriangles.back();

        for (size_t v = 0; v < std::size(outTri.v); ++v) {
            const VSInputType& inputVertex = inputAssembler.GetTriangleVertex(i, v);
            outTri.v[v] = this->vertexShader(inputVertex);
        }
    }
}

template<typename TraitsT>
void Pipeline2<TraitsT>::ClipSpaceClipStage() {
    this->vsOutClippedTriangles.clear();
    this->vsOutClippedTriangles.reserve(this->vsOutTriangles.size());

    for (const auto& triangle : this->vsOutTriangles) {
        auto clippedPoly = this->clipper.Clip(triangle.v[0], triangle.v[1], triangle.v[2]);

        // TODO triangulate clippedPoly and push results to this->vsOutClippedTriangles
    }
}

template<typename TraitsT>
void Pipeline2<TraitsT>::ClipSpaceToNdcStage() {

}

template<typename TraitsT>
void Pipeline2<TraitsT>::RasterizeStage() {

}
