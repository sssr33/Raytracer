#pragma once
#include "ScatterRecord.h"
#include "Math/ray.h"
#include "Render/Hitable/HitRecord.h"

#include <optional>

#define USE_IRandomInUnitSphere 0
#define USE_RND_UNIT_SPHERE 0

class IMaterial
{
public:
    virtual ~IMaterial() = default;

    virtual std::optional<ScatterRecord> Scatter(const ray<float>& r, const HitRecord& hitRecord) const = 0;
};
