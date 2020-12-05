#pragma once

#include "Meta/IndexPair.h"

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class ... Indexes>
struct swizzleOp4
{
    union
    {
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<0, sizeof...(Indexes)>> x;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<1, sizeof...(Indexes)>> y;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<2, sizeof...(Indexes)>> z;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<3, sizeof...(Indexes)>> w;
    };

    typedef OpHelper<VecT, ComponentT, Indexes...> OpHelp;
    typedef typename OpHelp::T OpHelpT;

    OpHelpT operator=(const OpHelpT rhs)
    {
        OpHelp::Assign(reinterpret_cast<VecT*>(this), rhs);
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
    operator OpHelpT() const
    {
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
    OpHelpT operator()() const
    {
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
};

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, class ... Indexes>
struct swizzleOp4<OpHelper, VecT, ComponentT, 0, Indexes...>
{
    typedef OpHelper<VecT, ComponentT, Indexes...> OpHelp;
    typedef typename OpHelp::T OpHelpT;

    OpHelpT operator=(const OpHelpT rhs)
    {
        OpHelp::Assign(reinterpret_cast<VecT*>(this), rhs);
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
    operator OpHelpT() const
    {
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
    OpHelpT operator()() const
    {
        return OpHelp::Get(reinterpret_cast<const VecT*>(this));
    }
};
