#pragma once

#include <cstdint>
#include <limits>
#include <tuple>

class SemanticRegisterHelpers {
public:
    static constexpr size_t InvalidIdx = (std::numeric_limits<size_t>::max)();

    template<typename RegisterT, typename T>
    static constexpr size_t CountRegisters();

    template<typename RegisterT, typename T>
    static constexpr size_t GetFieldIdxFromRegisterIdx(size_t regIdx);

    template<typename RegisterT, typename T>
    static constexpr size_t GetRegisterIdxFromFieldIdx(size_t fieldIdx);

    template<typename DstT, typename SrcT>
    static constexpr void CopyRegisters(DstT& dst, const SrcT& src);

    template<typename Fn, typename... TupleArgs>
    static constexpr std::tuple<TupleArgs...> ApplyRegisters(const std::tuple<TupleArgs...>& a, const std::tuple<TupleArgs...>& b, Fn fn);
};

#include "SemanticRegisterHelpers.hpp"
