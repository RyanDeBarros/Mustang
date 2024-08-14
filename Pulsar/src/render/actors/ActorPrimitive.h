#pragma once

#include <vector>

#include "Typedefs.h"
#include "../ActorRenderBase.h"
#include "../Renderable.h"
#include "../transform/Transform.h"
#include "../transform/Modulate.h"

struct AP2D_Notification;

class ActorPrimitive2D : public ActorRenderBase2D
{
	AP2D_Notification* m_Notification;
	Transformer2D m_Transformer;
	Modulator m_Modulator;

protected:
	static constexpr Stride end_attrib_pos = 11;
	
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	Renderable m_Render;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status = 0b111;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), const Transform2D& transform = {}, ZIndex z = 0, bool visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept;
	ActorPrimitive2D& operator=(const ActorPrimitive2D & primitive);
	ActorPrimitive2D& operator=(ActorPrimitive2D&& primitive) noexcept;
	~ActorPrimitive2D();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	inline void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(bool visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FillModulationPoints(const glm::vec4& default_value = { 1.0f, 1.0f, 1.0f, 1.0f })
	{ 
		if (m_ModulationColors.size() < m_Render.vertexCount)
			m_ModulationColors.resize(m_Render.vertexCount, default_value);
		FlagModulate();
	}

	inline void FlagTransform() { m_Status |= 0b110; }
	inline void FlagTransformP() { m_Status |= 0b10; }
	inline void FlagTransformRS() { m_Status |= 0b100; }
	inline void FlagModulate() { m_Status |= 0b1000; }
	
	inline void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); FlagModulate(); }
	inline void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; FlagModulate(); }
	inline void SetModulationPerPoint(std::vector<glm::vec4>&& colors) { m_ModulationColors = std::move(colors); FlagModulate(); }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	inline TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
	inline const Renderable& GetRenderable() const { return m_Render; }

	Transformer2D* Transformer() { return &m_Transformer; }
	Transform2D* Transform() { return &m_Transformer.self.transform; }
	Modulator* Modulator() { return &m_Modulator; }
	Modulate* Modulate() { return &m_Modulator.self.modulate; }

protected:
	void OnDraw(signed char texture_slot);
};

struct AP2D_Notification : public TransformNotification, public ModulateNotification
{
	ActorPrimitive2D* prim = nullptr;

	AP2D_Notification(ActorPrimitive2D* prim) : prim(prim) {}

	void Notify() override { if (prim) prim->FlagTransform(); }
	void NotifyP() override { if (prim) prim->FlagTransformP(); }
	void NotifyRS() override { if (prim) prim->FlagTransformRS(); }
	void NotifyM() override { if (prim) prim->FlagModulate(); }
};
