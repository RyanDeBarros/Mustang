#include "RectRender.h"

#include <string>

#include "RendererSettings.h"
#include "AssetLoader.h"
#include "Logger.inl"
#include "utils/Data.inl"
#include "render/CanvasLayer.h"

Renderable RectRender::rect_renderable;

RectRender::RectRender(TextureHandle texture, ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible)
	: ActorPrimitive2D(rect_renderable, z, fickle_type, visible)
{
	SetShaderHandle(shader);
	SetTextureHandle(texture);
	SetPivot(0.5, 0.5);
	m_UVWidth = static_cast<float>(GetWidth());
	m_UVHeight = static_cast<float>(GetHeight());
}

RectRender::RectRender(const RectRender& other)
	: ActorPrimitive2D(other), m_UVWidth(other.m_UVWidth), m_UVHeight(other.m_UVHeight), m_Pivot(other.m_Pivot)
{
}

RectRender::RectRender(RectRender&& other) noexcept
	: ActorPrimitive2D(std::move(other)), m_UVWidth(other.m_UVWidth), m_UVHeight(other.m_UVHeight), m_Pivot(other.m_Pivot)
{
}

RectRender& RectRender::operator=(const RectRender& other)
{
	if (this == &other)
		return *this;
	ActorPrimitive2D::operator=(other);
	m_UVWidth = other.m_UVWidth;
	m_UVHeight = other.m_UVHeight;
	SetPivot(other.m_Pivot);
	return *this;
}

RectRender& RectRender::operator=(RectRender&& other) noexcept
{
	if (this == &other)
		return *this;
	ActorPrimitive2D::operator=(std::move(other));
	m_UVWidth = other.m_UVWidth;
	m_UVHeight = other.m_UVHeight;
	SetPivot(other.m_Pivot);
	return *this;
}

void RectRender::DefineRectRenderable()
{
	LOAD_STATUS load_status = Loader::loadRenderable(_RendererSettings::rect_renderable_filepath.c_str(), rect_renderable);
	if (load_status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Could not load rect renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
}

void RectRender::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawRect(this);
}

void RectRender::SetPivot(float pivotX, float pivotY)
{
	m_Pivot = { pivotX, pivotY };
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

void RectRender::CropToRect(glm::vec4 rect, int atlas_width, int atlas_height)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	m_UVWidth = rect[2];
	m_UVHeight = rect[3];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = rect[0] / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = rect[1] / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + stride] = (rect[0] + rect[2]) / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + stride] = rect[1] / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 2 * stride] = (rect[0] + rect[2]) / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 2 * stride] = (rect[1] + rect[3]) / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 3 * stride] = rect[0] / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 3 * stride] = (rect[1] + rect[3]) / atlas_height;
}

void RectRender::CropToRelativeRect(glm::vec4 rect)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	m_UVWidth = rect[2] * GetWidth();
	m_UVHeight = rect[2] * GetHeight();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = rect[0];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = rect[1];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + stride] = rect[0] + rect[2];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + stride] = rect[1];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 2 * stride] = rect[0] + rect[2];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 2 * stride] = rect[1] + rect[3];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 3 * stride] = rect[0];
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 3 * stride] = rect[1] + rect[3];
}

void RectRender::ResetTransformUVs()
{
	set_ptr(m_Fickler.Transform(), {});
	m_Fickler.SyncT();
	CropToRelativeRect({ 0.0f, 0.0f, 1.0f, 1.0f });
	SetPivot(0.5f, 0.5f);
}
