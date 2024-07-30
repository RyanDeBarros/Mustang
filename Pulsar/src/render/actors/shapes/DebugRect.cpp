#include "DebugRect.h"

DebugRect::DebugRect(float width, float height, bool filled_in, const glm::vec2& pivot, const Transform2D& transform, const glm::vec4& color, ZIndex z)
	: DebugPolygon({ { -pivot.x * width, -pivot.y * height }, { (1 - pivot.x) * width, -pivot.y * height }, { (1 - pivot.x) * width, (1 - pivot.y) * height }, { -pivot.x * width, (1 - pivot.y) * height } },
		transform, color, filled_in ? GL_TRIANGLE_FAN : GL_LINE_LOOP, z), m_Width(width), m_Height(height), m_Pivot(pivot), m_Filled(filled_in)
{
}

DebugRect::DebugRect(const DebugRect& other)
	: DebugPolygon(other), m_Width(other.m_Width), m_Height(other.m_Height), m_Pivot(other.m_Pivot), m_Filled(other.m_Filled)
{
}

DebugRect::DebugRect(DebugRect&& other) noexcept
	: DebugPolygon(std::move(other)), m_Width(other.m_Width), m_Height(other.m_Height), m_Pivot(other.m_Pivot), m_Filled(other.m_Filled)
{
}

DebugRect& DebugRect::operator=(const DebugRect& other)
{
	DebugPolygon::operator=(other);
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_Pivot = other.m_Pivot;
	m_Filled = other.m_Filled;
	return *this;
}

DebugRect& DebugRect::operator=(DebugRect&& other) noexcept
{
	DebugPolygon::operator=(std::move(other));
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	m_Pivot = other.m_Pivot;
	m_Filled = other.m_Filled;
	return *this;
}

void DebugRect::SetWidth(float width)
{
	m_Width = width;
	Readjust();
}

void DebugRect::GetHeight(float height)
{
	m_Height = height;
	Readjust();
}

void DebugRect::SetFilled(bool filled)
{
	m_Filled = filled;
	DebugPolygon::SetIndexingMode(m_Filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
}

void DebugRect::SetPivot(const glm::vec2& pivot)
{
	m_Pivot = pivot;
	Readjust();
}

void DebugRect::Readjust()
{
	DebugPolygon::SetPointPosition(0, { -m_Pivot.x * m_Width, -m_Pivot.y * m_Height });
	DebugPolygon::SetPointPosition(1, { (1 - m_Pivot.x) * m_Width, -m_Pivot.y * m_Height });
	DebugPolygon::SetPointPosition(2, { (1 - m_Pivot.x) * m_Width, (1 - m_Pivot.y) * m_Height });
	DebugPolygon::SetPointPosition(3, { -m_Pivot.x * m_Width, (1 - m_Pivot.y) * m_Height });
}
