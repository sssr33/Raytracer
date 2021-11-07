#include "ISoftRend.h"
#include "render/GraphicImpInMemory.h"
#include "render/GeometryGen.h"

class SoftRend : public ISoftRend
{
public:
    void Render(uint32_t width, uint32_t height, void* dstMemory) override
    {
        this->CheckGrapics(width, height, dstMemory);

        this->graphics->DrawBegin(true);
        this->DrawScene();
        this->graphics->DrawEnd();
    }

private:
    void DrawScene()
    {
        math3D::POINT2D center;

        center.x = 100.f;
        center.y = 100.f;

        this->graphics->DrawCircle(&center, 20, _ARGB32BIT(255, 255, 255, 255), true);

        this->graphics->DrawLine(0, 0, 100, 100, _ARGB32BIT(255, 255, 255, 255));

        this->graphics->DrawLine(0, 0, 100, 20, _ARGB32BIT(255, 0, 0, 255));
        this->graphics->DrawLine(0, 0, 20, 100, _ARGB32BIT(255, 0, 255, 0));
        this->graphics->DrawLine(0, 0, 80, 100, _ARGB32BIT(255, 255, 0, 0));
        this->graphics->DrawLine(0, 0, 100, 80, _ARGB32BIT(255, 255, 0, 0));

        GeometryGen geomGen;
        std::unique_ptr<OBJECT4D> sphere = std::make_unique<OBJECT4D>();
        math3D::POINT4D spherePos = {};

        spherePos.z = 10.f;
        spherePos.y = 3.f;
        spherePos.w = 1.f;

        geomGen.generateSphere(32, 1.f, sphere.get(), &spherePos, _ARGB32BIT(255, 255, 255, 255));

        this->graphics->DrawOBJECT4DSolid(sphere.get());
    }

    void CheckGrapics(uint32_t width, uint32_t height, void* dstMemory)
    {
        DstFlipParams dstFliParams;

        dstFliParams.dstMemory = dstMemory;
        dstFliParams.dstMemoryByteWidth = width * 4;

        if (!this->graphics)
        {
            ScreenParamsInMemory screenParams;

            screenParams.bitsPerPixel = 32;
            screenParams.graphicQuality = 0;
            screenParams.width = width;
            screenParams.height = height;

            this->graphics = std::make_unique<GraphicImpInMemory>();
            this->graphics->Initialize(screenParams, dstFliParams);
        }
        else
        {
            this->graphics->SetDstFlipParams(dstFliParams);
            this->graphics->Resize(static_cast<int>(width), static_cast<int>(height));
        }
    }

    std::unique_ptr<GraphicImpInMemory> graphics;
};

std::unique_ptr<ISoftRend> CreateISoftRend()
{
    return std::make_unique<SoftRend>();
}
