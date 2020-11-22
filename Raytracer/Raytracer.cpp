// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Window.h"
#include "GameWindow.h"
#include "RayTraceWindowHandler.h"

#include <iostream>
#include <Helpers/is.h>
#include <MassiveCompute/MassiveComputeTest.h>

#include "Math/vec2.h"
#include "Math/vec3.h"

int main()
{
    vec2 v1 = { 1.f, 2.f };
    vec2 v2 = { 3.f, 4.f };

    swizzle<X, Y>(v1) = swizzle<X, X>(v1);
    swizzle<Y, X>(v1) = v2;

    v1 = v1 + v2;
    swizzle<Y, X>(v1) = v1 + v2;

    vec3 v3;
    v3 = swizzle<X, X, X>(v1);
    v3 = vec3(v1.y);
    v3 = swizzle<Y, X, Y>(v1);

    v1 = { 1.f, 2.f };
    v2 = { 3.f, 4.f };

    swizzle<Z, X>(v3) += v1 + v2;
    swizzle<Z, X>(v3) = v1 + v2;

    //MassiveComputeTest();

    RayTraceWindowHandler rayTraceHandler;
    // https://devblogs.microsoft.com/oldnewthing/20050217-00/?p=36423
    GameWindow wnd(rayTraceHandler, L"Raytracer");

    while (!is<Window::Quit>(wnd.ProcessMessages()))
    {

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
