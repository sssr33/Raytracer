#pragma once

#include <cstdint>
#include <limits>
#include <tuple>

class SemanticValueHelpers {
public:
    static constexpr size_t InvalidIdx = (std::numeric_limits<size_t>::max)();

    template<typename ValueT, typename T>
    static constexpr size_t CountValues();

    template<typename ValueT, typename T>
    static constexpr size_t GetFieldIdxFromValueIdx(size_t regIdx);

    template<typename ValueT, typename T>
    static constexpr size_t GetValueIdxFromFieldIdx(size_t fieldIdx);

    template<typename DstT, typename SrcT>
    static constexpr void CopyValues(DstT& dst, const SrcT& src);

    template<typename Fn, typename... TupleArgs>
    static constexpr std::tuple<TupleArgs...> ApplyValues(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, Fn fn);
};

#include "SemanticValueHelpers.hpp"
