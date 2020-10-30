#pragma once

#include <type_traits>

template<class SourceT, class DestT>
struct PropagateConst
{
    typedef typename std::conditional_t<
        std::is_const<SourceT>::value,
        const typename std::remove_const<DestT>::type,
        typename std::remove_const<DestT>::type>
        type;
};

template<class SourceT, class DestT>
using PropagateConstT = typename PropagateConst<SourceT, DestT>::type;
