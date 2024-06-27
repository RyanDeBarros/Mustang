#pragma once

#include <vector>

#include "Typedefs.h"
#include "ActorRenderBase.h"
#include "Renderable.h"
#include "Transform.h"

class ActorPrimitive2D : public ActorRenderBase2D
{
	ACTOR_RENDER_INHERIT(ActorPrimitive2D, ActorRenderBase2D)
protected:
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	ZIndex m_Z;
	Renderable m_Render;
	Transform2D m_Transform;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status;

	static constexpr Stride end_attrib_pos = 11;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = Transform2D(), const ZIndex& z = 0, const bool& visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	
	inline virtual ZIndex GetZIndex() const override { return m_Z; }
	inline virtual void SetZIndex(const ZIndex& z) override { m_Z = z; }
	inline const Transform2D& GetTransform() const { return m_Transform; }
	inline void SetShaderHandle(const ShaderHandle& handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(const TextureHandle& handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(const bool& visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FlushPosition() { m_Status |= 0b10; }
	inline void FlushRotation() { m_Status |= 0b100; }
	inline void FlushScale() { m_Status |= 0b100; }
	inline void SetTransform(const Transform2D& transform) { m_Transform = transform; m_Status |= 0b110; }

	inline void SetPosition(const float& x, const float& y) { m_Transform.position.x = x; m_Transform.position.y = y; FlushPosition(); }
	inline void SetRotation(const float& r) { m_Transform.rotation = r; FlushRotation(); }
	inline void SetScale(const float& x, const float& y) { m_Transform.scale.x = x; m_Transform.scale.y = y; FlushScale(); }
	inline void AddPosition(const float& x, const float& y) { m_Transform.position.x += x; m_Transform.position.y += y; FlushPosition(); }
	inline void AddRotation(const float& r) { m_Transform.rotation += r; FlushRotation(); }
	inline void AddScale(const float& x, const float& y) { m_Transform.scale.x += x; m_Transform.scale.y += y; FlushScale(); }
	inline void MultPosition(const float& x, const float& y) { m_Transform.position.x *= x; m_Transform.position.y *= y; FlushPosition(); }
	inline void MultRotation(const float& r) { m_Transform.rotation *= r; FlushRotation(); }
	inline void MultScale(const float& x, const float& y) { m_Transform.scale.x *= x; m_Transform.scale.y *= y; FlushScale(); }
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); m_Status |= 0b1000; }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; m_Status |= 0b1000; }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }

protected:
	void OnDraw(signed char texture_slot);
};
