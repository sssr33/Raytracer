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
    static constexpr size_t GetFieldIdxFromValueIdx(size_t valIdx);

    template<typename ValueT, typename T>
    static constexpr size_t GetValueIdxFromFieldIdx(size_t fieldIdx);

    template<typename ValueT, size_t Idx = 0, typename SemanticValueT>
    static constexpr const ValueT& GetValue(const SemanticValueT& sv);

    template<typename ValueT, size_t Idx = 0, typename SemanticValueT>
    static constexpr void SetValue(SemanticValueT& sv, const ValueT& val);

    template<typename DstT, typename SrcT>
    static constexpr void CopyValues(DstT& dst, const SrcT& src);

    template<typename Fn, typename... TupleArgs>
    static constexpr std::tuple<TupleArgs...> ApplyValues(const std::tuple<TupleArgs...>& a, Fn fn);

    template<typename Fn, typename... TupleArgs>
    static constexpr std::tuple<TupleArgs...> ApplyValues(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, Fn fn);

    template<typename Fn, typename... TupleArgs>
    static constexpr std::tuple<TupleArgs...> ApplyValues(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, const std::tuple<TupleArgs...>& c, Fn fn);
};

#include "SemanticValueHelpers.hpp"
