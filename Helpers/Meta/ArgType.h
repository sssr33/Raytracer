#pragma once

#include <tuple>

template <typename T>
struct ArgType;

template <typename ClassType, typename ReturnType, typename... Args>
struct ArgType<ReturnType(ClassType::*)(Args...)> {
    using return_type = ReturnType;
    using types = std::tuple<Args...>;

    template<size_t Index>
    using n_type = std::tuple_element_t<Index, types>;
};
