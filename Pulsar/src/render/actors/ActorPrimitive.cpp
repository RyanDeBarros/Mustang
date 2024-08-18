#include "ActorPrimitive.h"

#include "Logger.inl"
#include "render/CanvasLayer.h"

ActorPrimitive2D::ActorPrimitive2D(const Renderable& render, ZIndex z, FickleType fickle_type, bool visible)
	: FickleActor2D(fickle_type, z), m_Render(render), m_Notification(new AP2D_Notification(this)), m_Status(visible ? 0b111 : 0b110)
{
	*m_Fickler.Notification() = m_Notification;
	BindBufferFuncs();
}

ActorPrimitive2D::ActorPrimitive2D(const ActorPrimitive2D& primitive)
	: FickleActor2D(primitive), m_Render(primitive.m_Render), m_Notification(new AP2D_Notification(this)), m_Status(primitive.m_Status), m_ModulationColors(primitive.m_ModulationColors), f_BufferPackedP(primitive.f_BufferPackedP), f_BufferPackedRS(primitive.f_BufferPackedRS), f_BufferPackedM(primitive.f_BufferPackedM)
{
	*m_Fickler.Notification() = m_Notification;
}

ActorPrimitive2D::ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept
	: FickleActor2D(std::move(primitive)), m_Render(std::move(primitive.m_Render)), m_Notification(new AP2D_Notification(this)), m_Status(primitive.m_Status), m_ModulationColors(std::move(primitive.m_ModulationColors)), f_BufferPackedP(primitive.f_BufferPackedP), f_BufferPackedRS(primitive.f_BufferPackedRS), f_BufferPackedM(primitive.f_BufferPackedM)
{
	*m_Fickler.Notification() = m_Notification;
}

ActorPrimitive2D& ActorPrimitive2D::operator=(const ActorPrimitive2D& primitive)
{
	if (this == &primitive)
		return *this;
	FickleActor2D::operator=(primitive);
	m_Render = primitive.m_Render;
	m_Status = primitive.m_Status;
	m_ModulationColors = primitive.m_ModulationColors;
	f_BufferPackedP = primitive.f_BufferPackedP;
	f_BufferPackedRS = primitive.f_BufferPackedRS;
	f_BufferPackedM = primitive.f_BufferPackedM;
	return *this;
}

ActorPrimitive2D& ActorPrimitive2D::operator=(ActorPrimitive2D&& primitive) noexcept
{
	if (this == &primitive)
		return *this;
	FickleActor2D::operator=(std::move(primitive));
	m_Render = std::move(primitive.m_Render);
	m_Status = primitive.m_Status;
	m_ModulationColors = std::move(primitive.m_ModulationColors);
	f_BufferPackedP = primitive.f_BufferPackedP;
	f_BufferPackedRS = primitive.f_BufferPackedRS;
	f_BufferPackedM = primitive.f_BufferPackedM;
	return *this;
}

ActorPrimitive2D::~ActorPrimitive2D()
{
	if (m_Notification)
		delete m_Notification;
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
			m_Render.vertexBufferData[i * stride] = static_cast<GLfloat>(texture_slot);
	}
	// update TransformP
	if (m_Status & 0b10)
	{
		m_Status &= ~0b10;
		// TODO use regular position/condensed_rs for parent-independent actors - use bool flag.
		(this->*f_BufferPackedP)(stride);
	}
	// update TransformRS
	if (m_Status & 0b100)
	{
		m_Status &= ~0b100;
		(this->*f_BufferPackedRS)(stride);
	}
	// update ModulationColor
	if (m_Status & 0b1000)
	{
		m_Status &= ~0b1000;
		(this->*f_BufferPackedM)(stride);
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

void ActorPrimitive2D::BindBufferFuncs()
{
	switch (m_Fickler.Type())
	{
	case FickleType::Protean:
		f_BufferPackedP = &ActorPrimitive2D::buffer_packed_p_protean;
		f_BufferPackedRS = &ActorPrimitive2D::buffer_packed_rs_protean;
		f_BufferPackedM = &ActorPrimitive2D::buffer_packed_m_protean;
		break;
	case FickleType::Transformable:
		f_BufferPackedP = &ActorPrimitive2D::buffer_packed_p_transformable;
		f_BufferPackedRS = &ActorPrimitive2D::buffer_packed_rs_transformable;
		f_BufferPackedM = &ActorPrimitive2D::buffer_packed_m_transformable;
		break;
	case FickleType::Modulatable:
		f_BufferPackedP = &ActorPrimitive2D::buffer_packed_p_modulatable;
		f_BufferPackedRS = &ActorPrimitive2D::buffer_packed_rs_modulatable;
		f_BufferPackedM = &ActorPrimitive2D::buffer_packed_m_modulatable;
		break;
	default:
		f_BufferPackedP = nullptr;
		f_BufferPackedRS = nullptr;
		f_BufferPackedM = nullptr;
	}
}

void ActorPrimitive2D::buffer_packed_p_protean(Stride stride)
{
	const PackedP2D& position = m_Fickler.ProteanLinker()->self.packedP;
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(position.x);
		m_Render.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(position.y);
	}
}

