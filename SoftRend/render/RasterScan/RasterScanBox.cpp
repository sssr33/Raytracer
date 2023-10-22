#include "RasterScanBox.h"

#include <algorithm>
#include <cmath>

RasterScanBox::RasterScanBox(const TriangleParams& params)
    : RasterScanBox(RasterScanBox::MakeBoxParams(params))
{}

RasterScanBox::RasterScanBox(const BoxParams& params)
    : startX(params.startX)
    , startY(params.startY)
    , endX(params.endX)
    , endY(params.endY)
{}

RasterScanBox::BoxParams RasterScanBox::MakeBoxParams(const TriangleParams& triParams) {
    BoxParams boxParams;

	// triangle bounding box
	float minX = (std::min)((std::min)(triParams.x1, triParams.x2), triParams.x3);
	float minY = (std::min)((std::min)(triParams.y1, triParams.y2), triParams.y3);
	float maxX = (std::max)((std::max)(triParams.x1, triParams.x2), triParams.x3);
	float maxY = (std::max)((std::max)(triParams.y1, triParams.y2), triParams.y3);

	// limit by clip borders
	minX = (std::max)(minX, triParams.minClipX);
	minY = (std::max)(minY, triParams.minClipY);
	maxX = (std::min)(maxX, triParams.maxClipX);
	maxY = (std::min)(maxY, triParams.maxClipY);

	// limit by physical size of video memory buffer
	minX = (std::max)(minX, 0.f);
	minY = (std::max)(minY, 0.f);
	maxX = (std::min)(maxX, static_cast<float>(triParams.videoMemoryWidth));
	maxY = (std::min)(maxY, static_cast<float>(triParams.videoMemoryHeight));

	boxParams.startX = static_cast<uint32_t>(minX);
	boxParams.startY = static_cast<uint32_t>(minY);
	boxParams.endX = static_cast<uint32_t>(std::ceil(maxX));
	boxParams.endY = static_cast<uint32_t>(std::ceil(maxY));

    return boxParams;
}
