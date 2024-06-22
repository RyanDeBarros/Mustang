#include "AtlasPrimitive.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.h"

AtlasPrimitive::AtlasPrimitive(const Transform2D& transform, const TextureHandle& texture, const ShaderHandle& shader, const ZIndex& z, const bool& visible)
	: RectRender(transform, texture, shader, z, visible), m_AtlasSubrect(0.0f)
{
}

void AtlasPrimitive::SetAtlasTexel(glm::vec4 rect, int atlas_width, int atlas_height)
{
	if (rect != m_AtlasSubrect)
	{
		m_AtlasSubrect = rect;
		auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
		m_Render.vertexBufferData[RectRender::end_attrib_pos] = m_AtlasSubrect[0] / atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 1] = m_AtlasSubrect[1] / atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + stride] = (m_AtlasSubrect[0] + m_AtlasSubrect[2]) / atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 1 + stride] = m_AtlasSubrect[1] / atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 2 * stride] = (m_AtlasSubrect[0] + m_AtlasSubrect[2]) / atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 1 + 2 * stride] = (m_AtlasSubrect[1] + m_AtlasSubrect[3]) / atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 3 * stride] = m_AtlasSubrect[0] / atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 1 + 3 * stride] = (m_AtlasSubrect[1] + m_AtlasSubrect[3]) / atlas_height;
	}
}
