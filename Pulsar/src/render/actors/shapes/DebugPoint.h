#pragma once

#include "DebugPolygon.h"

class DebugPoint : public DebugPolygon
{
	float m_Diameter;
	float m_InnerRadius;
	glm::vec4 m_InnerColor;

	// 0b ... inner color | inner radius | point size
	unsigned char m_PointStatus = 0b111;

public:
	DebugPoint(float diameter = 2.0f, float inner_radius = 0.0f, const glm::vec4& inner_color = { 0.0f, 0.0f, 0.0f, 0.0f }, ZIndex z = 0, FickleType fickle_type = FickleType::Protean);
	DebugPoint(const DebugPoint&);
	DebugPoint(DebugPoint&&) noexcept;
	DebugPoint& operator=(const DebugPoint&);
	DebugPoint& operator=(DebugPoint&&) noexcept;

	virtual bool DrawPrep() override;

	float GetDiameter() const { return m_Diameter; }
	void SetDiameter(float point_size) { m_Diameter = point_size; m_PointStatus |= 0b1; }
	float GetInnerRadius() const { return m_InnerRadius; }
	void SetInnerRadius(float inner_radius) { m_InnerRadius = inner_radius; m_PointStatus |= 0b10; }
	glm::vec4 GetInnerColor() const { return m_InnerColor; }
	void SetInnerColor(const glm::vec4& inner_color) { m_InnerColor = inner_color; m_PointStatus |= 0b100; }

	static float InnerRadiusFromBorderWidth(float border_width, float diameter_inv) { return 0.5f - border_width * diameter_inv; }
	float InnerRadiusFromBorderWidth(float border_width) const { return 0.5f - border_width / m_Diameter; }

private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;

protected:
	virtual void CheckStatus() override;
};
