#include "DebugCircle.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "render/CanvasLayer.h"

DebugCircle::DebugCircle(const float& radius, const Transform2D& transform, const glm::vec4& color, const ZIndex& z)
	: DebugPolygon({}, transform, color, GL_TRIANGLE_FAN, z), m_Radius(2 * radius)
{
	loadRenderable(_RendererSettings::solid_circle_filepath.c_str(), m_Renderable);
	PointsRef() = { {m_Radius, m_Radius}, {-m_Radius, m_Radius}, {-m_Radius, -m_Radius}, {m_Radius, -m_Radius} };
	m_Status &= ~0b1;
}

void DebugCircle::SetRadius(const float& radius)
{
	m_Radius = 2 * radius;
	SetPointPosition(0, {  m_Radius,  m_Radius });
	SetPointPosition(1, { -m_Radius,  m_Radius });
	SetPointPosition(2, { -m_Radius, -m_Radius });
	SetPointPosition(3, {  m_Radius, -m_Radius });
}
