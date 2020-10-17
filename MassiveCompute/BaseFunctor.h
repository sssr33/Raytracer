#pragma once
#include "Block.h"

#include <functional>

namespace MassiveCompute
{
    using BaseFunctor = std::function<void(const Block&)>;
}
