#pragma once

#include "DebugPolygon.h"

class DebugPoint : public DebugPolygon
{
	float m_PointSize;
	float m_InnerRadius;
	glm::vec4 m_InnerColor;

	// 0b ... inner color | inner radius | point size
	unsigned char m_CircleStatus = 0b111;

public:
	DebugPoint(const Transform2D& transform, const glm::vec4& outer_color = { 1.0f, 1.0f, 1.0f, 1.0f }, const float& point_size = 2.0f, const float& inner_radius = 0.5f, const glm::vec4& inner_color = { 0.0f, 0.0f, 0.0f, 0.0f }, const ZIndex& z = 0);

	void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline float GetPointSize() const { return m_PointSize; }
	inline void SetPointSize(const float& point_size) { m_PointSize = point_size; m_CircleStatus |= 0b1; }
	inline float GetInnerRadius() const { return m_InnerRadius; }
	inline void SetInnerRadius(const float& inner_radius) { m_InnerRadius = inner_radius; m_CircleStatus |= 0b10; }
	inline glm::vec4 GetInnerColor() const { return m_InnerColor; }
	inline void SetInnerColor(const glm::vec4& inner_color) { m_InnerColor = inner_color; m_CircleStatus |= 0b100; }

	inline static float PointSizeFromRadius(const float& radius) { return 5.0f * radius; }
	inline static float InnerRadiusFromBWR(const float& border_width, const float& radius) { return 0.5f - 0.5f * border_width / radius; }
	inline float InnerRadiusFromBorderWidth(const float& border_width) const { return 0.5f - 2.5f * border_width / m_PointSize; }

private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;

protected:
	virtual void CheckStatus() override;
};
