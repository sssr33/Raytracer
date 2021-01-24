// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved


#include "Direct2DHelloWorld.h"

#include <functional>
#include <DirectXMath.h>
#include <cstdlib>
#include <wrl.h>
#include <vector>
#include <random>

//
// Provides the entry point to the application.
//
int WINAPI WinMain(
    HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPSTR /*lpCmdLine*/,
    int /*nCmdShow*/
    )
{
    // Ignore the return value because we want to run the program even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}


//
// Initialize members.
//
DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pTextFormat(NULL),
    m_pBlackBrush(NULL)
{
}

//
// Release resources.
//
DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pBlackBrush);

}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        //m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        dpiX = dpiY = static_cast<float>(GetDpiForSystem());

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo Application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);

            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}


//
// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D and
// DirectWrite factories,  and a DirectWrite Text Format object
// (used for identifying particular font characteristics).
//
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;
    HRESULT hr;
    ID2D1GeometrySink *pSink = NULL;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
            );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    }

    SafeRelease(&pSink);

    return hr;
}

//
//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
//
HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {

        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );
        if (SUCCEEDED(hr))
        {
            // Create a black brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
                );
        }

    }

    return hr;
}


//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
}

//
// The main window message loop.
//
void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

std::vector<D2D1_POINT_2F> vectors;
std::vector<D2D1_POINT_2F> points;

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<float> dis(0.0f, 1.0f);
//std::exponential_distribution<float> dis(1.0f);
//std::normal_distribution<float> dis;

float RndFloat()
{
    /*float rnd = static_cast<float>(rand() % 10000) / 10000.f;
    return rnd;*/

    return dis(gen);
}

float easeOutCubic(float x)
{
    return 1.f - std::pow(1.f - x, 3.f);
}

float easeOutQuint(float x)
{
    return 1.f - std::pow(1.f - x, 5.f);
}

D2D1_POINT_2F RandOnDisk()
{
    DirectX::XMVECTOR vec;
    float dp;

    do
    {
        vec = DirectX::XMVectorSet(RndFloat(), RndFloat(), 0.f, 0.f);

        vec = DirectX::XMVectorScale(vec, 2.f);
        vec = DirectX::XMVectorSubtract(vec, DirectX::XMVectorSet(1.f, 1.f, 0.f, 0.f));
        dp = DirectX::XMVectorGetX(DirectX::XMVector2Dot(vec, vec));

    } while (dp >= 1.f);

    return { DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec) };
}

D2D1_POINT_2F RandOnDiskNoIter()
{
    DirectX::XMVECTOR vec = DirectX::XMVectorSet(RndFloat(), RndFloat(), 0.f, 0.f);
    vec = DirectX::XMVectorScale(vec, 2.f);
    vec = DirectX::XMVectorSubtract(vec, DirectX::XMVectorSet(1.f, 1.f, 0.f, 0.f));

    vec = DirectX::XMVectorScale(DirectX::XMVector2Normalize(vec), easeOutQuint(RndFloat()));

    /*DirectX::XMVECTOR vecNorm = DirectX::XMVector2Normalize(vec);

    vec = vecNorm;
    vec = DirectX::XMVectorScale(vec, RndFloat());
    vec = DirectX::XMVectorScale(vec, 2.f);
    vec = DirectX::XMVectorSubtract(vec, vecNorm);*/

    return { DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec) };
}

D2D1_POINT_2F RandOnDiskNoIter2()
{
    DirectX::XMVECTOR vec = DirectX::XMVectorSet(dis(gen), dis(gen), 0.f, 0.f);
    vec = DirectX::XMVectorScale(vec, 2.f);
    vec = DirectX::XMVectorSubtract(vec, DirectX::XMVectorSet(1.f, 1.f, 0.f, 0.f));

    vec = DirectX::XMVectorScale(DirectX::XMVector2Normalize(vec), dis(gen));

    /*DirectX::XMVECTOR vecNorm = DirectX::XMVector2Normalize(vec);

    vec = vecNorm;
    vec = DirectX::XMVectorScale(vec, RndFloat());
    vec = DirectX::XMVectorScale(vec, 2.f);
    vec = DirectX::XMVectorSubtract(vec, vecNorm);*/

    return { DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec) };
}

