#pragma once

#include "Typedefs.h"
#include "render/Renderable.h"
#include "Transform.h"

class ActorPrimitive2D
{
protected:
	friend class CanvasLayer;
	ZIndex m_Z;
	// TODO make Renderable a pointer?
	Renderable m_Render;
	Transform2D m_Transform;
	bool m_Visible;
public:
	ActorPrimitive2D();
	ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z = 0, bool visible = true);
	~ActorPrimitive2D();

	void OnDraw(signed char texture_slot);

	inline ZIndex GetZIndex() const { return m_Z; }
	inline void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }
};
