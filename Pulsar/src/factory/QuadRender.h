#pragma once

#include "render/ActorPrimitive.h"
#include "render/Transform.h"

class QuadRender : public ActorPrimitive2D
{
	static Renderable quad_renderable;
public:
	QuadRender(const Transform2D& transform = Transform2D(), ZIndex z = 0, bool visible = true);
	static void DefineQuadRenderable();
};
