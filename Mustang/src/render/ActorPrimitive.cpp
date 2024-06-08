#include "ActorPrimitive.h"

#include "Logger.h"

ActorPrimitive2D::ActorPrimitive2D()
	: m_Render(Render::Empty), m_Transform(), m_Z(0), m_Visible(true)
{
}

ActorPrimitive2D::ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z, bool visible)
	: m_Render(render), m_Transform(transform), m_Z(z), m_Visible(visible)
{
}

ActorPrimitive2D::~ActorPrimitive2D()
{
}

void ActorPrimitive2D::OnDraw(signed char texture_slot)
{
	for (VertexCounter i = 0; i < Render::VertexBufferLayoutCount(m_Render); i += Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask))
	{
		*(m_Render.vertexBufferData + i) = texture_slot;
	}
	// TODO modify first vertex attrib = texture_slot ONLY IF IT IS POSITIVE OR == -1 AND WASN'T -1 BEFORE
	// TODO insert transform into vertexBufferData
}
