#include "AtlasPrimitive.h"

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.h"

Renderable AtlasPrimitive::rect_renderable;

AtlasPrimitive::AtlasPrimitive(const Transform2D& transform, const TextureHandle& texture, const ShaderHandle& shader, const ZIndex& z, const bool& visible)
	: RectRender(transform, texture, shader, z, visible), m_AtlasTexel(0.0f)
{
	m_Render = rect_renderable;
	SetTextureHandle(texture);
	SetShaderHandle(shader);
}

void AtlasPrimitive::DefineRectRenderable()
{
	LOAD_STATUS load_status = loadRenderable(_RendererSettings::rect_renderable_filepath.c_str(), rect_renderable);
	if (load_status != LOAD_STATUS::OK)
	{
		Logger::LogErrorFatal("Could not load quad renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
	}
}

void AtlasPrimitive::SetAtlasTexel(glm::vec4 uv, int atlas_width, int atlas_height)
{
	glm::vec4 texel(0.0f);
	//texel[0] = (atlas_width - 1) * (uv[0] + uv[1] * (atlas_height + 1));
	//texel[1] = (atlas_width - 1) * (uv[0] + uv[2] + uv[1] * (atlas_height + 1));
	//texel[2] = (atlas_width - 1) * (uv[0] + uv[2] + (uv[1] + uv[3]) * (atlas_height + 1));
	//texel[3] = (atlas_width - 1) * (uv[0] + (uv[1] + uv[3]) * (atlas_height + 1));
	
	if (texel != m_AtlasTexel)
	{
		m_AtlasTexel = texel;
		auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
		m_Render.vertexBufferData[RectRender::end_attrib_pos] = m_AtlasTexel[0];
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 2] = atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 1] = atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + stride] = m_AtlasTexel[1];
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 2 + stride] = atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 1 + stride] = atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 2 * stride] = m_AtlasTexel[2];
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 2 + 2 * stride] = atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 1 + 2 * stride] = atlas_height;
		m_Render.vertexBufferData[RectRender::end_attrib_pos + 3 * stride] = m_AtlasTexel[3];
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 2 + 3 * stride] = atlas_width;
		m_Render.vertexBufferData[RectRender::end_attrib_pos - 1 + 3 * stride] = atlas_height;
	}
}
