#pragma once

#include "DebugPolygon.h"

class DebugRect : public DebugPolygon
{
	float m_Width, m_Height;
	glm::vec2 m_Pivot;
	bool m_Filled;

public:
	DebugRect(float width, float height, bool filled_in = false, const glm::vec2& pivot = { 0.5f, 0.5f }, const Transform2D& transform = {}, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, ZIndex z = 0);
	DebugRect(const DebugRect&);
	DebugRect(DebugRect&&) noexcept;
	DebugRect& operator=(const DebugRect&);
	DebugRect& operator=(DebugRect&&) noexcept;

	inline float GetWidth() const { return m_Width; }
	inline float GetHeight() const { return m_Height; }
	inline bool IsFilled() const { return m_Filled; }
	inline glm::vec2 GetPivot() const { return m_Pivot; }
	void SetWidth(float width);
	void GetHeight(float height);
	void SetFilled(bool filled);
	void SetPivot(const glm::vec2& pivot);

private:
	using DebugPolygon::GetIndexingMode;
	using DebugPolygon::SetIndexingMode;
	using DebugPolygon::PointsRef;

	void Readjust();
};
