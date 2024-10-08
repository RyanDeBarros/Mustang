#pragma once

#include "ActorPrimitive.h"
#include "registry/Texture.h"
#include "../Renderer.h"
#include "utils/Functor.inl"

class RectRender : public ActorPrimitive2D
{
protected:
	static Renderable* rect_renderable;
	static constexpr Stride end_attrib_pos = ActorPrimitive2D::end_attrib_pos + 4;

	int m_UVWidth, m_UVHeight;
	glm::vec2 m_Pivot;
	Functor<void, TextureSlot> on_draw_callback;

public:
	RectRender(TextureHandle texture = 0, const glm::vec2& pivot = { 0.5f, 0.5f }, ShaderHandle shader = ShaderRegistry::HANDLE_CAP,
		ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);
	RectRender(const RectRender&);
	RectRender(RectRender&&) noexcept;
	RectRender& operator=(const RectRender&);
	RectRender& operator=(RectRender&&) noexcept;
	
	static void DefineRectRenderable();
	static void DestroyRectRenderable();

	virtual void RequestDraw(class CanvasLayer*) override;

	int GetWidth() const { return Renderer::Textures().GetWidth(m_Render.textureHandle); }
	int GetHeight() const { return Renderer::Textures().GetHeight(m_Render.textureHandle); }
	// TODO when rect is cropped, width/height when setting scale is not accurate. Also, define SetWidth and SetHeight so that scale is not necessary.
	int GetUVWidth() const { return m_UVWidth; }
	int GetUVHeight() const { return m_UVHeight; }
	glm::vec2 GetPivot() const { return m_Pivot; }
	virtual void SetPivot(float pivotX, float pivotY);
	virtual void SetPivot(const glm::vec2& pivot) { SetPivot(pivot.x, pivot.y); }
	void RefreshTexture();

	void CropToRect(const glm::vec4& rect, int atlas_width, int atlas_height);
	void CropToRelativeRect(const glm::vec4& rect = { 0.0f, 0.0f, 1.0f, 1.0f });
	void ResetTransformUVs();

	void SetTextureHandle(TextureHandle handle) override { ActorPrimitive2D::SetTextureHandle(handle); SetPivot(m_Pivot); }
};
