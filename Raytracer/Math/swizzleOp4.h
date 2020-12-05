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

        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<0, sizeof...(Indexes)>> r;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<1, sizeof...(Indexes)>> g;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<2, sizeof...(Indexes)>> b;
        swizzleOp4<OpHelper, VecT, ComponentT, left - 1, Indexes..., IndexPair<3, sizeof...(Indexes)>> a;
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

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator+(const swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    return static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(a) + static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(b);
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator+=(swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    auto res = a + b;
    a = res;
    return res;
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator-(const swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    return static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(a) - static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(b);
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator-=(swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    auto res = a - b;
    a = res;
    return res;
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator*(const swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    return static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(a) * static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(b);
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator*=(swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    auto res = a * b;
    a = res;
    return res;
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator/(const swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    return static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(a) / static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(b);
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class B, class ... Indexes>
std::enable_if_t<std::is_convertible_v<B, typename OpHelper<VecT, ComponentT, Indexes...>::T>, typename OpHelper<VecT, ComponentT, Indexes...>::T> operator/=(swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a, const B& b)
{
    auto res = a / b;
    a = res;
    return res;
}

template<template<class VecT, class ComponentT, class ... Indexes> class OpHelper, class VecT, class ComponentT, int left, class ... Indexes>
typename OpHelper<VecT, ComponentT, Indexes...>::T operator-(const swizzleOp4<OpHelper, VecT, ComponentT, left, Indexes...>& a)
{
    return -static_cast<typename OpHelper<VecT, ComponentT, Indexes...>::T>(a);
}
