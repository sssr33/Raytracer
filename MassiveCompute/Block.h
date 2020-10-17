#pragma once

#include <cstdint>

namespace MassiveCompute
{
    struct Block
    {
        size_t left = 0; // inclusive
        size_t top = 0; // inclusive
        size_t right = 0; // exclusive
        size_t bottom = 0; // exclusive
        size_t imageWidth = 0;
        size_t imageHeight = 0;

        bool operator==(const Block& other) const;
    };
}
