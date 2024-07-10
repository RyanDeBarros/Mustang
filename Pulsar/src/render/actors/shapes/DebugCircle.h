#pragma once

#include "DebugPolygon.h"

class DebugCircle : public DebugPolygon
{
	float m_Radius;

public:
	DebugCircle(const float& radius = 1.0f, const Transform2D& transform = {}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, const ZIndex& z = 0);

	inline float GetRadius() const { return m_Radius; }
	void SetRadius(const float& radius);
	
private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;
};
