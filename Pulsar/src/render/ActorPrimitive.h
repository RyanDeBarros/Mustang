#pragma once

#include <vector>

#include "Typedefs.h"
#include "Renderable.h"
#include "Transform.h"

class ActorPrimitive2D
{
protected:
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	ZIndex m_Z;
	Renderable m_Render;
	Transform2D m_Transform;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = Transform2D(), const ZIndex& z = 0, const bool& visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	
	// TODO mostly just for testing. these properties will be set with subclasses that have access to the protected data members.
	inline ZIndex GetZIndex() const { return m_Z; }
	inline const Transform2D& GetTransform() const { return m_Transform; }
	inline void SetShaderHandle(const ShaderHandle& handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(const TextureHandle& handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(const bool& visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	
	inline void SetTransform(const Transform2D& transform) { m_Transform = transform; m_Status |= 0b110; }
	inline void SetPosition(const float& x, const float& y) { m_Transform.position.x = x; m_Transform.position.y = y; m_Status |= 0b10; }
	inline void SetRotation(const float& r) { m_Transform.rotation = r; m_Status |= 0b100; }
	inline void SetScale(const float& x, const float& y) { m_Transform.scale.x = x; m_Transform.scale.y = y; m_Status |= 0b100; }
	inline void AddPosition(const float& x, const float& y) { m_Transform.position.x += x; m_Transform.position.y += y; m_Status |= 0b10; }
	inline void AddRotation(const float& r) { m_Transform.rotation += r; m_Status |= 0b100; }
	inline void AddScale(const float& x, const float& y) { m_Transform.scale.x += x; m_Transform.scale.y += y; m_Status |= 0b100; }
	inline void MultPosition(const float& x, const float& y) { m_Transform.position.x *= x; m_Transform.position.y *= y; m_Status |= 0b10; }
	inline void MultRotation(const float& r) { m_Transform.rotation *= r; m_Status |= 0b100; }
	inline void MultScale(const float& x, const float& y) { m_Transform.scale.x *= x; m_Transform.scale.y *= y; m_Status |= 0b100; }
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); m_Status |= 0b1000; }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; m_Status |= 0b1000; }

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
protected:
	void OnDraw(signed char texture_slot);
};
