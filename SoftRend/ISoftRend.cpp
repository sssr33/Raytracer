// 🐚
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

        spherePos.z = 300.f;

        geomGen.generateSphere(32, 100.f, sphere.get(), &spherePos, _ARGB32BIT(255, 0, 255, 0));

        mat::RGBA ambient;
        mat::RGBA diffuse;
        mat::RGBA specular;
        VECTOR4D lightDir, lightPos;

        lightDir.VECTOR4D_ZERO();
        /*lightDir.z = 1.0f;
        lightDir.y = 1;*/
        lightDir.x = 1;

        vecNormalize(&lightDir);

        diffuse.a = 255;
        diffuse.r = 255;
        diffuse.g = 255;
        diffuse.b = 255;

        specular.a = 255;
        specular.r = 255;
        specular.g = 255;
        specular.b = 255;

        ambient.a = 255;
        ambient.r = 50;
        ambient.g = 50;
        ambient.b = 50;

        int l_idx = 0;

        if (!this->graphics->getLight(0))
        {
            this->graphics->AddLight(mat::LIGHT_STATE_ON, mat::LIGHT_ATTR_AMBIENT, ambient, diffuse, specular, 0, &lightDir, 0, 0, 0, 0, 0, 10);
        }

        if (!this->graphics->getLight(1))
        {
            l_idx = this->graphics->AddLight(mat::LIGHT_STATE_ON, mat::LIGHT_ATTR_INFINITE, ambient, diffuse, specular, 0, &lightDir, 0, 0, 0, 0, 0, 10);
        }

        this->graphics->setRenderState(RendState::RS_LIGHTING, 1);

        LIGHT_PTR l = this->graphics->getLight(1);

        std::unique_ptr<OBJECT4D> sphereShadow = std::make_unique<OBJECT4D>();

        geomGen.generateShadowVolume(l, 1, sphere.get(), sphereShadow.get(), 1);

        this->graphics->DrawOBJECT4DSolid(sphere.get());

        this->graphics->DrawOBJECT4DSolid(sphereShadow.get());
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
