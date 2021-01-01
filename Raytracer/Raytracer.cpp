// Raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Window.h"
#include "GameWindow.h"
#include "RayTraceWindowHandler.h"
#include "Math/MathTest.h"

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
