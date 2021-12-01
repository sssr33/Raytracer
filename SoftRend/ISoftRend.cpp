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

        if (false)
        {
            this->graphics->DrawCircle(&center, 20, _ARGB32BIT(255, 255, 255, 255), true);

            this->graphics->DrawLine(0, 0, 100, 100, _ARGB32BIT(255, 255, 255, 255));

            this->graphics->DrawLine(0, 0, 100, 20, _ARGB32BIT(255, 0, 0, 255));
            this->graphics->DrawLine(0, 0, 20, 100, _ARGB32BIT(255, 0, 255, 0));
            this->graphics->DrawLine(0, 0, 80, 100, _ARGB32BIT(255, 255, 0, 0));
            this->graphics->DrawLine(0, 0, 100, 80, _ARGB32BIT(255, 255, 0, 0));
        }

        GeometryGen geomGen;
        std::unique_ptr<OBJECT4D> sphere = std::make_unique<OBJECT4D>();
        math3D::POINT4D spherePos = {};

        spherePos.z = 300.f;

        geomGen.generateSphere(32, 100.f, sphere.get(), &spherePos, _ARGB32BIT(255, 0, 255, 0));

        mat::RGBA ambient;
        mat::RGBA diffuse;
        mat::RGBA specular;

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

        /*if (!this->graphics->getLight(0))
        {
            this->graphics->AddLight(mat::LIGHT_STATE_ON, mat::LIGHT_ATTR_AMBIENT, ambient, diffuse, specular, 0, &lightDir, 0, 0, 0, 0, 0, 10);
        }*/

        POINT4D lightPos = spherePos;

        lightPos.x = 100.f;
        lightPos.y = 100.f;

        static float angle = -53.9799156f; //-53.9799156f;// 10.22f; //0.78f;

        MATRIX4X4 mrot;
        mrot.Build_XYZ_Rotation_MATRIX4X4(0.f, 0.f, angle);

        POINT4D lightPosNew = {};

        lightPos.Mat_Mul_VECTOR4D_4X4(&mrot, &lightPosNew);

        if (KEYDOWN(VK_UP))
        {
            //angle += 0.01f;
            angle += 0.3f;
        }
        if (KEYDOWN(VK_DOWN))
        {
            //angle -= 0.01f;
            angle -= 0.3f;
        }

        //angle += 0.1f;

        VECTOR4D lightDir;

        spherePos.VECTOR4D_Sub(&lightPosNew, &lightDir);

        vecNormalize(&lightDir);

        if (!this->graphics->getLight(0))
        {
            l_idx = this->graphics->AddLight(mat::LIGHT_STATE_ON, mat::LIGHT_ATTR_POINT, ambient, diffuse, specular, &lightPos, &lightDir, 1.f, 0, 0, 0, 0, 10.f);
        }

        LIGHT_PTR l = this->graphics->getLight(l_idx);

        l->pos = lightPosNew;
        l->dir = lightDir;

        //this->graphics->setRenderState(RendState::RS_LIGHTING, 1);

        std::unique_ptr<OBJECT4D> sphereShadow = std::make_unique<OBJECT4D>();

        geomGen.generateShadowVolume(l, 1, sphere.get(), sphereShadow.get(), 1);

        //this->graphics->DrawOBJECT4DSolid(sphere.get());

        sphereShadow->ModelToWorld();

        std::unique_ptr<RENDERLIST4D> renderList = std::make_unique<RENDERLIST4D>();

        //if (false)
        {
            renderList->Insert_OBJECT4D_RENDERLIST4D(sphereShadow.get());
        }

        std::unique_ptr<OBJECT4D> testTirangles = std::make_unique<OBJECT4D>();

        if(false)
        {
            int polyAttr = struct3D::POLY4D_ATTR_RGB24 | struct3D::POLY4D_ATTR_SHADE_MODE_GOURAUD | struct3D::POLY4D_ATTR_DISABLE_MATERIAL;
            int vertAttr = struct3D::VERTEX4DT_ATTR_NORMAL | struct3D::VERTEX4DT_ATTR_POINT;
            int polyState = struct3D::POLY4D_STATE_ACTIVE;// | struct3D::OBJECT4D_STATE_VISIBLE;

            int triangleCount = 3;
            float anglePerTri = 360.f / (float)(triangleCount);
            float angle = -90.f;
            float radius = 7.f;

            auto angleToRads = [](float angle)
            {
                float pi = 3.14159265358979323846f;
                float res = angle * (pi / 180.f);
                return res;
            };

            auto triArea = [](struct3D::POLYF4D poly)
            {
                auto& tvlist = poly.tvlist;

                float x1 = tvlist[0].x;
                float x2 = tvlist[1].x;
                float x3 = tvlist[2].x;

                float y1 = tvlist[0].y;
                float y2 = tvlist[1].y;
                float y3 = tvlist[2].y;

                float area = 0.5f * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
                return std::abs(area);
            };

            angle += anglePerTri;

            float prevX = std::cos(angleToRads(angle)) * radius;
            float prevY = std::sin(angleToRads(angle)) * radius;

            float firstX = prevX;
            float firstY = prevY;

            for (int i = 0; i < triangleCount - 1; i++)
            {
                angle += anglePerTri;

                float x = std::cos(angleToRads(angle)) * radius;
                float y = std::sin(angleToRads(angle)) * radius;

                auto& poly = renderList->poly_data[i];
                auto& tvlist = poly.tvlist;

                tvlist[0].x = 0.f;
                tvlist[0].y = 0.f;

                tvlist[1].x = x;
                tvlist[1].y = y;

                tvlist[2].x = prevX;
                tvlist[2].y = prevY;

                prevX = x;
                prevY = y;
            }

            auto& poly = renderList->poly_data[triangleCount - 1];
            auto& tvlist = poly.tvlist;

            tvlist[0].x = 0.f;
            tvlist[0].y = 0.f;

            tvlist[1].x = prevX;
            tvlist[1].y = prevY;

            tvlist[2].x = firstX;
            tvlist[2].y = firstY;

            float offset = 99.5f;
            float offsetX = 0.f;
            float offsetY = 0.0f;

            std::vector<float> areas;

            for (int i = 0; i < triangleCount; i++)
            {
                auto& poly = renderList->poly_data[i];
                auto& tvlist = poly.tvlist;

                for (auto& pt : tvlist)
                {
                    pt.x += offset + offsetX;
                    pt.y += offset + offsetY;
                }

                if (i % 3 == 0)
                {
                    poly.color = ARGB32BIT(127, 0, 255, 0);
                }
                else if (i % 2 == 0)
                {
                    poly.color = ARGB32BIT(127, 0, 0, 255);
                }
                else if (i % 1 == 0)
                {
                    poly.color = ARGB32BIT(127, 0, 0, 0);
                }

                areas.push_back(triArea(poly));
            }

            int stop = 234;

            // quad test
            //int triangleCount = 2;
            
            //auto& poly = renderList->poly_data[0];
            //auto& poly2 = renderList->poly_data[1];

            //auto& tvlist = poly.tvlist;
            //auto& tvlist2 = poly2.tvlist;

            //float offset = 0.f;// 0.500001f;

            //float left = 1.0f + offset;
            //float top = 1.0f + offset;
            //float right = 600.0f + offset;
            //float bottom = 200.0f + offset;

            //// poly1
            //tvlist[0].x = left;
            //tvlist[0].y = top;

            //tvlist[1].x = right;
            //tvlist[1].y = top;

            //tvlist[2].x = right;
            //tvlist[2].y = bottom;

            //// poly2
            //tvlist2[0].x = left;
            //tvlist2[0].y = bottom;

            //tvlist2[1].x = left;
            //tvlist2[1].y = top;

            //tvlist2[2].x = right;
            //tvlist2[2].y = bottom;

            //poly.color = ARGB32BIT(127, 0, 0, 0);
            //poly2.color = ARGB32BIT(127, 0, 255, 0);

            renderList->num_polys = triangleCount;

            for (int i = 0; i < renderList->num_polys; i++)
            {
                renderList->poly_data[i].state = polyState;
                renderList->poly_data[i].attr = polyAttr;
                renderList->poly_ptrs[i] = &renderList->poly_data[i];
            }
        }

        this->graphics->DrawRENDERLIST4DSolid(renderList.get(), nullptr);

        //this->graphics->DrawOBJECT4DSolid(sphereShadow.get());
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
            screenParams.graphicQuality = 1;
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
