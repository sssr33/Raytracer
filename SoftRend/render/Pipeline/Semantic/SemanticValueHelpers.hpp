#pragma once
#include "SemanticValueHelpers.h"

#include <Helpers\Meta\for_idx.h>

template<typename ValueT, typename T>
constexpr size_t SemanticValueHelpers::CountValues() {
    size_t count = 0;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if (std::is_base_of_v<ValueT, std::tuple_element_t<i.value, T>> || std::is_same_v<ValueT, std::tuple_element_t<i.value, T>>) {
            ++count;
        }
        });

    return count;
}

template<typename ValueT, typename T>
constexpr size_t SemanticValueHelpers::GetFieldIdxFromValueIdx(size_t valIdx) {
    size_t count = 0;
    size_t fieldIdx = InvalidIdx;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if constexpr (std::is_base_of_v<ValueT, std::tuple_element_t<i.value, T>> || std::is_same_v<ValueT, std::tuple_element_t<i.value, T>>) {
            if (count == valIdx) {
                fieldIdx = i.value;
            }
            ++count;
        }
        });

    return fieldIdx;
}

template<typename ValueT, typename T>
constexpr size_t SemanticValueHelpers::GetValueIdxFromFieldIdx(size_t fieldIdx) {
    size_t count = 0;
    size_t valIdx = InvalidIdx;

    for_idx<std::tuple_size_v<T>>([&](auto i) {
        if constexpr (std::is_base_of_v<ValueT, std::tuple_element_t<i.value, T>> || std::is_same_v<ValueT, std::tuple_element_t<i.value, T>>) {
            if (fieldIdx == i.value) {
                valIdx = count;
            }
            ++count;
        }
        });

    return valIdx;
}

template<typename ValueT, size_t Idx /*= 0*/, typename T>
constexpr const ValueT& SemanticValueHelpers::GetValue(const T& v) {
    constexpr size_t fieldIdx = SemanticValueHelpers::GetFieldIdxFromValueIdx<ValueT, T>(Idx);
    return std::get<fieldIdx>(v);
}

template<typename DstT, typename SrcT>
constexpr void SemanticValueHelpers::CopyValues(DstT& dst, const SrcT& src) {
    using LessFeildsT = std::conditional_t < std::tuple_size_v<DstT> < std::tuple_size_v<SrcT>, DstT, SrcT>;

    for_idx<std::tuple_size_v<LessFeildsT>>([&](auto i)
        {
            using ValueT = std::tuple_element_t<i.value, LessFeildsT>;
            constexpr auto ValIdx = GetValueIdxFromFieldIdx<ValueT, LessFeildsT>(i.value);
            constexpr auto DstFieldIdx = GetFieldIdxFromValueIdx<ValueT, DstT>(ValIdx);
            constexpr auto SrcFieldIdx = GetFieldIdxFromValueIdx<ValueT, SrcT>(ValIdx);

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
constexpr std::tuple<TupleArgs...> SemanticValueHelpers::ApplyValues(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, Fn fn) {
    using T = std::tuple<TupleArgs...>;
    T result;

    for_idx<std::tuple_size_v<T>>([&](auto i)
        {
            using ValueT = std::tuple_element_t<i.value, T>;

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
