// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Window.h"
#include "GameWindow.h"
#include "RayTraceWindowHandler.h"

#include <iostream>
#include <Helpers/is.h>
#include <MassiveCompute/MassiveComputeTest.h>

#include "Math/vec.h"

int main()
{
    {
        vec2<float> v2(1, 2);
        vec3<float> v3(3, 4, 5);
        vec4<float> v4(6, 7, 8, 9);

        sizeof(v2);
        sizeof(v3);
        sizeof(v4);

        v2 = v2.swizzle.y.x;
        v4 = v3.swizzle.x.z.z.y;
        v4 = v2.swizzle.y.y.x.x;

        int stop = 243;
    }

    {
        vec2<float> v1 = { 1.f, 2.f };
        vec2<float> v2 = { 3.f, 4.f };

        v1.swizzle.x.y = v1.swizzle.x.x;
        v1.swizzle.y.x = v2;

        v1 = v1 + v2;
        v1.swizzle.y.x = v1 + v2;

        vec3 v3;
        v3 = v1.swizzle.x.x.x;
        v3 = v1.y;
        v3 = v1.swizzle.y.x.y;

        v1 = { 1.f, 2.f };
        v2 = { 3.f, 4.f };

        v3.swizzle.z.x = v3.swizzle.z.x() + v1 + v2;
        v3.swizzle.z.x = v1 + v2;
        int stop = 243;
    }

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
