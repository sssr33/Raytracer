// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Window.h"
#include "GameWindow.h"
#include "RayTraceWindowHandler.h"
#include "Math/MathTest.h"
#include "Math/vec.h"

#include <iostream>
#include <Helpers/is.h>
#include <MassiveCompute/MassiveComputeTest.h>

#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

/*
* Perlin noise links
* https://rtouti.github.io/graphics/perlin-noise-algorithm#:~:text=Perlin%20noise%20is%20a%20popular,number%20of%20inputs%20it%20gets.
* http://www.cplusplus.com/reference/algorithm/random_shuffle/
* https://en.cppreference.com/w/cpp/algorithm/random_shuffle
* https://www.youtube.com/watch?v=Or19ilef4wE
*/

struct ss
{
    int a = 0;
    std::vector<int> vv;
};

int main()
{
    //MathTest();
    //MassiveComputeTest();

    {
        vec3<float> start = { -1.f, -2.f, 0.f };
        vec3<float> end = { -4.f, 0.f, 0.f };

        vec3<float> v = end - start;

        vec3<float> end2 = start + 0.75f * v;

        v = end2 - start;

        vec3<float> vAbs = { std::fabsf(v.x), std::fabsf(v.y), std::fabsf(v.z) };

        vec2<float> texCoords;

        if (vAbs.x > vAbs.y && vAbs.x > vAbs.z)
        {
            float t = -start.x / v.x;
            texCoords = (start + t * v).swizzle.y.z;
        }
        else if (vAbs.y > vAbs.z)
        {
            float t = -start.y / v.y;
            texCoords = (start + t * v).swizzle.x.z;
        }
        else
        {
            float t = -start.z / v.z;
            texCoords = (start + t * v).swizzle.x.y;
        }

        float dx = v.dot({ 1.f, 0.f, 0.f });
        float dy = v.dot({ 0.f, 1.f, 0.f });
        float dz = v.dot({ 0.f, 0.f, 1.f });

        float dxn = v.normalized().dot({ 1.f, 0.f, 0.f });
        float dyn = v.normalized().dot({ 0.f, 1.f, 0.f });
        float dzn = v.normalized().dot({ 0.f, 0.f, 1.f });

        vec3<float> vt = -start / v;

        vec3<float> xend = start + vt.x * v;
        vec3<float> yend = start + vt.y * v;
        vec3<float> zend = start + vt.z * v;

        zend *= 0.f;



        int stop = 234; stop++;
    }

    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(v.begin(), v.end(), g);

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
