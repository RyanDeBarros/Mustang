#pragma once

#include "ActorPrimitive.h"
#include "factory/TextureFactory.h"

class RectRender : public ActorPrimitive2D
{
protected:
	static Renderable rect_renderable;
	static constexpr Stride end_attrib_pos = ActorPrimitive2D::end_attrib_pos + 4;

	float m_UVWidth, m_UVHeight;
	glm::vec2 m_Pivot;

public:
	RectRender(TextureHandle texture = 0, const Transform2D& transform = {}, ShaderHandle shader = ShaderFactory::standard_shader, ZIndex z = 0, bool visible = true);
	RectRender(const RectRender&);
	RectRender(RectRender&&) noexcept;
	RectRender& operator=(const RectRender&);
	RectRender& operator=(RectRender&&) noexcept;
	
	static void DefineRectRenderable();

	inline int GetWidth() const { return TextureFactory::GetWidth(m_Render.textureHandle); }
	inline int GetHeight() const { return TextureFactory::GetHeight(m_Render.textureHandle); }
	// TODO when rect is cropped, width/height when setting scale is not accurate. Also, define SetWidth and SetHeight so that scale is not necessary.
	inline float GetUVWidth() const { return m_UVWidth; }
	inline float GetUVHeight() const { return m_UVHeight; }
	inline glm::vec2 GetPivot() const { return m_Pivot; }
	void SetPivot(float pivotX, float pivotY);
	inline void SetPivot(const glm::vec2& pivot) { SetPivot(pivot.x, pivot.y); }

	void CropToRect(glm::vec4 rect, int atlas_width, int atlas_height);
	void CropToRelativeRect(glm::vec4 rect);
	void ResetTransformUVs();
};
