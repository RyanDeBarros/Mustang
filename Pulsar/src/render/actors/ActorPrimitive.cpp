#include "ActorPrimitive.h"

#include "Logger.h"
#include "render/CanvasLayer.h"

ActorPrimitive2D::ActorPrimitive2D(const Renderable& render, const Transform2D& transform, const ZIndex& z, const bool& visible)
	: m_Render(render), m_Transform(transform), m_Z(z), m_Status(visible ? 0b111 : 0b110)
{
}

ActorPrimitive2D::ActorPrimitive2D(const ActorPrimitive2D& primitive)
	: m_Render(primitive.m_Render), m_Transform(primitive.m_Transform), m_Z(primitive.m_Z), m_Status(primitive.m_Status), m_ModulationColors(primitive.m_ModulationColors)
{
}

void ActorPrimitive2D::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawPrimitive(*this);
}

void ActorPrimitive2D::OnDraw(signed char texture_slot)
{
	if (!m_Render.vertexBufferData)
		return;
	Stride stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	// update TextureSlot
	if (m_Render.vertexBufferData[0] != texture_slot)
	{
		for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
			m_Render.vertexBufferData[i * stride	] = static_cast<GLfloat>(texture_slot);
	}
	// update TransformP
	if ((m_Status & 0b10) == 0b10)
	{
		m_Status &= ~0b10;
		for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
		{
			m_Render.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(m_Transform.position.x);
			m_Render.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(m_Transform.position.y);
		}
	}
	// update TransformRS
	if ((m_Status & 0b100) == 0b100)
	{
		m_Status &= ~0b100;
		glm::mat2 condensed_rs_matrix = Transform::CondensedRS(m_Transform);
		for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
		{
			m_Render.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
			m_Render.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
			m_Render.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
			m_Render.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
		}
	}
	// update ModulationColor
	if ((m_Status & 0b1000) == 0b1000)
	{
		m_Status &= ~0b1000;
		for (BufferCounter i = 0; i < m_Render.vertexCount && i < m_ModulationColors.size(); i++)
		{
			m_Render.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(m_ModulationColors[i][0]);
			m_Render.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(m_ModulationColors[i][1]);
			m_Render.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(m_ModulationColors[i][2]);
			m_Render.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(m_ModulationColors[i][3]);
		}
	}
}

void ActorPrimitive2D::CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	for (size_t i = 0; i < points.size() && i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = points[i][0] / atlas_width;
		m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = points[i][1] / atlas_height;
	}
}

void ActorPrimitive2D::CropRelativePoints(const std::vector<glm::vec2>& atlas_points)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	for (size_t i = 0; i < atlas_points.size() && i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = atlas_points[i][0];
		m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = atlas_points[i][1];
	}
}
