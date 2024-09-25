#pragma once

#include "ActorPrimitive.h"
#include "registry/TextureRegistry.h"
#include "utils/Functor.inl"

class RectRender : public ActorPrimitive2D
{
protected:
	static Renderable rect_renderable;
	static constexpr Stride end_attrib_pos = ActorPrimitive2D::end_attrib_pos + 4;

	int m_UVWidth, m_UVHeight;
	glm::vec2 m_Pivot;
	Functor<void, TextureSlot> on_draw_callback;

public:
	// TODO renderable should have empty index buffer, since it will not be used.
	RectRender(TextureHandle texture = 0, const glm::vec2& pivot = { 0.5f, 0.5f }, ShaderHandle shader = ShaderRegistry::standard_shader,
		ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);
	RectRender(const RectRender&);
	RectRender(RectRender&&) noexcept;
	RectRender& operator=(const RectRender&);
	RectRender& operator=(RectRender&&) noexcept;
	
	static void DefineRectRenderable();

	void RequestDraw(class CanvasLayer*) override;

	int GetWidth() const { return TextureRegistry::GetWidth(m_Render.textureHandle); }
	int GetHeight() const { return TextureRegistry::GetHeight(m_Render.textureHandle); }
	// TODO when rect is cropped, width/height when setting scale is not accurate. Also, define SetWidth and SetHeight so that scale is not necessary.
	int GetUVWidth() const { return m_UVWidth; }
	int GetUVHeight() const { return m_UVHeight; }
	glm::vec2 GetPivot() const { return m_Pivot; }
	void SetPivot(float pivotX, float pivotY);
	void SetPivot(const glm::vec2& pivot) { SetPivot(pivot.x, pivot.y); }
	void RefreshTexture();

	void CropToRect(const glm::vec4& rect, int atlas_width, int atlas_height);
	void CropToRelativeRect(const glm::vec4& rect = { 0.0f, 0.0f, 1.0f, 1.0f });
	void ResetTransformUVs();

	void SetTextureHandle(TextureHandle handle) override { ActorPrimitive2D::SetTextureHandle(handle); SetPivot(m_Pivot); }
};
