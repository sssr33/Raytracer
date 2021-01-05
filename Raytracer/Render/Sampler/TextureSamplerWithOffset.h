#pragma once
#include "ITextureSampler.h"

template<class PixelT>
class TextureSamplerWithOffset : public ITextureSampler<PixelT>
{
public:
    explicit TextureSamplerWithOffset(std::unique_ptr<ITextureSampler<PixelT>> sampler)
        : sampler(std::move(sampler))
    {}

    PixelT Sample(const vec2<float>& texCoords) const override
    {
        return this->sampler->Sample(texCoords + this->offset);
    }

    std::unique_ptr<ITextureSampler<PixelT>> Clone() const
    {
        std::unique_ptr<TextureSamplerWithOffset> clone = std::make_unique<TextureSamplerWithOffset>(this->sampler->Clone());

        clone->SetOffset(this->GetOffset());

        return clone;
    }

    const vec2<float>& GetOffset() const
    {
        return this->offset;
    }

    void SetOffset(const vec2<float>& offset)
    {
        this->offset = offset;
    }

private:
    std::unique_ptr<ITextureSampler<PixelT>> sampler;
    vec2<float> offset = { 0.f, 0.f };
};
