#include "RectRender.h"

#include <string>

#include "PulsarSettings.h"
#include "AssetLoader.h"
#include "Logger.inl"
#include "utils/Data.inl"
#include "render/CanvasLayer.h"

Renderable RectRender::rect_renderable;

static Functor<void, TextureSlot> create_on_draw_callback(RectRender* rr)
{
	return make_functor<true>([](TextureSlot slot, RectRender* rr) { rr->OnDraw(slot); }, rr);
}

RectRender::RectRender(TextureHandle texture, const glm::vec2& pivot, ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible)
	: ActorPrimitive2D(rect_renderable, z, fickle_type, visible), on_draw_callback(create_on_draw_callback(this))
{
	SetShaderHandle(shader);
	SetTextureHandle(texture);
	SetPivot(pivot);
	m_UVWidth = GetWidth();
	m_UVHeight = GetHeight();
}

RectRender::RectRender(const RectRender& other)
	: ActorPrimitive2D(other), m_UVWidth(other.m_UVWidth), m_UVHeight(other.m_UVHeight),
	m_Pivot(other.m_Pivot), on_draw_callback(create_on_draw_callback(this))
{
}

RectRender::RectRender(RectRender&& other) noexcept
	: ActorPrimitive2D(std::move(other)), m_UVWidth(other.m_UVWidth), m_UVHeight(other.m_UVHeight),
	m_Pivot(other.m_Pivot), on_draw_callback(create_on_draw_callback(this))
{
}

RectRender& RectRender::operator=(const RectRender& other)
{
	if (this != &other)
	{
		ActorPrimitive2D::operator=(other);
		m_UVWidth = other.m_UVWidth;
		m_UVHeight = other.m_UVHeight;
		SetPivot(other.m_Pivot);
		on_draw_callback = create_on_draw_callback(this);
	}
	return *this;
}

RectRender& RectRender::operator=(RectRender&& other) noexcept
{
	if (this != &other)
	{
		ActorPrimitive2D::operator=(std::move(other));
		m_UVWidth = other.m_UVWidth;
		m_UVHeight = other.m_UVHeight;
		SetPivot(other.m_Pivot);
		on_draw_callback = create_on_draw_callback(this);
	}
	return *this;
}

void RectRender::DefineRectRenderable()
{
	LOAD_STATUS load_status = Loader::loadRenderable(PulsarSettings::rect_renderable_filepath(), rect_renderable);
	if (load_status != LOAD_STATUS::OK)
		Logger::LogErrorFatal("Could not load rect renderable. Load Status = " + std::to_string(static_cast<int>(load_status)));
}

void RectRender::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawRect(m_Render, on_draw_callback);
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

void RectRender::RefreshTexture()
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	// TODO should these maybe be uv sizes? That might resolve the other todo issue.
	int width = GetWidth();
	int height = GetHeight();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos] = -m_Pivot.x * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1] = -m_Pivot.y * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + stride] = (1 - m_Pivot.x) * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + stride] = -m_Pivot.y * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 * stride] = (1 - m_Pivot.x) * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + 2 * stride] = (1 - m_Pivot.y) * height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 * stride] = -m_Pivot.x * width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 1 + 3 * stride] = (1 - m_Pivot.y) * height;
	CropToRelativeRect();
}

void RectRender::CropToRect(const glm::vec4& rect, int atlas_width, int atlas_height)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	m_UVWidth = static_cast<int>(rect[2]);
	m_UVHeight = static_cast<int>(rect[3]);
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = rect[0] / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = rect[1] / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + stride] = (rect[0] + rect[2]) / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + stride] = rect[1] / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 2 * stride] = (rect[0] + rect[2]) / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 2 * stride] = (rect[1] + rect[3]) / atlas_height;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 3 * stride] = rect[0] / atlas_width;
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 3 * stride] = (rect[1] + rect[3]) / atlas_height;
}

void RectRender::CropToRelativeRect(const glm::vec4& rect)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	m_UVWidth = static_cast<int>(rect[2] * GetWidth());
	m_UVHeight = static_cast<int>(rect[2] * GetHeight());
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
