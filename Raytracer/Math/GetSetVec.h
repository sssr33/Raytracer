#pragma once

#include <type_traits>

struct X {};
struct Y {};
struct Z {};

template<class T>
struct GetSetVec {};

template<>
struct GetSetVec<X>
{
    template<class VecT>
    static float Get(const VecT& vec)
    {
        return vec.x;
    }

    template<class VecT>
    static void Set(VecT& vec, float v)
    {
        vec.x = v;
    }
};

template<>
struct GetSetVec<Y>
{
    template<class VecT>
    static float Get(const VecT& vec)
    {
        return vec.y;
    }

    template<class VecT>
    static void Set(VecT& vec, float v)
    {
        vec.y = v;
    }
};

template<>
struct GetSetVec<Z>
{
    template<class VecT>
    static float Get(const VecT& vec)
    {
        return vec.z;
    }

    template<class VecT>
    static void Set(VecT& vec, float v)
    {
        vec.z = v;
    }
};
