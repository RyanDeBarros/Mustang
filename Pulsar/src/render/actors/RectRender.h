#pragma once

#include "ActorPrimitive.h"
#include "registry/TextureRegistry.h"

class RectRender : public ActorPrimitive2D
{
protected:
	static Renderable rect_renderable;
	static constexpr Stride end_attrib_pos = ActorPrimitive2D::end_attrib_pos + 4;

	float m_UVWidth, m_UVHeight;
	glm::vec2 m_Pivot;

public:
	// TODO renderable should have empty index buffer, since it will not be used.
	RectRender(TextureHandle texture = 0, ShaderHandle shader = ShaderRegistry::standard_shader, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true);
	RectRender(const RectRender&);
	RectRender(RectRender&&) noexcept;
	RectRender& operator=(const RectRender&);
	RectRender& operator=(RectRender&&) noexcept;
	
	static void DefineRectRenderable();

	void RequestDraw(class CanvasLayer*) override;

	int GetWidth() const { return TextureRegistry::GetWidth(m_Render.textureHandle); }
	int GetHeight() const { return TextureRegistry::GetHeight(m_Render.textureHandle); }
	// TODO when rect is cropped, width/height when setting scale is not accurate. Also, define SetWidth and SetHeight so that scale is not necessary.
	float GetUVWidth() const { return m_UVWidth; }
	float GetUVHeight() const { return m_UVHeight; }
	glm::vec2 GetPivot() const { return m_Pivot; }
	void SetPivot(float pivotX, float pivotY);
	void SetPivot(const glm::vec2& pivot) { SetPivot(pivot.x, pivot.y); }

	void CropToRect(glm::vec4 rect, int atlas_width, int atlas_height);
	void CropToRelativeRect(glm::vec4 rect);
	void ResetTransformUVs();

	void SetTextureHandle(TextureHandle handle) override { ActorPrimitive2D::SetTextureHandle(handle); SetPivot(m_Pivot); }
};
