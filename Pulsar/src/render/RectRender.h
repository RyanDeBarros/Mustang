#pragma once

#include "ActorPrimitive.h"
#include "Transform.h"
#include "factory/TextureFactory.h"

class RectRender : public ActorPrimitive2D
{
protected:
	static Renderable rect_renderable;
	static constexpr Stride end_attrib_pos = ActorPrimitive2D::end_attrib_pos + 4;

public:
	RectRender(const Transform2D& transform = Transform2D(), const TextureHandle& texture = 0, const ShaderHandle& shader = 0, const ZIndex& z = 0, const bool& visible = true);
	static void DefineRectRenderable();

	inline int GetWidth() const { return TextureFactory::GetWidth(m_Render.textureHandle); }
	inline int GetHeight() const { return TextureFactory::GetHeight(m_Render.textureHandle); }
	void SetPivot(float pivotX, float pivotY);

	void CropToRect(glm::vec4 rect, int atlas_width, int atlas_height);
	void CropToRelativeRect(glm::vec4 rect);
	void ResetTransformUVs();
};
