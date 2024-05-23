#include "ActorPrimitive.h"

ActorPrimitive2D::ActorPrimitive2D()
	: m_Render(Render::Empty), m_Transform(), m_Z(0), m_Visible(true)
{
}

ActorPrimitive2D::ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z, bool visible)
	: m_Render(render), m_Transform(transform), m_Z(z), m_Visible(visible)
{
}
