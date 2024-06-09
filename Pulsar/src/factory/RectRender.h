#pragma once

#include "render/ActorPrimitive.h"
#include "render/Transform.h"

class RectRender : public ActorPrimitive2D
{
	static Renderable rect_renderable;
public:
	RectRender(const Transform2D& transform = Transform2D(), ZIndex z = 0, bool visible = true);
	static void DefineRectRenderable();
};
