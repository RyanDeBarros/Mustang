#pragma once

#include <vector>

#include "Typedefs.h"
#include "render/ActorRenderBase.h"
#include "render/Renderable.h"
#include "render/Transform.h"

class ActorPrimitive2D : public ActorRenderBase2D, public Transformable2D
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

	static constexpr Stride end_attrib_pos = 11;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = Transform2D(), const ZIndex& z = 0, const bool& visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	inline virtual ZIndex GetZIndex() const override { return m_Z; }
	inline virtual void SetZIndex(const ZIndex& z) override { m_Z = z; }

	inline bool IsVisible() const { return (m_Status & 1) > 0; }

	inline Transform2D GetTransform() const override { return m_Transform; }
	inline void SetTransform(const Transform2D& transform) override { m_Transform = transform; m_Status |= 0b110; }
	inline glm::vec2 GetPosition() const override { return m_Transform.position; }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); FlushPosition(); }
	inline glm::float32 GetRotation() const override { return m_Transform.rotation; }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); FlushRotation(); }
	inline glm::vec2 GetScale() const override { return m_Transform.scale; }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); FlushScale(); }

	inline Transform2D* const GetTransformRef() { return &m_Transform; }

	inline void SetShaderHandle(const ShaderHandle& handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(const TextureHandle& handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(const bool& visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FlushPosition() { m_Status |= 0b10; }
	inline void FlushRotation() { m_Status |= 0b100; }
	inline void FlushScale() { m_Status |= 0b100; }
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); m_Status |= 0b1000; }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; m_Status |= 0b1000; }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
	inline const Renderable& GetRenderable() const { return m_Render; }

protected:
	void OnDraw(signed char texture_slot);
};
