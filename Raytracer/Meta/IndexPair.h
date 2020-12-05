#pragma once

template<int d, int s> struct IndexPair
{
    static constexpr int dst = d;
    static constexpr int src = s;
};
