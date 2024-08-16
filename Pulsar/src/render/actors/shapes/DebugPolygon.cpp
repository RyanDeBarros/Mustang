#include "DebugPolygon.h"

#include "render/CanvasLayer.h"
#include "AssetLoader.h"

DebugPolygon::DebugPolygon(const std::vector<glm::vec2>& points, GLenum indexing_mode, ZIndex z, FickleType fickle_type)
	: FickleActor2D(fickle_type, z), m_Notification(new DP_Notification(this))
{
	*m_Fickler.Notification() = m_Notification;
	Loader::loadRenderable(_RendererSettings::solid_polygon_filepath.c_str(), m_Renderable);
	SetIndexingMode(indexing_mode);
	PointsRef() = points;
}

DebugPolygon::DebugPolygon(const DebugPolygon& other)
	: FickleActor2D(other), m_Renderable(other.m_Renderable), m_Points(other.m_Points), m_IndexingMode(other.m_IndexingMode), m_Notification(new DP_Notification(this)), m_Status(other.m_Status)
{
	*m_Fickler.Notification() = m_Notification;
}

DebugPolygon::DebugPolygon(DebugPolygon&& other) noexcept
	: FickleActor2D(std::move(other)), m_Renderable(std::move(other.m_Renderable)), m_Points(std::move(other.m_Points)), m_IndexingMode(other.m_IndexingMode), m_Notification(new DP_Notification(this)), m_Status(other.m_Status)
{
	*m_Fickler.Notification() = m_Notification;
}

DebugPolygon& DebugPolygon::operator=(const DebugPolygon& other)
{
	FickleActor2D::operator=(other);
	m_Renderable = other.m_Renderable;
	m_Points = other.m_Points;
	m_IndexingMode = other.m_IndexingMode;
	m_Status = other.m_Status;
	return *this;
}

DebugPolygon& DebugPolygon::operator=(DebugPolygon&& other) noexcept
{
	FickleActor2D::operator=(std::move(other));
	m_Renderable = std::move(other.m_Renderable);
	m_Points = std::move(other.m_Points);
	m_IndexingMode = other.m_IndexingMode;
	m_Status = other.m_Status;
	return *this;
}

DebugPolygon::~DebugPolygon()
{
	if (m_Notification)
		delete m_Notification;
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
		if (Modulate* color = m_Fickler.PackedM()) [[likely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				// TODO buffer overflow?
				m_Renderable.vertexBufferData[i * stride + 6] = static_cast<GLfloat>((*color)[0]);
				m_Renderable.vertexBufferData[i * stride + 7] = static_cast<GLfloat>((*color)[1]);
				m_Renderable.vertexBufferData[i * stride + 8] = static_cast<GLfloat>((*color)[2]);
				m_Renderable.vertexBufferData[i * stride + 9] = static_cast<GLfloat>((*color)[3]);
			}
		}
		else [[unlikely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				// TODO buffer overflow?
				m_Renderable.vertexBufferData[i * stride + 6] = 1.0f;
				m_Renderable.vertexBufferData[i * stride + 7] = 1.0f;
				m_Renderable.vertexBufferData[i * stride + 8] = 1.0f;
				m_Renderable.vertexBufferData[i * stride + 9] = 1.0f;
			}
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
		if (PackedP2D* position = m_Fickler.PackedP()) [[likely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				m_Renderable.vertexBufferData[i * stride	] = static_cast<GLfloat>(position->x);
				m_Renderable.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(position->y);
			}
		}
		else [[unlikely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				m_Renderable.vertexBufferData[i * stride	] = 0.0f;
				m_Renderable.vertexBufferData[i * stride + 1] = 0.0f;
			}
		}
	}
	// update TransformRS
	if (m_Status & 0b10000)
	{
		m_Status &= ~0b10000;
		Stride stride = Render::StrideCountOf(m_Renderable.model.layout, m_Renderable.model.layoutMask);
		// TODO resolve this conditional at construction
		if (PackedRS2D* condensed_rs_matrix = m_Fickler.PackedRS()) [[likely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				m_Renderable.vertexBufferData[i * stride + 2] = static_cast<GLfloat>((*condensed_rs_matrix)[0][0]);
				m_Renderable.vertexBufferData[i * stride + 3] = static_cast<GLfloat>((*condensed_rs_matrix)[0][1]);
				m_Renderable.vertexBufferData[i * stride + 4] = static_cast<GLfloat>((*condensed_rs_matrix)[1][0]);
				m_Renderable.vertexBufferData[i * stride + 5] = static_cast<GLfloat>((*condensed_rs_matrix)[1][1]);
			}
		}
		else [[unlikely]]
		{
			for (BufferCounter i = 0; i < m_Renderable.vertexCount; i++)
			{
				m_Renderable.vertexBufferData[i * stride + 2] = 0.0f;
				m_Renderable.vertexBufferData[i * stride + 3] = 0.0f;
				m_Renderable.vertexBufferData[i * stride + 4] = 0.0f;
				m_Renderable.vertexBufferData[i * stride + 5] = 0.0f;
			}
		}
	}
}
