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

// based on
// https://kiorisyshen.github.io/2018/08/27/Vector%20Swizzling%20and%20Parameter%20Pack%20in%20C++/

template<template<typename> class TT, typename T, int ... Indexes>
class swizzle2 {
    // here sizeof...() is a function to query the number of parameter pack
    T v[sizeof...(Indexes)];
public:
    TT<T>& operator=(const TT<T>& rhs)
    {
        int indexes[] = { Indexes... }; // unpack
        for (int i = 0; i < sizeof...(Indexes); i++) {
            v[indexes[i]] = rhs[i]; // access pack element
        }
        return *(TT<T>*)this;
    }
    operator TT<T>() const
    {
        // unpack
        return TT<T>(v[Indexes]...);
    }
};

// --------------------------
// Define multi-dimensional vector type
// --------------------------
template <typename T = float>
struct vv2
{
    union {
        struct { T x, y; };
        T data[2];
        

        union
        {
            swizzle2<vv2, T, 0, 1> xy;
            swizzle2<vv2, T, 0, 0> xx;
            swizzle2<vv2, T, 1, 0> yx;
            swizzle2<vv2, T, 0, 2> xz;
            swizzle2<vv2, T, 2, 0> zx;
            swizzle2<vv2, T, 1, 2> yz;
            swizzle2<vv2, T, 2, 1> zy;
        } swizzle;

        
    };
    vv2<T>() {};
    vv2<T>(const T& _v) : x(_v), y(_v) {};
    vv2<T>(const T& _x, const T& _y) : x(_x), y(_y) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };
};

template<int d, int s> struct IndexPair
{
    static constexpr int dst = d;
    static constexpr int src = s;
};

template<class GenTypeSrc, class... Args> struct Pack
{
    template<class... Args2> struct Next
    {
        typedef Pack<GenTypeSrc, Args..., Args2...> T;
    };

    typedef typename GenTypeSrc::template GenType<Args...>::T Gen;
};

template<int count, int curIdx, class Pk, int cur, int ... Indexes>
struct GenPairsHelpersNext : public GenPairsHelpersNext<count - 1, curIdx + 1, typename Pk::template Next<IndexPair<cur, curIdx>>::T, Indexes...>
{};

template<int curIdx, class Pk, int last>
struct GenPairsHelpersNext<0, curIdx, Pk, last>
{
    typedef typename Pk::template Next<IndexPair<last, curIdx>>::T T;
    typedef typename Pk::template Next<IndexPair<last, curIdx>>::T::Gen Gen;
};

template<int count, class Pk, int ... Indexes>
struct GenPairsHelpers : public GenPairsHelpersNext<count - 1, 0, Pk, Indexes...>
{};

template<class GenTypeSrc, int ... Indexes>
struct GenPairs : public GenPairsHelpers<sizeof...(Indexes), Pack<GenTypeSrc>, Indexes...>
{};

//template<int ... Indexes> struct IndexPack {};
//
//template<int count, int ... Indexes>
//struct GenIndexes : public GenIndexes<count - 1, count - 1, Indexes...>
//{};
//
//template<int ... Indexes>
//struct GenIndexes<0, Indexes...>
//{
//    typedef IndexPack<Indexes...> Type;
//};

// Vector3d
template <typename T>
struct vv3
{
    union {
        T data[3];
        struct { T x, y, z; };

        union
        {
            swizzle2<vv2, T, 0, 1> xy;
            swizzle2<vv2, T, 0, 0> xx;
            swizzle2<vv2, T, 1, 0> yx;
            swizzle2<vv2, T, 0, 2> xz;
            swizzle2<vv2, T, 2, 0> zx;
            swizzle2<vv2, T, 1, 2> yz;
            swizzle2<vv2, T, 2, 1> zy;
            swizzle2<vv3, T, 0, 1, 2> xyz;
            swizzle2<vv3, T, 1, 0, 2> yxz;
            swizzle2<vv3, T, 0, 2, 1> xzy;
            swizzle2<vv3, T, 2, 0, 1> zxy;
            swizzle2<vv3, T, 1, 2, 0> yzx;
            swizzle2<vv3, T, 2, 1, 0> zyx;
        } swizzle;
        
    };
    vv3<T>() {};
    vv3<T>(const T& _v) : x(_v), y(_v), z(_v) {};
    vv3<T>(const T& _x, const T& _y, const T& _z) : x(_x), y(_y), z(_z) {};
    operator T* () { return data; };
    operator const T* () const { return static_cast<const T*>(data); };
};

template<int> struct ShowMax {};

template<int ... Indexes, class Fn>
ShowMax<(std::max)({ Indexes ...})>TemplateFor(Fn fn)
{
    (fn(Indexes), ...);
    return ShowMax < (std::max)({ Indexes ... }) > ();
}

template<template<typename> class TT, typename T, class ... Indexes>
class swizzle22 {
    // use constexpr std::max to find max index that will be written
    // for example vec2 can write XZ components and must access indexes 0 and 2
    T v[(std::max)({ Indexes::dst ... }) + 1];
public:
    TT<T>& operator=(const TT<T>& rhs)
    {
        ((v[Indexes::dst] = rhs[Indexes::src]), ...);
        return *(TT<T>*)this;
    }
    operator TT<T>() const
    {
        // unpack
        return TT<T>(v[Indexes::d]...);
    }

    template<class ... NewIndexes> struct GenType
    {
        typedef typename swizzle22<TT, T, NewIndexes...> T;
    };
};

template<template<typename> class TT, typename T>
struct swizzle22GenHelper {
    template<class ... NewIndexes> struct GenType
    {
        typedef typename swizzle22<TT, T, NewIndexes...> T;
    };
};

template<template<typename> class TT, typename T, int ... Indexes>
struct swizzle22Gen {
    typedef typename GenPairs<swizzle22GenHelper<TT, T>, Indexes...>::Gen T;
};

int main()
{
    {
        swizzle22Gen<vv2, float, 1, 0>::T xy;

        vv2<float> v(1, 2);

        sizeof(xy);

        xy = v;

        xy = xy;

        int i = 0;

        int a[3];
        int b[3];

        (a[0] = b[2], (a[2] = b[1], (i++)));

        int stop = 324;
    }

    //GenPairs<3, 2, 1>::

    TemplateFor<1, 3, 2, 8, 4>(



        [](int i)
        {
            int stop = 324;
        });






    vv2 x = { 1.f, 2.f };

    //sizeof(x);

    x.swizzle.xy = x.swizzle.xx;

    //x.set(x, y, z) = 213;

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
