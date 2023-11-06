#pragma once
#include "num_v.h"

#include <utility>

template <class F, std::size_t... Is>
constexpr void for_idx(F func, std::index_sequence<Is...>) {
    (func(num_v<Is>{}), ...);
}

// https://stackoverflow.com/questions/37602057/why-isnt-a-for-loop-a-compile-time-expression
template <std::size_t N, typename F>
constexpr void for_idx(F func) {
    for_idx(func, std::make_index_sequence<N>());
}
