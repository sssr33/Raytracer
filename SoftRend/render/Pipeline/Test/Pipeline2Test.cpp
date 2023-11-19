#include "Pipeline2Test.h"
#include "..\Pipeline2.h"

#include <DirectXMath.h>

using VSInputTest = std::tuple<DirectX::XMVECTOR, Float2, Float3, float>;
using VSOutputTest = std::tuple<SVPosition, Float2, Float3, float>;

struct VertexShaderTest {
    VSOutputTest operator()(const VSInputTest& vertexInput) {
        auto& [pos, tex, color, brihtness] = vertexInput;
        auto transformedPos = DirectX::XMVector3Transform(pos, this->mvp);

        SVPosition svPos;

        svPos.x = DirectX::XMVectorGetX(transformedPos);
        svPos.y = DirectX::XMVectorGetY(transformedPos);
        svPos.z = DirectX::XMVectorGetZ(transformedPos);
        svPos.w = DirectX::XMVectorGetW(transformedPos);

        return { svPos, tex, color, brihtness };
    }

    DirectX::XMMATRIX mvp;
};

struct InputAssemblerTest {
    size_t GetTriangleCount() const {
        return 1;
    }

    VSInputTest GetTriangleVertex(size_t triIdx, size_t vertexIdx) const {
        if (triIdx > 0) {
            assert(false);
            return {};
        }
        if (vertexIdx > 3) {
            assert(false);
            return {};
        }

        if (vertexIdx == 0) {
            return {
                DirectX::XMVectorSet(-1.f, 1.f, 1.f, 1.f),
                Float2(0.f, 0.f),
                Float3(0.f, 0.f, 0.f),
                0.f
            };
        }
        else if (vertexIdx == 1) {
            return {
                DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f),
                Float2(1.f, 0.f),
                Float3(0.f, 1.f, 0.f),
                1.f
            };
        }
        else if (vertexIdx == 2) {
            return {
                DirectX::XMVectorSet(-1.f, -1.f, 1.f, 1.f),
                Float2(0.f, 1.f),
                Float3(0.f, 0.f, 1.f),
                1.f
            };
        }

        return {};
    }
};

struct Pipeline2TraitsTest {
    using VertexShaderType = VertexShaderTest;
    using PixelShaderType = PixelShaderDefault;
    using BlendType = BlendDefault;
};

void Pipeline2Test() {
    using TestPipeline = Pipeline2<Pipeline2TraitsTest>;

    VertexShaderTest vsTest;

    vsTest.mvp = DirectX::XMMatrixOrthographicLH(2.f, 2.f, 0.1f, 10.f);

    TestPipeline pipeline;

    pipeline.SetVertexShader(std::move(vsTest));

    pipeline.Draw(InputAssemblerTest{});
}
