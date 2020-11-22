#pragma once
#include "GetSetVec.h"

template<class VecT, class... T>
struct VecRef {};

template<class VecT, class XT, class YT>
struct VecRef<VecT, XT, YT>
{
    VecRef(VecT& vec)
        : vec(vec)
    {}

    VecRef& operator=(const VecRef& v)
    {
        return this->operator=<VecRef>(v);
    }

    template<class VectorT>
    VecRef& operator=(const VectorT& v)
    {
        float a = v.getX();
        float b = v.getY();

        this->setX(a);
        this->setY(b);

        return *this;
    }

    float getX() const
    {
        return GetSetVec<XT>::Get(this->vec);
    }

    float getY() const
    {
        return GetSetVec<YT>::Get(this->vec);
    }

    std::enable_if_t<!std::is_const<VecT>::value> setX(float v)
    {
        GetSetVec<XT>::Set(this->vec, v);
    }

    std::enable_if_t<!std::is_const<VecT>::value> setY(float v)
    {
        GetSetVec<YT>::Set(this->vec, v);
    }

private:
    VecT& vec;
};

template<class VecT, class XT, class YT, class ZT>
struct VecRef<VecT, XT, YT, ZT>
{
    VecRef(VecT& vec)
        : vec(vec)
    {}

    VecRef& operator=(const VecRef& v)
    {
        return this->operator=<VecRef>(v);
    }

    template<class VectorT>
    VecRef& operator=(const VectorT& v)
    {
        float a = v.getX();
        float b = v.getY();
        float c = v.getZ();

        this->setX(a);
        this->setY(b);
        this->setZ(c);

        return *this;
    }

    float getX() const
    {
        return GetSetVec<XT>::Get(this->vec);
    }

    float getY() const
    {
        return GetSetVec<YT>::Get(this->vec);
    }

    float getZ() const
    {
        return GetSetVec<ZT>::Get(this->vec);
    }

    std::enable_if_t<!std::is_const<VecT>::value> setX(float v)
    {
        GetSetVec<XT>::Set(this->vec, v);
    }

    std::enable_if_t<!std::is_const<VecT>::value> setY(float v)
    {
        GetSetVec<YT>::Set(this->vec, v);
    }

    std::enable_if_t<!std::is_const<VecT>::value> setZ(float v)
    {
        GetSetVec<ZT>::Set(this->vec, v);
    }

private:
    VecT& vec;
};
