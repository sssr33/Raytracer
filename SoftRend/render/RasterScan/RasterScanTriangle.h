#pragma once
#include "..\Clip\SutherlandHodgmanTriangleClipper2D.h"

#include <cstdint>

class RasterScanTriangle {
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

	RasterScanTriangle() = default;
	RasterScanTriangle(const TriangleParams& params);

	template<class Fn = void(uint32_t y, uint32_t startX, uint32_t endX)>
	void Scan(Fn fn) const;

private:
	class ScanEdge {
	public:
		ScanEdge(const POINT2D& a, const POINT2D& b, bool isLeftSide);

		const POINT2D& GetPointA() const;
		const POINT2D& GetPointB() const;
		bool IsLeftSide() const;

	private:
		POINT2D a;
		POINT2D b;
		// true - left side, start
		// false - right side, end
		bool isLeftSide = true;
	};

	struct ScanRange {
		ScanEdge leftEdge;
		ScanEdge rightEdge;
		float startY = 0.f;
		float endY = 0.f;

		float LerpXLeft(float y) const;
		float LerpXRight(float y) const;

	private:
		static float LerpXEdge(float y, const ScanEdge& edge);
	};

	TriangleParams params;
	std::vector<ScanRange> scanRanges;
	SutherlandHodgmanTriangleClipper2D clip;
};

#include "RasterScanTriangle.hpp"
