#pragma once

#include <vector>

#include "Typedefs.h"
#include "../ActorRenderBase.h"
#include "../Renderable.h"
#include "../transform/Transformable.h"

class ActorPrimitive2D : public ActorRenderBase2D
{
	std::shared_ptr<class PrimitiveTransformable2D> m_Transform;

protected:
	static constexpr Stride end_attrib_pos = 11;
	
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	Renderable m_Render;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = {}, ZIndex z = 0, bool visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept;
	ActorPrimitive2D& operator=(const ActorPrimitive2D & primitive);
	ActorPrimitive2D& operator=(ActorPrimitive2D&& primitive) noexcept;

	ActorPrimitive2D Clone();
	void Clone(ActorPrimitive2D&);

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline bool IsVisible() const { return (m_Status & 1) > 0; }


	inline void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(bool visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FlushTransform() { m_Status |= 0b110; }
	inline void FlushPosition() { m_Status |= 0b10; }
	inline void FlushRotation() { m_Status |= 0b100; }
	inline void FlushScale() { m_Status |= 0b100; }
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); m_Status |= 0b1000; }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; m_Status |= 0b1000; }
	inline void SetModulationPerPoint(std::vector<glm::vec4>&& colors) { m_ModulationColors = std::move(colors); m_Status |= 0b1000; }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
	inline const Renderable& GetRenderable() const { return m_Render; }

	std::shared_ptr<class PrimitiveTransformable2D> Transform() { return m_Transform; }

protected:
	void OnDraw(signed char texture_slot);
};

class PrimitiveTransformable2D : public TransformableProxy2D
{
	ActorPrimitive2D* m_Primitive;

public:
	PrimitiveTransformable2D(ActorPrimitive2D* primitive = nullptr, const Transform2D& transform = {}) : TransformableProxy2D(transform), m_Primitive(primitive) {}
	
	inline void SetPrimitive(ActorPrimitive2D* primitive) { m_Primitive = primitive; }

	inline void OperateTransform(const std::function<void(Transform2D& transform)>& op) override { op(m_Transform); m_Primitive->FlushTransform(); }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); m_Primitive->FlushPosition(); }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); m_Primitive->FlushRotation(); }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); m_Primitive->FlushScale(); }
};

