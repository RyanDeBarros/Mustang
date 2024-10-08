#include "DebugPoint.h"

#include "PulsarSettings.h"
#include "AssetLoader.h"
#include "render/CanvasLayer.h"

DebugPoint::DebugPoint(float diameter, float inner_radius, const glm::vec4& inner_color, ZIndex z, FickleType fickle_type)
	: DebugPolygon({{ 0.0f, 0.0f }}, GL_POINTS, z, fickle_type), m_Diameter(diameter), m_InnerRadius(inner_radius), m_InnerColor(inner_color)
{
	Loader::loadRenderable(PulsarSettings::solid_point_filepath(), m_Renderable);
}

DebugPoint::DebugPoint(const DebugPoint& other)
	: DebugPolygon(other), m_Diameter(other.m_Diameter), m_InnerRadius(other.m_InnerRadius), m_InnerColor(other.m_InnerColor), m_PointStatus(other.m_PointStatus)
{
}

DebugPoint::DebugPoint(DebugPoint&& other) noexcept
	: DebugPolygon(std::move(other)), m_Diameter(other.m_Diameter), m_InnerRadius(other.m_InnerRadius), m_InnerColor(other.m_InnerColor), m_PointStatus(other.m_PointStatus)
{
}

DebugPoint& DebugPoint::operator=(const DebugPoint& other)
{
	if (this == &other)
		return *this;
	DebugPolygon::operator=(other);
	m_Diameter = other.m_Diameter;
	m_InnerRadius = other.m_InnerRadius;
	m_InnerColor = other.m_InnerColor;
	m_PointStatus = other.m_PointStatus;
	return *this;
}

DebugPoint& DebugPoint::operator=(DebugPoint&& other) noexcept
{
	if (this == &other)
		return *this;
	DebugPolygon::operator=(std::move(other));
	m_Diameter = other.m_Diameter;
	m_InnerRadius = other.m_InnerRadius;
	m_InnerColor = other.m_InnerColor;
	m_PointStatus = other.m_PointStatus;
	return *this;
}

bool DebugPoint::DrawPrep()
{
	if (visible)
	{
		DebugPolygon::CheckStatus();
		DebugPoint::CheckStatus();
		return true;
	}
	return false;
}

void DebugPoint::CheckStatus()
{
	if (m_PointStatus & 0b1)
	{
		m_PointStatus &= ~0b1;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (VertexBufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 12] = static_cast<GLfloat>(m_Diameter);
		}
	}
	if (m_PointStatus & 0b10)
	{
		m_PointStatus &= ~0b10;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (VertexBufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 13] = static_cast<GLfloat>(m_InnerRadius);
		}
	}
	if (m_PointStatus & 0b100)
	{
		m_PointStatus &= ~0b100;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (VertexBufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 14] = static_cast<GLfloat>(m_InnerColor[0]);
			m_Renderable.vertexBufferData[i * stride + 15] = static_cast<GLfloat>(m_InnerColor[1]);
			m_Renderable.vertexBufferData[i * stride + 16] = static_cast<GLfloat>(m_InnerColor[2]);
			m_Renderable.vertexBufferData[i * stride + 17] = static_cast<GLfloat>(m_InnerColor[3]);
		}
	}
}