D2D1_POINT_2F RandOnDiskPolar()
{
    //float theta = RndFloat() * DirectX::XM_2PI;// random(0, TWO_PI);
    //float phi = RndFloat() * DirectX::XM_PI;// random(0, PI);
    //float r = RndFloat();// random(0, 1);
    //float x = r * sin(phi) * cos(theta);
    //float y = r * sin(phi) * sin(theta);
    //float z = r * cos(phi);

    //return { x, y };

    float theta = RndFloat() * DirectX::XM_2PI;// random(0, TWO_PI);
    float v = RndFloat();// random(0, 1);
    float phi = acos((2 * v) - 1);
    float r = pow(RndFloat(), 1.f / 3.f);
    float x = r * sin(phi) * cos(theta);
    float y = r * sin(phi) * sin(theta);
    float z = r * cos(phi);

    return { x, y };
}

D2D1_POINT_2F RandOnDiskCircular()
{
    float theta = RndFloat() * DirectX::XM_2PI;
    float r = pow(RndFloat(), 1.f / 2.f);
    //float r = sqrt(RndFloat());
    float x = r * cos(theta);
    float y = r * sin(theta);

    return { x, y };
}

D2D1_POINT_2F RandOnDiskCustom()
{
    float U = pow(RndFloat(), 1 / 3);
    float x = RndFloat() * 2.f - 1.f;
    float y = RndFloat() * 2.f - 1.f;
    float z = RndFloat() * 2.f - 1.f;
    float mag = sqrt((x * x) + (y * y) + (z * z));
    x = U * x / mag;
    y = U * y / mag;
    z = U * z / mag;

    return { x, y };
}

unsigned TausStep(unsigned& z, int S1, int S2, int S3, unsigned M) { unsigned b = (((z << S1) ^ z) >> S2);   return z = (((z & M) << S3) ^ b); }
unsigned LCGStep(unsigned& z, unsigned A, unsigned C) { return z = (A * z + C); }

thread_local unsigned z1 = 0, z2 = 0, z3 = 0, z4 = 0;
float HybridTaus()
{
    // Combined period is lcm(p1,p2,p3,p4)~ 2^121
    return 2.3283064365387e-10 * (
        // Periods
        TausStep(z1, 13, 19, 12, 4294967294UL) ^
        // p1=2^31-1
        TausStep(z2, 2, 25, 4, 4294967288UL) ^
        // p2=2^30-1
        TausStep(z3, 3, 11, 17, 4294967280UL) ^
        // p3=2^28-1
        LCGStep(z4, 1664525, 1013904223UL)
        // p4=2^32
        );
}

