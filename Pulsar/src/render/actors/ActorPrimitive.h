#pragma once

#include <vector>

#include "Typedefs.h"
#include "../ActorRenderBase.h"
#include "../Renderable.h"

struct AP2D_Notification;

class ActorPrimitive2D : public FickleActor2D
{
	AP2D_Notification* m_Notification;

protected:
	static constexpr Stride end_attrib_pos = 11;
	
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	Renderable m_Render;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... | transformRS updated | transformP updated | visible
	unsigned char m_Status = 0b111;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept;
	ActorPrimitive2D& operator=(const ActorPrimitive2D & primitive);
	ActorPrimitive2D& operator=(ActorPrimitive2D&& primitive) noexcept;
	~ActorPrimitive2D();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	inline void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	inline virtual void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }

	inline void SetVisible(bool visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	inline void FillModulationPoints(const glm::vec4& default_value = { 1.0f, 1.0f, 1.0f, 1.0f })
	{ 
		if (m_ModulationColors.size() < m_Render.vertexCount)
			m_ModulationColors.resize(m_Render.vertexCount, default_value);
		FlagModulate();
	}

	inline void FlagProteate() { m_Status |= 0b1110; }
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

protected:
	void OnDraw(signed char texture_slot);

private:
	void BindBufferFuncs();
	void (ActorPrimitive2D::*f_BufferPackedP)(Stride) = nullptr;
	void (ActorPrimitive2D::*f_BufferPackedRS)(Stride) = nullptr;
	void (ActorPrimitive2D::*f_BufferPackedM)(Stride) = nullptr;
	void buffer_packed_p_protean(Stride);
	void buffer_packed_p_transformable(Stride);
	void buffer_packed_p_modulatable(Stride);
	void buffer_packed_rs_protean(Stride);
	void buffer_packed_rs_transformable(Stride);
	void buffer_packed_rs_modulatable(Stride);
	void buffer_packed_m_protean(Stride);
	void buffer_packed_m_transformable(Stride);
	void buffer_packed_m_modulatable(Stride);
};

struct AP2D_Notification : public FickleNotification
{
	ActorPrimitive2D* prim = nullptr;

	inline AP2D_Notification(ActorPrimitive2D* prim) : prim(prim) {}

	inline void Notify(FickleSyncCode code)
	{
		switch (code)
		{
		case FickleSyncCode::SyncAll:
			if (prim) prim->FlagProteate();
			break;
		case FickleSyncCode::SyncT:
			if (prim) prim->FlagTransform();
			break;
		case FickleSyncCode::SyncP:
			if (prim) prim->FlagTransformP();
			break;
		case FickleSyncCode::SyncRS:
			if (prim) prim->FlagTransformRS();
			break;
		case FickleSyncCode::SyncM:
			if (prim) prim->FlagModulate();
			break;
		}
	}
};
