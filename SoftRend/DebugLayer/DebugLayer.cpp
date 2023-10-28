#include "DebugLayer.h"

#include <cmath>

DebugLayer& DebugLayer::Instance() {
    // do not delete to let it stay alive even while static objects destroyed
    // after program closed the OS will free all its memory
    static DebugLayer* instance = new DebugLayer();
    return *instance;
}

bool DebugLayer::IsEnabled() const {
    return this->layerEnabled;
}

void DebugLayer::SetEnabled(bool enabled) {
    this->layerEnabled = enabled;
}

uint32_t DebugLayer::GetWidth() const {
    return this->pixelInfoWidth;
}

uint32_t DebugLayer::GetHeight() const {
    return this->pixelInfoHeight;
}

void DebugLayer::ClearPixelInfo() {
    if (!this->IsEnabled()) {
        // clear can take long time and it makes big stutters
        return;
    }

    for (auto& pixInfo : this->pixelInfo) {
        pixInfo.triDrawn.clear();
        pixInfo.triDrawnHalfPlane.clear();
    }
}

void DebugLayer::ResizePiexlInfo(uint32_t width, uint32_t height) {
    if (this->pixelInfoWidth == width && this->pixelInfoHeight == height) {
        return;
    }

    this->pixelInfoWidth = width;
    this->pixelInfoHeight = height;

    if (!this->IsEnabled()) {
        // resize can take long time and it makes big stutters
        return;
    }

    this->pixelInfo.resize(this->pixelInfoWidth * this->pixelInfoHeight);

    for (uint32_t y = 0; y < this->pixelInfoHeight; ++y) {
        for (uint32_t x = 0; x < this->pixelInfoWidth; ++x) {
            auto& pixInfo = this->ModifyPixelInfo(x, y);
            pixInfo.pixelCoords.x = x;
            pixInfo.pixelCoords.y = y;
        }
    }
}

const DebugLayer::PixelInfo& DebugLayer::GetPixelInfo(uint32_t x, uint32_t y) const {
    size_t idx = this->GetPixelInfoIndex(x, y);
    return this->pixelInfo[idx];
}

void DebugLayer::AddTriangleRasterInfoForPixel(uint32_t x, uint32_t y, TriangleRasterInfo info) {
    auto& pixelInfo = this->ModifyPixelInfo(x, y);
    pixelInfo.triDrawn.push_back(std::move(info));
}

void DebugLayer::AddTriangleRasterInfoForPixel(uint32_t x, uint32_t y, TriangleRasterHalfPlaneInfo info) {
    auto& pixelInfo = this->ModifyPixelInfo(x, y);
    pixelInfo.triDrawnHalfPlane.push_back(std::move(info));
}

const DebugLayer::TriangleRasterInfo& DebugLayer::GetCurrentTriangleRasterInfo() const {
    return this->curTriRasterInfo;
}

void DebugLayer::SetCurrentTriangleRasterInfo(const TriangleRasterInfo& info) {
    this->curTriRasterInfo = info;
}

DebugLayer::PixelInfo& DebugLayer::ModifyPixelInfo(uint32_t x, uint32_t y) {
    size_t idx = this->GetPixelInfoIndex(x, y);
    return this->pixelInfo[idx];
}

size_t DebugLayer::GetPixelInfoIndex(uint32_t x, uint32_t y) const {
    size_t idx = y * this->pixelInfoWidth + x;
    return idx;
}

bool Point2D::operator==(const Point2D& other) const {
    bool same =
        this->x == other.x &&
        this->y == other.y;

    return same;
}

bool Point2D::operator!=(const Point2D& other) const {
    return !this->operator==(other);
}

Point2D Point2D::operator-(const Point2D& other) const {
    Point2D res;

    res.x = this->x - other.x;
    res.y = this->y - other.y;

    return res;
}

Point2D Point2D::Normalized() const {
    Point2D res;
    const float length = this->Length();

    res.x = this->x / length;
    res.y = this->y / length;

    return res;
}

float Point2D::Length() const {
    float lengthSq = this->Dot(*this);
    float length = std::sqrt(lengthSq);

    return length;
}

float Point2D::Dot(const Point2D& other) const {
    float dp = this->x * other.x + this->y * other.y;
    return dp;
}

Point2D Point2D::Rotated90CW() const {
    Point2D res;

    res.x = this->y;
    res.y = -this->x;

    return res;
}

Point2D Point2D::Rotated90CWScreenSpace() const {
    Point2D res;

    res.x = -this->y;
    res.y = this->x;

    return res;
}

Point2D Point2D::Rotated90CCWScreenSpace() const {
    Point2D res;

    res.x = this->y;
    res.y = -this->x;

    return res;
}