D2D1_POINT_2F BoxMuller()
{
    float u0 = HybridTaus();// RndFloat();
    float u1 = HybridTaus();// RndFloat();
    //float r = sqrt(-2.f * log(u0));
    float r = sqrt(u0);
    float theta = DirectX::XM_2PI * u1;
    return { r * sin(theta), r * cos(theta) };

    /*float sigma = 4.f;
    float mu = 0.f;


    float mag = sigma * sqrt(-2.0f * log(u0));
    float z0 = mag * cos(DirectX::XM_2PI * u1) + mu;
    float z1 = mag * sin(DirectX::XM_2PI * u1) + mu;

    return { z0, z1 };*/
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float qerp(float a, float b, float c, float t)
{
    float ab = lerp(a, b, t);
    float bc = lerp(b, c, t);

    return lerp(ab, bc, t);
}

float cerp(float a, float b, float c, float d, float t)
{
    /*float abc = qerp(a, b, c, t);
    float bcd = qerp(b, c, d, t);

    return lerp(abc, bcd, t);*/

    float ab = lerp(a, b, t);
    float bc = lerp(b, c, t);
    float cd = lerp(c, d, t);

    float abc = lerp(ab, bc, t);
    float bcd = lerp(bc, cd, t);

    return lerp(abc, bcd, t);

    /*float u = 1.f - t;
    float uu = u * u;
    float uuu = uu * u;

    float tt = t * t;
    float ttt = tt * t;

    float res =
        uuu * a +
        3.f * uu * t * b +
        3.f * u * tt * c +
        ttt * d;

    return res;*/
}

//
//  Called whenever the application needs to display the client
//  window. This method writes "Hello, World"
//
//  Note that this function will not render anything if the window
//  is occluded (e.g. when the screen is locked).
//  Also, this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//
HRESULT DemoApp::OnRender()
{
    HRESULT hr;

    hr = CreateDeviceResources();

    const float VecLength = 0.9f;

    if (vectors.empty())
    //if(false)
    {
        for (size_t i = 0; i < 100000; i++)
        {
            //D2D1_POINT_2F rndVec = RandOnDisk();
            //D2D1_POINT_2F rndVec = RandOnDiskNoIter();
            //D2D1_POINT_2F rndVec = RandOnDiskNoIter2();
            //D2D1_POINT_2F rndVec = RandOnDiskPolar();
            //D2D1_POINT_2F rndVec = RandOnDiskCircular();
            //D2D1_POINT_2F rndVec = RandOnDiskCustom();
            D2D1_POINT_2F rndVec = BoxMuller();

            rndVec.x *= VecLength;
            rndVec.y *= VecLength;

            vectors.push_back(rndVec);
        }
    }

    //if (points.empty())
    if(false)
    {
        size_t max = 10;
        for (float i = 0; i <= max; i++)
        {
            float t = static_cast<float>(i) / static_cast<float>(max);

            //float x = t;
            //float y = sqrt(1.f - pow(t - 1.f, 2.f));

            float x = cerp(0.f, 0.f, 0.0f, 1.f, t);
            float y = cerp(0.f, 1.f, 1.f, 1.f, t);

            points.push_back(D2D1::Point2F(x, y));
        }
    }

    if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        static const WCHAR sc_helloWorld[] = L"Hello, World!";

        // Retrieve the size of the render target.
        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        {
            float aspectRatio = renderTargetSize.width / renderTargetSize.height;
            D2D1_MATRIX_3X2_F transform =
                D2D1::Matrix3x2F::Scale(1.f, -1.f) *
                D2D1::Matrix3x2F::Scale(1.f / aspectRatio, 1.f) *
                D2D1::Matrix3x2F::Scale(renderTargetSize.width * 0.5f, renderTargetSize.height * 0.5f) *
                D2D1::Matrix3x2F::Translation(renderTargetSize.width * 0.5f, renderTargetSize.height * 0.5f);

            auto TransformedPt = [transform](float x, float y)
            {
                return D2D1::Matrix3x2F::ReinterpretBaseType(&transform)->TransformPoint(D2D1::Point2F(x, y));
            };

            auto TransformedSize = [TransformedPt](float size)
            {
                float sizeInNormCoords = 1.f - size;
                return TransformedPt(0.f, sizeInNormCoords).y;
            };

            const float Radius = 5.f;
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> redBrush;

            hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), redBrush.GetAddressOf());

            if (!vectors.empty())
            {
                for (const D2D1_POINT_2F& vec : vectors)
                {
                    m_pRenderTarget->DrawEllipse(D2D1::Ellipse(TransformedPt(vec.x, vec.y), 1, 1), redBrush.Get());
                }

                vectors.clear();

                m_pRenderTarget->DrawEllipse(D2D1::Ellipse(TransformedPt(0.f, 0.f), TransformedSize(VecLength), TransformedSize(VecLength)), redBrush.Get());
            }

            if (!points.empty())
            {
                for (size_t i = 1; i < points.size(); i++)
                {
                    auto& pt1 = points[i - 1];
                    auto& pt2 = points[i];

                    m_pRenderTarget->DrawLine(TransformedPt(pt1.x, pt1.y), TransformedPt(pt2.x, pt2.y), redBrush.Get());
                }
            }

            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(0.f, 0.f), Radius, Radius), redBrush.Get());

            /*m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(-1.f, -1.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(0.f, -1.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(1.f, -1.f), Radius, Radius), redBrush.Get());

            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(-1.f, 0.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(0.f, 0.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(1.f, 0.f), Radius, Radius), redBrush.Get());

            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(-1.f, 1.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(0.f, 1.f), Radius, Radius), redBrush.Get());
            m_pRenderTarget->FillEllipse(D2D1::Ellipse(TransformedPt(1.f, 1.f), Radius, Radius), redBrush.Get());*/
        }

        /*m_pRenderTarget->DrawText(
            sc_helloWorld,
            ARRAYSIZE(sc_helloWorld) - 1,
            m_pTextFormat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            m_pBlackBrush
            );*/

        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}

//
//  If the application receives a WM_SIZE message, this method
//  resizes the render target appropriately.
//
void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(size);
    }
}


//
// The window message handler.
//
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                ));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_PAINT:
            case WM_DISPLAYCHANGE:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hwnd, &ps);

                    pDemoApp->OnRender();
                    EndPaint(hwnd, &ps);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                wasHandled = true;
                result = 1;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}
