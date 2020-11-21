#pragma once

#include <future>

template<typename R>
bool is_ready(const std::future<R>& f)
{
    return f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
