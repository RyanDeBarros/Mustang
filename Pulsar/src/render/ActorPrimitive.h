#pragma once

#include "Typedefs.h"
#include "Renderable.h"
#include "Transform.h"

class ActorPrimitive2D
{
protected:
	friend class CanvasLayer;
	ZIndex m_Z;
	Renderable m_Render;
	Transform2D m_Transform;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status;

public:
	ActorPrimitive2D();
	ActorPrimitive2D(const Renderable& render);
	ActorPrimitive2D(const Renderable& render, const Transform2D& transform, const ZIndex& z = 0, const bool& visible = true);
	
	// TODO mostly just for testing. these properties will be set with subclasses that have access to the protected data members.
	inline ZIndex GetZIndex() const { return m_Z; }
	inline void SetVisible(const bool& visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void SetShaderHandle(const ShaderHandle& handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(const TextureHandle& handle) { m_Render.textureHandle = handle; }
	inline void SetPosition(const float& x, const float& y) { m_Transform.position.x = x; m_Transform.position.y = y; m_Status |= 0b10; }
	inline void SetRotation(const float& r) { m_Transform.rotation = r; m_Status |= 0b100; }
	inline void SetScale(const float& x, const float& y) { m_Transform.scale.x = x; m_Transform.scale.y = y; m_Status |= 0b100; }

protected:
	void OnDraw(signed char texture_slot);
};
