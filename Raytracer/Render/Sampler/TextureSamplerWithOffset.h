#pragma once
#include "ITextureSampler.h"

#include <memory>

template<class PixelT>
class TextureSamplerWithOffset : public ITextureSampler<PixelT>
{
public:
    explicit TextureSamplerWithOffset(std::shared_ptr<ITextureSampler<PixelT>> sampler, const vec2<float>& offset)
        : sampler(std::move(sampler))
        , offset(offset)
    {}

    PixelT Sample(const vec2<float>& texCoords) const override
    {
        return this->sampler->Sample(texCoords + this->offset);
    }

private:
    std::shared_ptr<ITextureSampler<PixelT>> sampler;
    vec2<float> offset = { 0.f, 0.f };
};
