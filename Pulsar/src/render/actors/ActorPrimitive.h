#pragma once

#include <vector>

#include "../ActorRenderBase.h"
#include "../Renderable.h"

struct AP2D_Notification;

class ActorPrimitive2D : public FickleActor2D
{
protected:
	static constexpr Stride end_attrib_pos = 11;

	AP2D_Notification* m_Notification;
	
	friend class CanvasLayer;
	friend class ActorTesselation2D;
	Renderable m_Render;
	std::vector<glm::vec4> m_ModulationColors;
	// m_Status = 0b... transformM updated | transformRS updated | transformP updated | visible
	unsigned char m_Status = 0b111;

public:
	ActorPrimitive2D(const Renderable& render = Renderable(), ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);
	ActorPrimitive2D(const ActorPrimitive2D& primitive);
	ActorPrimitive2D(ActorPrimitive2D&& primitive) noexcept;
	ActorPrimitive2D& operator=(const ActorPrimitive2D & primitive);
	ActorPrimitive2D& operator=(ActorPrimitive2D&& primitive) noexcept;
	~ActorPrimitive2D();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	void SetShaderHandle(ShaderHandle handle) { m_Render.model.shader = handle; }
	virtual void SetTextureHandle(TextureHandle handle) { m_Render.textureHandle = handle; }

	void SetVisible(bool visible) { m_Status = (visible ? m_Status |= 1 : m_Status &= ~1); }
	bool IsVisible() const { return m_Status & 0b1; }
	void FillModulationPoints(const glm::vec4& default_value = { 1.0f, 1.0f, 1.0f, 1.0f })
	{ 
		if (m_ModulationColors.size() < m_Render.vertexCount)
			m_ModulationColors.resize(m_Render.vertexCount, default_value);
		FlagModulate();
	}

	void FlagProteate() { m_Status |= 0b1110; }
	void FlagTransform() { m_Status |= 0b110; }
	void FlagTransformP() { m_Status |= 0b10; }
	void FlagTransformRS() { m_Status |= 0b100; }
	void FlagModulate() { m_Status |= 0b1000; }
	
	void SetModulation(const glm::vec4& color) { m_ModulationColors = std::vector<glm::vec4>(m_Render.vertexCount, color); FlagModulate(); }
	void SetModulationPerPoint(const std::vector<glm::vec4>& colors) { m_ModulationColors = colors; FlagModulate(); }
	void SetModulationPerPoint(std::vector<glm::vec4>&& colors) { m_ModulationColors = std::move(colors); FlagModulate(); }

	void CropPoints(const std::vector<glm::vec2>& points, int atlas_width, int atlas_height);
	void CropRelativePoints(const std::vector<glm::vec2>& atlas_points);

	TextureHandle GetTextureHandle() const { return m_Render.textureHandle; }
	const Renderable& GetRenderable() const { return m_Render; }

	void OnDraw(signed char texture_slot);

private:
	void BindBufferFuncs();
	void (ActorPrimitive2D::*f_BufferPackedP)(Stride) = nullptr;
	void (ActorPrimitive2D::*f_BufferPackedRS)(Stride) = nullptr;
	void (ActorPrimitive2D::*f_BufferPackedM)(Stride) = nullptr;
	void buffer_packed_p(Stride);
	void buffer_packed_p_default(Stride);
	void buffer_packed_rs(Stride);
	void buffer_packed_rs_default(Stride);
	void buffer_packed_m(Stride);
	void buffer_packed_m_default(Stride);
};

struct AP2D_Notification : public FickleNotification
{
	ActorPrimitive2D* prim = nullptr;

	AP2D_Notification(ActorPrimitive2D* prim) : prim(prim) {}

	void Notify(FickleSyncCode code) override
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
