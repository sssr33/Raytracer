#pragma once

#include <variant>
#include <utility>

template <class _Ty, class... _Types>
constexpr bool is(const std::variant<_Types...>& _Var) noexcept
{
    return std::holds_alternative<_Ty>(_Var);
}
