#include "RectRender.h"

#include <string>

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.h"

Renderable RectRender::rect_renderable;

RectRender::RectRender(const Transform2D& transform, const TextureHandle& texture, const ShaderHandle& shader, const ZIndex& z, const bool& visible)
	: ActorPrimitive2D(rect_renderable, transform, z, visible)
{
	SetShaderHandle(shader);
	SetTextureHandle(texture);
	SetPivot(0.5, 0.5);
}

void RectRender::DefineRectRenderable()
{
	LOAD_STATUS load_status = loadRenderable(_RendererSettings::rect_renderable_filepath.c_str(), rect_renderable);
	if (load_status != LOAD_STATUS::OK)
	{
		Logger::LogErrorFatal("Could not load quad renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
	}
}

void RectRender::SetPivot(float pivotX, float pivotY)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	int width = GetWidth();
	int height = GetHeight();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos] = -pivotX * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1] = -pivotY * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + stride] = (1 - pivotX) * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + stride] = -pivotY * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 * stride] = (1 - pivotX) * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + 2 * stride] = (1 - pivotY) * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 * stride] = -pivotX * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + 3 * stride] = (1 - pivotY) * height;
}
