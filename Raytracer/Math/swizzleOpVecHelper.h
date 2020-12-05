#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

template<class ComponentT, int indexCount>
struct swizzleOpVecHelperTypes {};

template<class ComponentT>
struct swizzleOpVecHelperTypes<ComponentT, 2>
{
    typedef vec2<ComponentT> T;
};

template<class ComponentT>
struct swizzleOpVecHelperTypes<ComponentT, 3>
{
    typedef vec3<ComponentT> T;
};

template<class ComponentT>
struct swizzleOpVecHelperTypes<ComponentT, 4>
{
    typedef vec4<ComponentT> T;
};

template<class VecT, class ComponentT, class ... Indexes>
struct swizzleOpVecHelper
{
    typedef typename swizzleOpVecHelperTypes<ComponentT, sizeof...(Indexes)>::T T;

    static void Assign(VecT* _this, const T rhs)
    {
        (((*_this)[Indexes::dst] = rhs[Indexes::src]), ...);
    }

    static T Get(const VecT* _this)
    {
        return T((*_this)[Indexes::dst]...);
    }
};

template<class VecT, class ComponentT, class Index0>
struct swizzleOpVecHelper<VecT, ComponentT, Index0>
{
    typedef ComponentT T;

    static void Assign(VecT* _this, const T rhs)
    {
        (*_this)[Index0::dst] = rhs;
    }

    static T Get(const VecT* _this)
    {
        return T((*_this)[Index0::dst]);
    }
};
