#pragma once
#include "IHitable.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

class BvhNode : public IHitable
{
public:
    BvhNode() = default;
    BvhNode(std::vector<std::unique_ptr<IHitable>> objects, float time0, float time1);

    std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;
    std::optional<aabb> GetBoundingBox(float time0, float time1) const override;

private:
    enum class SortingAxis
    {
        X, Y, Z
    };

    class EmptyNode : public IHitable
    {
    public:
        std::optional<HitRecord> Hit(const ray<float>& ray, float tMin, float tMax) const override;
        std::optional<aabb> GetBoundingBox(float time0, float time1) const override;
    };

    template<class It>
    void BuildBvhTree(It begin, It end, float time0, float time1);
    void InitBox(std::optional<aabb> leftBox, std::optional<aabb> rightBox);

    template<class It>
    static void SortSubTree(It begin, It end, float time0, float time1);
    template<class It, class AabbCmpFn>
    static bool HitableSort(It begin, It end, float time0, float time1, AabbCmpFn aabbCmpFn);

    // invalid aabb
    aabb box = aabb(vec3<float>(1.f), vec3<float>(-1.f));
    std::unique_ptr<IHitable> left;
    std::unique_ptr<IHitable> right;
};
