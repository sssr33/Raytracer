#pragma once

#include <cstdint>

class RasterScanBox {
public:
	struct TriangleParams {
		float x1 = 0.f;
		float y1 = 0.f;
		float x2 = 0.f;
		float y2 = 0.f;
		float x3 = 0.f;
		float y3 = 0.f;
		float minClipX = 0.f;
		float minClipY = 0.f;
		float maxClipX = 0.f;
		float maxClipY = 0.f;
		uint32_t videoMemoryWidth = 0;
		uint32_t videoMemoryHeight = 0;
	};

	struct BoxParams {
		uint32_t startX = 0;
		uint32_t startY = 0;
		uint32_t endX = 0;
		uint32_t endY = 0;
	};

	RasterScanBox() = default;
	RasterScanBox(const TriangleParams& params);
	RasterScanBox(const BoxParams& params);

	template<class Fn = void(uint32_t y, uint32_t startX, uint32_t endX)>
	void Scan(Fn fn) const;

private:
	static BoxParams MakeBoxParams(const TriangleParams& triParams);

	uint32_t startX = 0;
	uint32_t startY = 0;
	uint32_t endX = 0;
	uint32_t endY = 0;
};

#include "RasterScanBox.hpp"
