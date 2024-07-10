#include "DebugPoint.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "render/CanvasLayer.h"

DebugPoint::DebugPoint(const glm::vec2& position, const glm::vec4& outer_color, const float& diameter, const float& inner_radius, const glm::vec4& inner_color, const ZIndex& z)
	: DebugPolygon({{ 0.0f, 0.0f }}, {position}, outer_color, GL_POINTS, z), m_Diameter(diameter), m_InnerRadius(inner_radius), m_InnerColor(inner_color)
{
	loadRenderable(_RendererSettings::solid_point_filepath.c_str(), m_Renderable);
}

void DebugPoint::RequestDraw(CanvasLayer* canvas_layer)
{
	if (visible)
	{
		DebugPolygon::CheckStatus();
		DebugPoint::CheckStatus();
		canvas_layer->DrawArray(m_Renderable, m_IndexingMode);
	}
}

void DebugPoint::CheckStatus()
{
	if ((m_PointStatus & 0b1) == 0b1)
	{
		m_PointStatus &= ~0b1;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 12] = static_cast<GLfloat>(m_Diameter);
		}
	}
	if ((m_PointStatus & 0b10) == 0b10)
	{
		m_PointStatus &= ~0b10;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 13] = static_cast<GLfloat>(m_InnerRadius);
		}
	}
	if ((m_PointStatus & 0b100) == 0b100)
	{
		m_PointStatus &= ~0b100;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 14] = static_cast<GLfloat>(m_InnerColor[0]);
			m_Renderable.vertexBufferData[i * stride + 15] = static_cast<GLfloat>(m_InnerColor[1]);
			m_Renderable.vertexBufferData[i * stride + 16] = static_cast<GLfloat>(m_InnerColor[2]);
			m_Renderable.vertexBufferData[i * stride + 17] = static_cast<GLfloat>(m_InnerColor[3]);
		}
	}
}
