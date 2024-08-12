#include "DebugPolygon.h"

#include "render/CanvasLayer.h"
#include "AssetLoader.h"

DebugPolygon::DebugPolygon(const std::vector<glm::vec2>& points, const Transform2D& transform, const glm::vec4& color, GLenum indexing_mode, ZIndex z)
	: ActorRenderBase2D(z), m_Color(std::make_shared<DebugModulatable>()), m_Transform(std::make_shared<DebugTransformable2D>())
{
	Loader::loadRenderable(_RendererSettings::solid_polygon_filepath.c_str(), m_Renderable);
	SetIndexingMode(indexing_mode);
	PointsRef() = points;
	m_Transform->m_Poly = this;
	m_Transform->SetTransform(transform);
	m_Color->m_Poly = this;
	m_Color->SetColor(color);
}

DebugPolygon::DebugPolygon(const DebugPolygon& other)
	: ActorRenderBase2D(other), m_Color(std::make_shared<DebugModulatable>(other.m_Color->GetColor())), m_Renderable(other.m_Renderable), m_Points(other.m_Points), m_IndexingMode(other.m_IndexingMode), m_Transform(std::make_shared<DebugTransformable2D>(other.m_Transform->GetTransform())), m_Status(other.m_Status)
{
	m_Transform->m_Poly = this;
	m_Color->m_Poly = this;
}

DebugPolygon::DebugPolygon(DebugPolygon&& other) noexcept
	: ActorRenderBase2D(std::move(other)), m_Color(other.m_Color), m_Renderable(std::move(other.m_Renderable)), m_Points(std::move(other.m_Points)), m_IndexingMode(other.m_IndexingMode), m_Transform(std::move(other.m_Transform)), m_Status(other.m_Status)
{
	m_Transform->m_Poly = this;
	m_Color->m_Poly = this;
}

DebugPolygon& DebugPolygon::operator=(const DebugPolygon& other)
{
	m_Color->SetColor(other.m_Color->GetColor());
	m_Renderable = other.m_Renderable;
	m_Points = other.m_Points;
	m_IndexingMode = other.m_IndexingMode;
	m_Status = other.m_Status;
	m_Transform->SetTransform(other.m_Transform->GetTransform());
	ActorRenderBase2D::operator=(other);
	return *this;
}

DebugPolygon& DebugPolygon::operator=(DebugPolygon&& other) noexcept
{
	m_Color->SetColor(other.m_Color->GetColor());
	m_Renderable = std::move(other.m_Renderable);
	m_Points = std::move(other.m_Points);
	m_IndexingMode = other.m_IndexingMode;
	m_Status = other.m_Status;
	m_Transform->SetTransform(other.m_Transform->GetTransform());
	ActorRenderBase2D::operator=(std::move(other));
	return *this;
}

void DebugPolygon::RequestDraw(CanvasLayer* canvas_layer)
{
	if (DrawPrep())
		canvas_layer->DrawArray(m_Renderable, m_IndexingMode);
}

bool DebugPolygon::DrawPrep()
{
	if (visible)
	{
		CheckStatus();
		return true;
	}
	return false;
}

void DebugPolygon::CheckStatus()
{
	// allocate vertex buffer
	if (m_Status & 0b1)
	{
		m_Status = ~0b1;
		m_Renderable.vertexCount = static_cast<BufferCounter>(m_Points.size());
		if (m_Renderable.vertexBufferData)
			delete[] m_Renderable.vertexBufferData;
		m_Renderable.vertexBufferData = new GLfloat[Render::VertexBufferLayoutCount(m_Renderable)];
	}
	ASSERT(m_Points.size() == m_Renderable.vertexCount);
	// modify color of vertices
	if (m_Status & 0b10)
	{
		m_Status &= ~0b10;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(m_Color->GetColor()[0]);
			m_Renderable.vertexBufferData[i * stride + 7] = static_cast<GLfloat>(m_Color->GetColor()[1]);
			m_Renderable.vertexBufferData[i * stride + 8] = static_cast<GLfloat>(m_Color->GetColor()[2]);
			m_Renderable.vertexBufferData[i * stride + 9] = static_cast<GLfloat>(m_Color->GetColor()[3]);
		}
	}
	// modify point positions
	if (m_Status & 0b100)
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
	if (m_Status & 0b1000)
	{
		m_Status &= ~0b1000;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride	] = static_cast<GLfloat>(m_Transform->GetPosition().x);
			m_Renderable.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(m_Transform->GetPosition().y);
		}
	}
	// update TransformRS
	if (m_Status & 0b10000)
	{
		m_Status &= ~0b10000;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		glm::mat2 condensed_rs_matrix = Transform::CondensedRS(m_Transform->GetTransform());
		for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
		{
			m_Renderable.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
			m_Renderable.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
			m_Renderable.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
			m_Renderable.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
		}
	}
}