void ActorPrimitive2D::buffer_packed_p_transformable(Stride stride)
{
	const PackedP2D& position = m_Fickler.Transformer()->self.packedP;
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(position.x);
		m_Render.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(position.y);
	}
}

void ActorPrimitive2D::buffer_packed_p_modulatable(Stride stride)
{
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 1] = 0.0f;
		m_Render.vertexBufferData[i * stride + 2] = 0.0f;
	}
}

void ActorPrimitive2D::buffer_packed_rs_protean(Stride stride)
{
	const PackedRS2D& condensed_rs_matrix = m_Fickler.ProteanLinker()->self.packedRS;
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
		m_Render.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
		m_Render.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
		m_Render.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
	}
}

void ActorPrimitive2D::buffer_packed_rs_transformable(Stride stride)
{
	const PackedRS2D& condensed_rs_matrix = m_Fickler.Transformer()->self.packedRS;
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(condensed_rs_matrix[0][0]);
		m_Render.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(condensed_rs_matrix[0][1]);
		m_Render.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(condensed_rs_matrix[1][0]);
		m_Render.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(condensed_rs_matrix[1][1]);
	}
}

void ActorPrimitive2D::buffer_packed_rs_modulatable(Stride stride)
{
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		m_Render.vertexBufferData[i * stride + 3] = 1.0f;
		m_Render.vertexBufferData[i * stride + 4] = 0.0f;
		m_Render.vertexBufferData[i * stride + 5] = 0.0f;
		m_Render.vertexBufferData[i * stride + 6] = 1.0f;
	}
}

void ActorPrimitive2D::buffer_packed_m_protean(Stride stride)
{
	const Modulate& modulate = m_Fickler.ProteanLinker()->self.packedM;
	for (BufferCounter i = 0; i < m_Render.vertexCount && i < m_ModulationColors.size(); i++)
	{
		Modulate color = m_ModulationColors[i] * modulate;
		m_Render.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(color.r);
		m_Render.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(color.g);
		m_Render.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(color.b);
		m_Render.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(color.a);
	}
}

void ActorPrimitive2D::buffer_packed_m_transformable(Stride stride)
{
	for (BufferCounter i = 0; i < m_Render.vertexCount && i < m_ModulationColors.size(); i++)
	{
		Modulate color = m_ModulationColors[i];
		m_Render.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(color.r);
		m_Render.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(color.g);
		m_Render.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(color.b);
		m_Render.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(color.a);
	}
}

void ActorPrimitive2D::buffer_packed_m_modulatable(Stride stride)
{
	const Modulate& modulate = m_Fickler.Modulator()->self.packedM;
	for (BufferCounter i = 0; i < m_Render.vertexCount && i < m_ModulationColors.size(); i++)
	{
		Modulate color = m_ModulationColors[i] * modulate;
		m_Render.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(color.r);
		m_Render.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(color.g);
		m_Render.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(color.b);
		m_Render.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(color.a);
	}
}
