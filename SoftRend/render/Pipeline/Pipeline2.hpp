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
const Pipeline2Viewport& Pipeline2<TraitsT>::GetViewport() const {
    return this->viewport;
}

template<typename TraitsT>
void Pipeline2<TraitsT>::SetViewport(const Pipeline2Viewport& viewport) {
    this->viewport = viewport;
}

template<typename TraitsT>
template<typename InputAssemblerT>
void Pipeline2<TraitsT>::Draw(const InputAssemblerT& inputAssembler) {
    this->VertexShaderStage(inputAssembler);
    this->ClipSpaceClipStage();
    this->ClipSpaceToScreenStage();
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

    for (const VSOutputTriangle& triangle : this->vsOutTriangles) {
        // TODO add logic for clipper to detect triangles inside
        std::vector<VSOutputType> clippedPoly = this->clipper.Clip(triangle.v[0], triangle.v[1], triangle.v[2]);

        if (clippedPoly.size() > 2) {
            // triangulate event if 3 vertices because they may be clipped too
            for (size_t vertex = 2; vertex < clippedPoly.size(); ++vertex) {
                VSOutputTriangle tri;

                tri.v[0] = clippedPoly[0];
                tri.v[1] = clippedPoly[vertex - 1];
                tri.v[2] = clippedPoly[vertex - 0];

                this->vsOutClippedTriangles.push_back(std::move(tri));
            }
        }
        else if (clippedPoly.size() == 0) {
            // fully outside, do nothing
            continue;
        }
        else {
            // clipper error
            assert(false);
        }
    }
}

template<typename TraitsT>
void Pipeline2<TraitsT>::ClipSpaceToScreenStage() {
    this->vsOutScreenTriangles.clear();
    this->vsOutScreenTriangles.reserve(this->vsOutClippedTriangles.size());

    const float halfViewportWidth = this->viewport.width * 0.5f;
    const float halfViewportHeight = this->viewport.height * 0.5f;

    const Float4 scale = Float4(halfViewportWidth, -halfViewportHeight, this->viewport.maxZ - this->viewport.minZ, 0.f);
    const Float4 offset = Float4(this->viewport.x + halfViewportWidth, this->viewport.y + halfViewportHeight, this->viewport.minZ, 0.f);

    std::array<SVPosition, 3> svPosNdc;

    for (const VSOutputTriangle& triangle : this->vsOutClippedTriangles) {
        for (size_t v = 0; v < std::size(svPosNdc); ++v) {
            svPosNdc[v] = SemanticValueHelpers::GetValue<SVPosition>(triangle.v[v]);

            svPosNdc[v].x /= svPosNdc[v].w;
            svPosNdc[v].y /= svPosNdc[v].w;
            svPosNdc[v].z /= svPosNdc[v].w;
        }

        if (this->IsBackface(svPosNdc)) {
            continue;
        }

        VSOutputTriangle screenTri = triangle;

        for (size_t v = 0; v < std::size(svPosNdc); ++v) {
            SVPosition screenPos = svPosNdc[v] * scale + offset;
            SemanticValueHelpers::SetValue<SVPosition>(screenTri.v[v], screenPos);
        }

        this->vsOutScreenTriangles.push_back(screenTri);
    }
}

template<typename TraitsT>
void Pipeline2<TraitsT>::RasterizeStage() {
    for (const VSOutputTriangle& triangle : this->vsOutScreenTriangles) {
    }
}

template<typename TraitsT>
bool Pipeline2<TraitsT>::IsBackface(const std::array<SVPosition, 3>& svPosNdc) const {
    auto a = static_cast<Float2>(svPosNdc[0]);
    auto b = static_cast<Float2>(svPosNdc[1]);
    auto c = static_cast<Float2>(svPosNdc[2]);

    auto ab = b - a;
    auto ac = c - a;

    float cross = ab.Cross(ac);

    bool backface = cross > 0.f;
    return backface;
}
