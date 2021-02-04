#pragma once
#include "ScatterRecord.h"
#include "Math/ray.h"
#include "Render/Hitable/HitRecord.h"

#include <optional>

class IMaterial
{
public:
    virtual ~IMaterial() = default;

    virtual std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const = 0;
};
