#pragma once
#include "SemanticRegisterHelpers.h"

#include <Helpers\Meta\for_idx.h>

template<typename RegisterT, typename T>
constexpr size_t SemanticRegisterHelpers::CountRegisters() {
    size_t count = 0;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if (std::is_base_of_v<RegisterT, std::tuple_element_t<i.value, T>> || std::is_same_v<RegisterT, std::tuple_element_t<i.value, T>>) {
            ++count;
        }
        });

    return count;
}

template<typename RegisterT, typename T>
constexpr size_t SemanticRegisterHelpers::GetFieldIdxFromRegisterIdx(size_t regIdx) {
    size_t count = 0;
    size_t fieldIdx = InvalidIdx;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if constexpr (std::is_base_of_v<RegisterT, std::tuple_element_t<i.value, T>> || std::is_same_v<RegisterT, std::tuple_element_t<i.value, T>>) {
            if (count == regIdx) {
                fieldIdx = i.value;
            }
            ++count;
        }
        });

    return fieldIdx;
}

template<typename RegisterT, typename T>
constexpr size_t SemanticRegisterHelpers::GetRegisterIdxFromFieldIdx(size_t fieldIdx) {
    size_t count = 0;
    size_t regIdx = InvalidIdx;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if constexpr (std::is_base_of_v<RegisterT, std::tuple_element_t<i.value, T>> || std::is_same_v<RegisterT, std::tuple_element_t<i.value, T>>) {
            if (fieldIdx == i.value) {
                regIdx = count;
            }
            ++count;
        }
        });

    return regIdx;
}

template<typename DstT, typename SrcT>
constexpr void SemanticRegisterHelpers::CopyRegisters(DstT& dst, const SrcT& src) {
    using LessFeildsT = std::conditional_t < std::tuple_size_v<DstT> < std::tuple_size_v<SrcT>, DstT, SrcT>;

    for_idx<std::tuple_size_v<LessFeildsT>>([&](auto i)
        {
            using RegisterT = std::tuple_element_t<i.value, LessFeildsT>;
            constexpr auto RegIdx = GetRegisterIdxFromFieldIdx<RegisterT, LessFeildsT>(i.value);
            constexpr auto DstFieldIdx = GetFieldIdxFromRegisterIdx<RegisterT, DstT>(RegIdx);
            constexpr auto SrcFieldIdx = GetFieldIdxFromRegisterIdx<RegisterT, SrcT>(RegIdx);

            if constexpr (DstFieldIdx != InvalidIdx && SrcFieldIdx != InvalidIdx) {
                const auto& srcRef = std::get<SrcFieldIdx>(src);
                auto& dstRef = std::get<DstFieldIdx>(dst);
                // using refs works a bit better on MSVC than std::get<DstFieldIdx>(dst) = std::get<SrcFieldIdx>(src)
                // tested with https://godbolt.org/
                dstRef = srcRef;
            }
        });
}

template<typename Fn, typename... TupleArgs>
constexpr std::tuple<TupleArgs...> SemanticRegisterHelpers::ApplyRegisters(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, Fn fn) {
    using T = std::tuple<TupleArgs...>;
    T result;

    for_idx<std::tuple_size_v<T>>([&](auto i)
        {
            using RegisterT = std::tuple_element_t<i.value, T>;

            // using refs works a bit better on MSVC than std::get<DstFieldIdx>(dst) = std::get<SrcFieldIdx>(src)
            // tested with https://godbolt.org/
            auto& resultRef = std::get<i.value>(result);
            const auto& aRef = std::get<i.value>(a);
            const auto& bRef = std::get<i.value>(b);

            // use static_cast because fn can return basic types which are (down)castable to semantic types
            // this may result in slicing, but if more specialized overload will be found then no slicing happens
            using FnResT = decltype(fn(aRef, bRef));
            static_cast<FnResT&>(resultRef) = fn(aRef, bRef);
        });

    return result;
}
