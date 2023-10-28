#pragma once

#include <vector>
#include <cstdint>
#include <variant>

struct Point2D {
    float x = 0.f;
    float y = 0.f;

    bool operator==(const Point2D& other) const;
    bool operator!=(const Point2D& other) const;

    Point2D operator-(const Point2D& other) const;

    Point2D Normalized() const;
    float Length() const;
    float Dot(const Point2D& other) const;
    Point2D Rotated90CW() const;
    Point2D Rotated90CWScreenSpace() const;
    Point2D Rotated90CCWScreenSpace() const;
};

struct HalfPlane {
    Point2D point;
    Point2D vector;
    bool isLeftTop = false;

    bool IsInside(const Point2D& other) const {
        // no normalization for better precision because of rounding errors
        auto v2 = other - this->point;
        float dp = this->vector.Dot(v2);

        /*if (v2.Length() == 0.f) {
            dp = 0.f;
        }*/

        bool inside = false;
        if (isLeftTop) {
            inside = dp >= 0.f;
        }
        else {
            inside = dp > 0.f;
        }

        return inside;
    }

    static HalfPlane BuildFromPoints(float startX, float startY, float endX, float endY, bool cw) {
        HalfPlane res;

        res.point.x = startX;
        res.point.y = startY;
        res.vector.x = endX;
        res.vector.y = endY;

        res.vector = res.vector - res.point;
        if (cw) {
            // no normalization for better precision because of rounding errors
            res.vector = res.vector.Rotated90CWScreenSpace();
        }
        else {
            // no normalization for better precision because of rounding errors
            res.vector = res.vector.Rotated90CCWScreenSpace();
        }

        // vector look inside triangle
        // screen space
        if (res.vector.x > 0.f) {
            res.isLeftTop = true;
        }
        else if (res.vector.x == 0.f && res.vector.y < 0.f) {
            res.isLeftTop = true;
        }
        else {
            res.isLeftTop = false;
        }

        return res;
    }

    static HalfPlane BuildFromPoints(const Point2D& start, const Point2D& end, bool cw) {
        return BuildFromPoints(start.x, start.y, end.x, end.y, cw);
    }
};

class DebugLayer {
public:
    struct Point2DI {
        int32_t x = 0;
        int32_t y = 0;
    };

    struct TriangleRasterInfo {
        Point2D vertices[3];
        int polyIdx = -1;

        bool isTopTriangle = false;

        float x = 0.f;
        float xStart = 0.f; // x13
        float xEnd = 0.f; // top x23, bottom x12
        float xstepxStart = 0.f; // xstep13
        float xstepEnd = 0.f; // top xstep23, bottom xstep12
        float xStartScreenClamp = 0.f;
        float xEndScreenClamp = 0.f;
        float ixStartScreenClamp = 0.f;
        float ixEndScreenClamp = 0.f;
        float startCenterX = 0.f;
        float endCenterX = 0.f;

        float y = 0.f;
        float yStart = 0.f;
        float yEnd = 0.f;
        float startCenterY = 0.f;
        float endCenterY = 0.f;
    };

    struct TriangleRasterHalfPlaneInfo {
        Point2D vertices[3];
        Point2D rasterPt;

        HalfPlane planeA;
        HalfPlane planeB;
        HalfPlane planeC;
    };

    struct PixelInfo {
        // what coords of this pixel
        Point2DI pixelCoords;
        // what triangles drawn in this pixel
        std::vector<TriangleRasterInfo> triDrawn;

        std::vector<TriangleRasterHalfPlaneInfo> triDrawnHalfPlane;
    };

    static DebugLayer& Instance();

    bool IsEnabled() const;
    void SetEnabled(bool enabled);

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

    void ClearPixelInfo();
    void ResizePiexlInfo(uint32_t width, uint32_t height);

    const PixelInfo& GetPixelInfo(uint32_t x, uint32_t y) const;

    void AddTriangleRasterInfoForPixel(uint32_t x, uint32_t y, TriangleRasterInfo info);

    void AddTriangleRasterInfoForPixel(uint32_t x, uint32_t y, TriangleRasterHalfPlaneInfo info);

    const TriangleRasterInfo& GetCurrentTriangleRasterInfo() const;
    void SetCurrentTriangleRasterInfo(const TriangleRasterInfo& info);

private:
    DebugLayer() = default;
    DebugLayer(const DebugLayer&) = delete;
    DebugLayer(DebugLayer&&) = delete;

    PixelInfo& ModifyPixelInfo(uint32_t x, uint32_t y);
    size_t GetPixelInfoIndex(uint32_t x, uint32_t y) const;

    bool layerEnabled = false;

    uint32_t pixelInfoWidth = 0;
    uint32_t pixelInfoHeight = 0;
    std::vector<PixelInfo> pixelInfo;
    TriangleRasterInfo curTriRasterInfo;
};
