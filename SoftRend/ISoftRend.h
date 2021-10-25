#pragma once

#include <memory>

class ISoftRend
{
public:
    virtual ~ISoftRend() = default;
};

std::unique_ptr<ISoftRend> CreateISoftRend();
