#pragma once

#include <glm/glm.hpp>

#include "RectRender.h"

class AtlasPrimitive : public RectRender
{
	glm::vec4 m_AtlasSubrect;

public:
	AtlasPrimitive(const Transform2D& transform = Transform2D(), const TextureHandle& texture = 0, const ShaderHandle& shader = 0, const ZIndex& z = 0, const bool& visible = true);
	
	void SetAtlasTexel(glm::vec4 rect, int atlas_width, int atlas_height);
};
