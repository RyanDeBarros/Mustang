#pragma once

#include "DebugPolygon.h"

class DebugCircle : public DebugPolygon
{
	float m_Diameter;

public:
	DebugCircle(float radius = 1.0f, ZIndex z = 0, FickleType fickle_type = FickleType::Protean);
	DebugCircle(const DebugCircle&);
	DebugCircle(DebugCircle&&) noexcept;
	DebugCircle& operator=(const DebugCircle&);
	DebugCircle& operator=(DebugCircle&&) noexcept;

	float GetDiameter() const { return m_Diameter; }
	void SetRadius(float radius);
	
private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;
};
