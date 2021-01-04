#pragma once

#include <utility>

template<class Ptr>
class ClonePtr
{
public:
    ClonePtr() = default;
    ~ClonePtr() = default;

    explicit ClonePtr(Ptr ptr)
        : ptr(std::move(ptr))
    {}

    ClonePtr(ClonePtr&&) = default;

    ClonePtr(const ClonePtr& other)
        : ptr(other.ptr->Clone())
    {}

    ClonePtr& operator=(Ptr ptr)
    {
        this->ptr = std::move(ptr);
        return *this;
    }

    ClonePtr& operator=(ClonePtr&&) = default;

    ClonePtr& operator=(const ClonePtr& other)
    {
        this->ptr = other.ptr->Clone();
        return *this;
    }

    typename Ptr::element_type* operator->() const
    {
        return this->ptr.get();
    }

    typename Ptr::element_type* get() const
    {
        return this->ptr.get();
    }

    typename const Ptr& getPtr() const
    {
        return this->ptr;
    }

    typename Ptr& getPtr()
    {
        return this->ptr;
    }


private:
    Ptr ptr;
};
