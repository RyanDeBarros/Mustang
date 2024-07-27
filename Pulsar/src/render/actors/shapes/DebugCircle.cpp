#include "DebugCircle.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "render/CanvasLayer.h"

DebugCircle::DebugCircle(float radius, const Transform2D& transform, const glm::vec4& color, ZIndex z)
	: DebugPolygon({}, transform, color, GL_TRIANGLE_FAN, z), m_Diameter(2 * radius)
{
	loadRenderable(_RendererSettings::solid_circle_filepath.c_str(), m_Renderable);
	PointsRef() = { {m_Diameter, m_Diameter}, {-m_Diameter, m_Diameter}, {-m_Diameter, -m_Diameter}, {m_Diameter, -m_Diameter} };
	m_Status &= ~0b1;
}

DebugCircle::DebugCircle(const DebugCircle& other)
	: DebugPolygon(other), m_Diameter(other.m_Diameter)
{
}

DebugCircle::DebugCircle(DebugCircle&& other) noexcept
	: DebugPolygon(std::move(other)), m_Diameter(other.m_Diameter)
{
}

DebugCircle& DebugCircle::operator=(const DebugCircle& other)
{
	m_Diameter = other.m_Diameter;
	return *this;
}

DebugCircle& DebugCircle::operator=(DebugCircle&& other) noexcept
{
	m_Diameter = other.m_Diameter;
	return *this;
}

void DebugCircle::SetRadius(float radius)
{
	m_Diameter = 2 * radius;
	SetPointPosition(0, {  m_Diameter,  m_Diameter });
	SetPointPosition(1, { -m_Diameter,  m_Diameter });
	SetPointPosition(2, { -m_Diameter, -m_Diameter });
	SetPointPosition(3, {  m_Diameter, -m_Diameter });
}
