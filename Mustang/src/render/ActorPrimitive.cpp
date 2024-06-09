#include "ActorPrimitive.h"

#include "Logger.h"

ActorPrimitive2D::ActorPrimitive2D()
	: m_Transform(), m_Z(0), m_Visible(true)
{
}

ActorPrimitive2D::ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z, bool visible)
	: m_Render(render), m_Transform(transform), m_Z(z), m_Visible(visible)
{
}

void ActorPrimitive2D::OnDraw(signed char texture_slot)
{
	if (!m_Render.vertexBufferData)
		return;
	const auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	// TODO if texture_slot == -1 and it was -1 in the previous frame, don't go through the for-loop below:
	for (BufferCounter i = 0; i < m_Render.vertexCount; i++)
	{
		*(m_Render.vertexBufferData + i * stride) = texture_slot;
	}
	// TODO insert transform into vertexBufferData
}
