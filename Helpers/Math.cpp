#include "Math.h"

#include <cassert>

namespace Helpers
{
    size_t Math::CeiledDiv(size_t a, size_t b)
    {
        assert(b > 0);
        size_t result = (a + b - 1) / b;
        return result;
    }

    size_t Math::NextMultipleOf(size_t val, size_t multiple)
    {
        size_t nextMultiple = Math::CeiledDiv(val, multiple) * multiple;
        return nextMultiple;
    }
}
