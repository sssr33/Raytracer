#include "Helpers.h"

#include <cassert>

size_t Helpers::CeiledDiv(size_t a, size_t b)
{
    assert(b > 0);
    size_t result = (a + b - 1) / b;
    return result;
}

size_t Helpers::NextMultipleOf(size_t val, size_t multiple)
{
    size_t nextMultiple = Helpers::CeiledDiv(val, multiple) * multiple;
    return nextMultiple;
}