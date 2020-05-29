#pragma once
#include "Chunk.h"

#include <functional>

using BaseFunctor = std::function<void(const Chunk&)>;