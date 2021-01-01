#pragma once
#include "Base/ICloneable.h"

#include <memory>

template<class T>
class IRandom : public ICloneable<std::unique_ptr<IRandom<T>>>
{
public:
    virtual ~IRandom() = default;

    virtual T Next() = 0;
};
