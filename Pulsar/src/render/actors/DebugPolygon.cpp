#include "DebugPolygon.h"

#include "render/CanvasLayer.h"
#include "AssetLoader.h"

DebugPolygon::DebugPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color, const GLenum& indexing_mode, const ZIndex& z)
	: m_Z(z), m_Color(color)
{
	loadRenderable(_RendererSettings::solid_polygon_filepath.c_str(), m_Renderable);
	SetIndexingMode(indexing_mode);
	PointsRef() = points;
}

void DebugPolygon::RequestDraw(CanvasLayer* canvas_layer)
{
	if (!visible)
		return;
	// allocate vertex buffer
	if ((m_Status & 0b1) == 0b1)
	{
		m_Status = ~0b1;
		m_Renderable.vertexCount = static_cast<BufferCounter>(m_Points.size());
		if (m_Renderable.vertexBufferData)
			delete[] m_Renderable.vertexBufferData;
		m_Renderable.vertexBufferData = new GLfloat[Render::VertexBufferLayoutCount(m_Renderable)];
	}
	ASSERT(m_Points.size() == m_Renderable.vertexCount);
	// modify color of vertices
	if ((m_Status & 0b10) == 0b10)
	{
		m_Status &= ~0b10;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(m_Color[0]);
			m_Renderable.vertexBufferData[i * stride + 7] = static_cast<GLfloat>(m_Color[1]);
			m_Renderable.vertexBufferData[i * stride + 8] = static_cast<GLfloat>(m_Color[2]);
			m_Renderable.vertexBufferData[i * stride + 9] = static_cast<GLfloat>(m_Color[3]);
		}
	}
	// modify point positions
	if ((m_Status & 0b100) == 0b100)
	{
		m_Status &= ~0b100;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(m_Points[i][0]);
			m_Renderable.vertexBufferData[i * stride + 11] = static_cast<GLfloat>(m_Points[i][1]);
		}
	}
	// update TransformP
	if ((m_Status & 0b1000) == 0b1000)
	{
		m_Status &= ~0b1000;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride	] = static_cast<GLfloat>(m_Transform.position.x);
			m_Renderable.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(m_Transform.position.y);
		}
	}
	// update TransformRS
	if ((m_Status & 0b10000) == 0b10000)
	{
		m_Status &= ~0b10000;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		glm::mat2 condensed_rs_matrix = Transform::CondensedRS(m_Transform);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
			m_Renderable.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
			m_Renderable.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
			m_Renderable.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
		}
	}
	canvas_layer->DrawArray(m_Renderable, m_IndexingMode);
}
