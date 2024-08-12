#pragma once

#include "DebugPolygon.h"

class DebugCircle : public DebugPolygon
{
	float m_Diameter;

public:
	DebugCircle(float radius = 1.0f, const Transform2D& transform = {}, const glm::vec4& color = Colors::WHITE, ZIndex z = 0);
	DebugCircle(const DebugCircle&);
	DebugCircle(DebugCircle&&) noexcept;
	DebugCircle& operator=(const DebugCircle&);
	DebugCircle& operator=(DebugCircle&&) noexcept;

	inline float GetDiameter() const { return m_Diameter; }
	void SetRadius(float radius);
	
private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;
};
