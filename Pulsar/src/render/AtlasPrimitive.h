#pragma once

#include <glm/glm.hpp>

#include "RectRender.h"

class AtlasPrimitive : public RectRender
{
	static Renderable rect_renderable;
	// (0,0), (1,0), (1,1), (0,1)
	glm::vec4 m_AtlasTexel;

public:
	AtlasPrimitive(const Transform2D& transform = Transform2D(), const TextureHandle& texture = 0, const ShaderHandle& shader = 0, const ZIndex& z = 0, const bool& visible = true);
	
	static void DefineRectRenderable();

	void SetAtlasTexel(glm::vec4 uv, int atlas_width, int atlas_height);
};
