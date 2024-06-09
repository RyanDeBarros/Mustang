#pragma once

#include "Typedefs.h"
#include "render/Renderable.h"
#include "Transform.h"

class ActorPrimitive2D
{
protected:
	friend class CanvasLayer;
	ZIndex m_Z;
	Renderable m_Render;
	Transform2D m_Transform;
	bool m_Visible;
public:
	ActorPrimitive2D();
	ActorPrimitive2D(Renderable render, Transform2D transform, ZIndex z = 0, bool visible = true);
	
	void OnDraw(signed char texture_slot);

	inline ZIndex GetZIndex() const { return m_Z; }
	// TODO mostly just for testing. these properties will be set with subclasses that have access to the protected data members.
	inline void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }
};
