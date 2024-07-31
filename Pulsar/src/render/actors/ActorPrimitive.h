#pragma once

#include <vector>

#include "Typedefs.h"
#include "../ActorRenderBase.h"
#include "../Renderable.h"
#include "../transform/Transformable.h"
#include "../transform/Modulatable.h"

class ActorPrimitive2D : public ActorRenderBase2D
{
	std::shared_ptr<class PrimitiveTransformable2D> m_Transform;
	std::shared_ptr<class PrimitiveModulatable> m_Modulate;

protected:
	static constexpr Stride end_attrib_pos = 11;
	
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	Renderable m_Render;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible

public:
	unsigned char m_Status;
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = {}, ZIndex z = 0, bool visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept;
	ActorPrimitive2D& operator=(const ActorPrimitive2D & primitive);
	ActorPrimitive2D& operator=(ActorPrimitive2D&& primitive) noexcept;

	ActorPrimitive2D Clone();
	void Clone(ActorPrimitive2D&);

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(bool visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FlushTransform() { m_Status |= 0b110; }
	inline void FlushPosition() { m_Status |= 0b10; }
	inline void FlushRotation() { m_Status |= 0b100; }
	inline void FlushScale() { m_Status |= 0b100; }
	inline void FlushModulate() { m_Status |= 0b1000; }
	inline void FillModulationPoints(const glm::vec4& default_value = { 1.0f, 1.0f, 1.0f, 1.0f })
	{ 
		if (m_ModulationColors.size() < m_Render.vertexCount)
			m_ModulationColors.resize(m_Render.vertexCount, default_value);
		FlushModulate();
	}
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); FlushModulate(); }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; FlushModulate(); }
	inline void SetModulationPerPoint(std::vector<glm::vec4>&& colors) { m_ModulationColors = std::move(colors); FlushModulate(); }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
	inline const Renderable& GetRenderable() const { return m_Render; }

	std::shared_ptr<class PrimitiveTransformable2D> Transform() { return m_Transform; }
	std::weak_ptr<class PrimitiveTransformable2D> TransformWeak() { return m_Transform; }
	std::shared_ptr<class PrimitiveModulatable> Modulate() { return m_Modulate; }
	std::weak_ptr<class PrimitiveModulatable> ModulateWeak() { return m_Modulate; }

protected:
	void OnDraw(signed char texture_slot);
};

class PrimitiveTransformable2D : public TransformableProxy2D
{
	friend class ActorPrimitive2D;
	ActorPrimitive2D* m_Primitive = nullptr;

public:
	PrimitiveTransformable2D(const Transform2D& transform = {}) : TransformableProxy2D(transform) {}

	inline void OperateTransform(const std::function<void(Transform2D& transform)>& op) override { op(m_Transform); m_Primitive->FlushTransform(); }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); m_Primitive->FlushPosition(); }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); m_Primitive->FlushRotation(); }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); m_Primitive->FlushScale(); }
};

class PrimitiveModulatable : public ModulatableProxy
{
	friend class ActorPrimitive2D;
	ActorPrimitive2D* m_Primitive = nullptr;

public:
	PrimitiveModulatable() : ModulatableProxy({ 1.0f, 1.0f, 1.0f, 1.0f }) {}

	inline void OperateColor(const std::function<void(glm::vec4& color)>& op) override { op(m_Color); m_Primitive->FillModulationPoints(); }
};
