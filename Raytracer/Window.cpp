#include "Window.h"

#include <stdexcept>
#include <mutex>

const std::wstring Window::WndClassName = L"RaytraceWindow";

Window::Window()
    : wndReg(Window::GetWndRegistration())
{}

void Window::CreateWnd()
{

}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProcW(hwnd, msg, wparam, lparam);
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
