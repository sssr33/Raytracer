// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Window.h"
#include <MassiveCompute/MassiveComputeTest.h>

template <class _Ty, class... _Types>
constexpr bool is(const std::variant<_Types...>& _Var) noexcept
{
    return std::holds_alternative<_Ty>(_Var);
}

int main()
{
    //MassiveComputeTest();

    {
        Window wnd(L"wnd 1");
        Window wnd2(L"wnd 2");

        bool wndActive = true;
        bool wnd2Active = true;

        int count = 0;

        while (wndActive || wnd2Active)
        {
            wndActive = !is<Window::Quit>(wnd.ProcessMessages());
            wnd2Active = !is<Window::Quit>(wnd2.ProcessMessages());

            if (!wndActive)
            {
                int stop = 432;
            }

            if (!wnd2Active)
            {
                int stop = 432;
            }
        }
    }


    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
