#include "Window.h"

#include <stdexcept>
#include <mutex>
#include <utility>
#include <cassert>

const std::wstring Window::WndClassName = L"RaytraceWindow";

Window::Window(const std::wstring& title)
    : wndReg(Window::GetWndRegistration())
{
    this->hwnd.reset(CreateWindowW(Window::WndClassName.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, Window::GetHInstance(), this));

    if (!this->hwnd)
    {
        throw std::exception();
    }

    ShowWindow(this->hwnd.get(), SW_SHOW);
    UpdateWindow(this->hwnd.get());
}

Window::Window(Window&& other) noexcept
{
    swap(*this, other);
}

Window& Window::operator=(Window other) noexcept
{
    swap(*this, other);
    return *this;
}

void swap(Window& a, Window& b) noexcept
{
    using std::swap;
    swap(a.wndReg, b.wndReg);
    swap(a.hwnd, b.hwnd);
    swap(a.quit, b.quit);

    // swap <GWLP_USERDATA> for both windows so that they can to use right <Window> instance in theirs <WndProc>

    if (a.hwnd)
    {
        SetWindowLongPtrW(a.hwnd.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&a));
    }

    if (b.hwnd)
    {
        SetWindowLongPtrW(b.hwnd.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&b));
    }
}

std::variant<Window::Nothing, Window::Quit> Window::ProcessMessages(size_t maxToProcess)
{
    MSG msg = {};

    for (size_t i = 0; i < maxToProcess && PeekMessageW(&msg, this->hwnd.get(), 0, 0, PM_REMOVE); i++)
    {
        // Translate the message and dispatch it to WindowProc()
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (this->quit)
    {
        return Quit();
    }

    return Nothing();
}

Arg::MsgProcessed Window::ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return Arg::MsgProcessed(false);
}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Window* wnd = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCTW createParams = reinterpret_cast<LPCREATESTRUCTW>(lparam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createParams->lpCreateParams));

        break;
    }
    case WM_DESTROY:
        assert(wnd);
        // use this instead of <PostQuitMessage> because it posts message not for specific window and then any window on thread can peek it
        wnd->quit = true;
        break;
    default:
        if (wnd && wnd->ProcessMessage(hwnd, msg, wparam, lparam))
        {
            return 0;
        }
        else
        {
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
    }

    return 0;
}

HINSTANCE Window::GetHInstance()
{
    return GetModuleHandle(NULL);
}

std::shared_ptr<Window::WndClassRegistration> Window::GetWndRegistration()
{
    static std::mutex mtx;
    static std::weak_ptr<WndClassRegistration> weakReg;

    std::lock_guard<std::mutex> lk(mtx);

    std::shared_ptr<WndClassRegistration> reg = weakReg.lock();
    if (!reg)
    {
        reg = std::make_shared<WndClassRegistration>();
        weakReg = reg;
    }

    return reg;
}




Window::WndClassRegistration::WndClassRegistration()
{
    HINSTANCE hInstance = Window::GetHInstance();
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = Window::WndClassName.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    ATOM registration = RegisterClassExW(&wcex);
    if (!registration)
    {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS)
        {
            throw std::exception();
        }
    }
}

Window::WndClassRegistration::~WndClassRegistration()
{
    try
    {
        this->Unregister();
    }
    catch(...)
    { }
}

void Window::WndClassRegistration::Unregister()
{
    if (!UnregisterClassW(Window::WndClassName.c_str(), Window::GetHInstance()))
    {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_HAS_WINDOWS)
        {
            throw std::exception();
        }
    }
}




void Window::HwndDeleter::operator()(HWND hwnd)
{
    DestroyWindow(hwnd);
}
