#include "ActorPrimitive.h"

#include "Logger.h"
#include "render/CanvasLayer.h"

ActorPrimitive2D::ActorPrimitive2D(const Renderable& render, const Transform2D& transform, ZIndex z, bool visible)
	: m_Render(render), m_Transformer(transform, std::make_unique<ActorPrimitive2D_TN>(this)), m_Modulate(std::make_shared<PrimitiveModulatable>()), ActorRenderBase2D(z), m_Status(visible ? 0b111 : 0b110)
{
	m_Modulate->m_Primitive = this;
}

ActorPrimitive2D::ActorPrimitive2D(const ActorPrimitive2D& primitive)
	: m_Render(primitive.m_Render), m_Transformer(primitive.m_Transformer), m_Modulate(primitive.m_Modulate), ActorRenderBase2D(primitive), m_Status(primitive.m_Status), m_ModulationColors(primitive.m_ModulationColors)
{
	m_Transformer.notify = std::make_unique<ActorPrimitive2D_TN>(this);
	m_Modulate->m_Primitive = this;
}

ActorPrimitive2D::ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept
	: m_Render(std::move(primitive.m_Render)), m_Transformer(std::move(primitive.m_Transformer)), m_Modulate(std::move(primitive.m_Modulate)), ActorRenderBase2D(std::move(primitive)), m_Status(primitive.m_Status), m_ModulationColors(std::move(primitive.m_ModulationColors))
{
	m_Transformer.notify = std::make_unique<ActorPrimitive2D_TN>(this);
	m_Modulate->m_Primitive = this;
}

ActorPrimitive2D& ActorPrimitive2D::operator=(const ActorPrimitive2D& primitive)
{
	m_Render = primitive.m_Render;
	m_Status = primitive.m_Status;
	m_ModulationColors = primitive.m_ModulationColors;
	m_Transformer = primitive.m_Transformer;
	m_Modulate = primitive.m_Modulate;
	ActorRenderBase2D::operator=(primitive);
	return *this;
}

ActorPrimitive2D& ActorPrimitive2D::operator=(ActorPrimitive2D&& primitive) noexcept
{
	m_Render = std::move(primitive.m_Render);
	m_Status = primitive.m_Status;
	m_ModulationColors = std::move(primitive.m_ModulationColors);
	m_Transformer = std::move(primitive.m_Transformer);
	m_Modulate = std::move(primitive.m_Modulate);
	ActorRenderBase2D::operator=(std::move(primitive));
	return *this;
}

ActorPrimitive2D::~ActorPrimitive2D()
{
}

void ActorPrimitive2D::RequestDraw(CanvasLayer* canvas_layer)
{
	if (m_Status & 0b1)
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
	if (m_Status & 0b10)
	{
		m_Status &= ~0b10;
		// TODO use regular position/condensed_rs for parent-independent actors - use bool flag.
		glm::vec2 position = m_Transformer.self.packedP;
		for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
		{
			m_Render.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(position.x);
			m_Render.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(position.y);
		}
	}
	// update TransformRS
	if (m_Status & 0b100)
	{
		m_Status &= ~0b100;
		glm::mat2 condensed_rs_matrix = m_Transformer.self.packedRS;
		for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
		{
			m_Render.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
			m_Render.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
			m_Render.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
			m_Render.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
		}
	}
	// update ModulationColor
	if (m_Status & 0b1000)
	{
		m_Status &= ~0b1000;
		for (BufferCounter i = 0; i < m_Render.vertexCount && i < m_ModulationColors.size(); i++)
		{
			auto color = m_ModulationColors[i] * m_Modulate->GetColor();
			m_Render.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(color.r);
			m_Render.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(color.g);
			m_Render.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(color.b);
			m_Render.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(color.a);
		}
	}
}

void ActorPrimitive2D::CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height)
{
	// TODO I added i * stride + to each line, as well as for CropRelativePoints. See that it still works, or that it works better.
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	for (size_t i = 0; i < points.size() && i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + ActorPrimitive2D::end_attrib_pos + 2] = points[i][0] / atlas_width;
		m_Render.vertexBufferData[i * stride + ActorPrimitive2D::end_attrib_pos + 3] = points[i][1] / atlas_height;
	}
}

void ActorPrimitive2D::CropRelativePoints(const std::vector<glm::vec2>& atlas_points)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	for (size_t i = 0; i < atlas_points.size() && i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + ActorPrimitive2D::end_attrib_pos + 2] = atlas_points[i][0];
		m_Render.vertexBufferData[i * stride + ActorPrimitive2D::end_attrib_pos + 3] = atlas_points[i][1];
	}
}
