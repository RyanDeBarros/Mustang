#include "DebugPoint.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "render/CanvasLayer.h"

DebugPoint::DebugPoint(const Transform2D& transform, const glm::vec4& outer_color, const float& point_size, const float& inner_radius, const glm::vec4& inner_color, const ZIndex& z)
	: DebugPolygon({{ 0.0f, 0.0f }}, outer_color, GL_POINTS, z), m_PointSize(point_size), m_InnerRadius(inner_radius), m_InnerColor(inner_color)
{
	SetTransform(transform);
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
	if ((m_CircleStatus & 0b1) == 0b1)
	{
		m_CircleStatus &= ~0b1;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 12] = static_cast<GLfloat>(m_PointSize);
		}
	}
	if ((m_CircleStatus & 0b10) == 0b10)
	{
		m_CircleStatus &= ~0b10;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 13] = static_cast<GLfloat>(m_InnerRadius);
		}
	}
	if ((m_CircleStatus & 0b100) == 0b100)
	{
		m_CircleStatus &= ~0b100;
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
