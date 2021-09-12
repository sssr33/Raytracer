#include "BvhNode.h"
#include "Render/Random/RandomFunctions.h"

#include <stdexcept>

BvhNode::BvhNode(std::vector<std::unique_ptr<IHitable>> objects, float time0, float time1)
{
    BuildBvhTree(objects.begin(), objects.end(), time0, time1);
}

std::optional<HitRecord> BvhNode::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    if (!this->box.Hit(ray, tMin, tMax))
    {
        return std::nullopt;
    }

    std::optional<HitRecord> leftHit = this->left->Hit(ray, tMin, tMax);
    std::optional<HitRecord> rightHit = this->right->Hit(ray, tMin, tMax);

    if (leftHit && rightHit)
    {
        if (leftHit->rayT < rightHit->rayT)
        {
            return leftHit;
        }

        return rightHit;
    }
    else if (leftHit)
    {
        return leftHit;
    }
    else if (rightHit)
    {
        return rightHit;
    }

    return std::nullopt;
}

std::optional<aabb> BvhNode::GetBoundingBox(float time0, float time1) const
{
    return this->box;
}

template<class It>
void BvhNode::BuildBvhTree(It begin, It end, float time0, float time1)
{
    BvhNode::SortSubTree(begin, end, time0, time1);

    std::ptrdiff_t elementCount = std::distance(begin, end);

    switch (elementCount)
    {
    case 1:
    {
        this->left = std::move(*begin);
        this->right = std::make_unique<EmptyNode>();

        std::optional<aabb> leftBox = this->left->GetBoundingBox(time0, time1);

        // right is empty, so use only left box and exit/return early
        InitBox(leftBox, leftBox);
        return;
    }
    case 2:
        this->left = std::move(*begin);
        this->right = std::move(*end);
        break;
    default:
        if (elementCount <= 0)
        {
            throw std::exception("BvhNode: elementCount <= 0");
        }

        It middle = begin;

        std::advance(middle, elementCount / 2);

        auto leftBvh = std::make_unique<BvhNode>();
        auto rightBvh = std::make_unique<BvhNode>();

        leftBvh->BuildBvhTree(begin, middle, time0, time1);
        rightBvh->BuildBvhTree(middle, end, time0, time1);

        this->left = std::move(leftBvh);
        this->right = std::move(rightBvh);
    }

    InitBox(
        this->left->GetBoundingBox(time0, time1),
        this->right->GetBoundingBox(time0, time1)
    );
}

void BvhNode::InitBox(std::optional<aabb> leftBox, std::optional<aabb> rightBox)
{
    if (!leftBox || !rightBox)
    {
        throw std::exception("BvhNode: no bounding box in constructor");
    }

    this->box = leftBox->United(*rightBox);
}

template<class It>
void BvhNode::SortSubTree(It begin, It end, float time0, float time1)
{
    SortingAxis axis = static_cast<SortingAxis>(3.f * RandomFloat());

    switch (axis)
    {
    case BvhNode::SortingAxis::X:
        BvhNode::HitableSort(begin, end, time0, time1, [](const aabb& left, const aabb& right)
            {
                return left.min.x < right.min.x;
            });
        break;
    case BvhNode::SortingAxis::Y:
        BvhNode::HitableSort(begin, end, time0, time1, [](const aabb& left, const aabb& right)
            {
                return left.min.y < right.min.y;
            });
        break;
    case BvhNode::SortingAxis::Z:
        BvhNode::HitableSort(begin, end, time0, time1, [](const aabb& left, const aabb& right)
            {
                return left.min.z < right.min.z;
            });
        break;
    default:
        throw std::exception("BvhNode: invalid axis");
    }
}

template<class It, class AabbCmpFn>
bool BvhNode::HitableSort(It begin, It end, float time0, float time1, AabbCmpFn aabbCmpFn)
{
    std::sort(begin, end, [&aabbCmpFn, time0, time1](const std::unique_ptr<IHitable>& left, const std::unique_ptr<IHitable>& right)
        {
            std::optional<aabb> leftBox = left->GetBoundingBox(time0, time1);
            std::optional<aabb> rightBox = right->GetBoundingBox(time0, time1);

            if (!leftBox || !rightBox)
            {
                throw std::exception("BvhNode: no bounding box in HitableSort");
            }

            return aabbCmpFn(*leftBox, *rightBox);
        });
}

std::optional<HitRecord> BvhNode::EmptyNode::Hit(const ray<float>& ray, float tMin, float tMax) const
{
    return std::nullopt;
}

std::optional<aabb> BvhNode::EmptyNode::GetBoundingBox(float time0, float time1) const
{
    return std::nullopt;
}
