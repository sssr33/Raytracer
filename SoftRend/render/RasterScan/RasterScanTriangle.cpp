#include "RasterScanTriangle.h"
#include "DebugLayer\DebugLayer.h"

#include <algorithm>

RasterScanTriangle::RasterScanTriangle(const TriangleParams& params)
    : params(params)
{
    Triangle2D tri;

    tri.a.x = params.x1;
    tri.a.y = params.y1;
    tri.b.x = params.x2;
    tri.b.y = params.y2;
    tri.c.x = params.x3;
    tri.c.y = params.y3;

    Screen2D screen;

    screen.min.x = (std::max)(params.minClipX, 0.f);
    screen.min.y = (std::max)(params.minClipY, 0.f);
    screen.max.x = (std::min)(params.maxClipX, static_cast<float>(params.videoMemoryWidth));
    screen.max.y = (std::min)(params.maxClipY, static_cast<float>(params.videoMemoryHeight));

    // all logic below can be optimized if tri is stays as triangle
    // scanRanges can be created by splitting triangle on Top and Bottom triangle
    auto clipped = this->clip.Clip(tri, screen);

    if (clipped.size() != 3) {
        int stop = 234;
    }
    if (clipped.size() == 0) {
        int stop = 234;
    }

    std::vector<ScanEdge> scanEdges;

    const POINT2D* a = &clipped.back();
    for (const auto& b : clipped) {
        if (a->y == b.y) {
            a = &b; // TODO add scope
            continue;
        }

        auto sortSwap = [](float& x1, float& y1, float& x2, float& y2)
        {
            bool doSwap = false;

            if (x1 > x2) {
                doSwap = true;
            }
            else if (x1 == x2 && y1 > y2) {
                doSwap = true;
            }

            if (doSwap) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }

            return doSwap;
        };

        bool cw = true;

        auto aTmp = *a;
        auto bTmp = b;

        if (sortSwap(aTmp.x, aTmp.y, bTmp.x, bTmp.y)) {
            cw = false;
        }

        auto plane = HalfPlane::BuildFromPoints(aTmp.x, aTmp.y, bTmp.x, bTmp.y, cw);

        if (a->y < b.y) {
            scanEdges.push_back(ScanEdge(*a, b, plane.isLeftTop));
        }
        else {
            scanEdges.push_back(ScanEdge(b, *a, plane.isLeftTop));
        }

        a = &b; // TODO add scope
    }

    if (scanEdges.empty()) {
        return;
    }

    std::sort(std::begin(scanEdges), std::end(scanEdges),
        [](const ScanEdge& a, const ScanEdge& b)
        {
            // A is top point
            return a.GetPointA().y < b.GetPointA().y;
        });

    auto getNextLeft = [&](auto it)
    {
        if (it == std::end(scanEdges)) {
            return std::end(scanEdges);
        }

        auto nextIt = std::find_if(it, std::end(scanEdges),
            [](const ScanEdge& i)
            {
                return i.IsLeftSide();
            });

        return nextIt;
    };
    auto getNextRight = [&](auto it)
    {
        if (it == std::end(scanEdges)) {
            return std::end(scanEdges);
        }

        auto nextIt = std::find_if(it, std::end(scanEdges),
            [](const ScanEdge& i)
            {
                return !i.IsLeftSide();
            });

        return nextIt;
    };

    auto itLeft = getNextLeft(std::begin(scanEdges));
    auto itRight = getNextRight(std::begin(scanEdges));
    std::vector<ScanRange> scanRange;

    while (itLeft != std::end(scanEdges) && itRight != std::end(scanEdges)) {
        float startY = (std::max)(itLeft->GetPointA().y, itRight->GetPointA().y);
        float endY = (std::min)(itLeft->GetPointB().y, itRight->GetPointB().y);

        ScanRange range = { *itLeft, *itRight, startY, endY };
        scanRange.push_back(range);

        auto nextLeft = getNextLeft(itLeft + 1);
        auto nextRight = getNextRight(itRight + 1);

        if (nextLeft != std::end(scanEdges) && nextRight != std::end(scanEdges)) {
            if (nextLeft->GetPointA().y < nextRight->GetPointA().y) {
                itLeft = nextLeft;
            }
            else {
                itRight = nextRight;
            }
        }
        else if (nextLeft != std::end(scanEdges)) {
            itLeft = nextLeft;
        }
        else if (nextRight != std::end(scanEdges)) {
            itRight = nextRight;
        }
        else {
            // both end
            break;
        }
    }

    this->scanRanges = std::move(scanRange);
}

RasterScanTriangle::ScanEdge::ScanEdge(const POINT2D& a, const POINT2D& b, bool isLeftSide)
    : a(a)
    , b(b)
    , isLeftSide(isLeftSide)
{}

const POINT2D& RasterScanTriangle::ScanEdge::GetPointA() const {
    return this->a;
}

const POINT2D& RasterScanTriangle::ScanEdge::GetPointB() const {
    return this->b;
}

bool RasterScanTriangle::ScanEdge::IsLeftSide() const {
    return this->isLeftSide;
}

float RasterScanTriangle::ScanRange::LerpXLeft(float y) const {
    return ScanRange::LerpXEdge(y, this->leftEdge);
}

float RasterScanTriangle::ScanRange::LerpXRight(float y) const {
    return ScanRange::LerpXEdge(y, this->rightEdge);
}

float RasterScanTriangle::ScanRange::LerpXEdge(float y, const ScanEdge& edge) {
    const auto& a = edge.GetPointA();
    const auto& b = edge.GetPointB();

    float t = (y - a.y) / (b.y - a.y);
    float x = a.x + t * (b.x - a.x);

    return x;
}
