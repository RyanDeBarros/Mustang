#pragma once

#include "Typedefs.h"
#include "render/Renderable.h"
#include "Transform.h"

class ActorPrimitive2D
{
	ZIndex m_Z;
	Renderable m_Render;
	Transform2D m_Transform;
	bool m_Visible;
public:
	ActorPrimitive2D();
	ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z = 0, bool visible = true);
};
