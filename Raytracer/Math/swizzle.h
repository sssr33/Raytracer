#pragma once
#include "VecRef.h"

template<class... T, class VecT>
VecRef<std::conditional_t<std::is_const<VecT>::value, const VecT, VecT>, T...> swizzle(VecT& v)
{
    return VecRef<std::conditional_t<std::is_const<VecT>::value, const VecT, VecT>, T...>(v);
}
