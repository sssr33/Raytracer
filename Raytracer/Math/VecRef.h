#pragma once
#include "GetSetVec.h"

template<class ImplT, class ImplVal>
struct VecRefBase2
{
    template<class ImplT2, class ImplVal2>
    std::enable_if_t<!std::is_const<ImplT>::value, ImplT&> operator+=(const VecRefBase2<ImplT2, ImplVal2>& other)
    {
        this->setX(this->getX() + other.getX());
        this->setY(this->getY() + other.getY());

        return static_cast<ImplT&>(*this);
    }

    template<class ImplT2, class ImplVal2>
    ImplVal operator+(const VecRefBase2<ImplT2, ImplVal2>& other) const
    {
        ImplVal res;

        res.setX(this->getX() + other.getX());
        res.setY(this->getY() + other.getY());

        return res;
    }

    float getX() const
    {
        return static_cast<const ImplT&>(*this).getX();
    }

    float getY() const
    {
        return static_cast<const ImplT&>(*this).getY();
    }

    std::enable_if_t<!std::is_const<ImplT>::value> setX(float v)
    {
        static_cast<ImplT&>(*this).setX(v);
    }

    std::enable_if_t<!std::is_const<ImplT>::value> setY(float v)
    {
        static_cast<ImplT&>(*this).setY(v);
    }
};

template<class VecT, class... T>
struct VecRef {};

struct vec2;

template<class VecT, class XT, class YT>
struct VecRef<VecT, XT, YT> : public VecRefBase2<std::conditional_t<std::is_const<VecT>::value, const VecRef<VecT, XT, YT>, VecRef<VecT, XT, YT>>, vec2>
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
