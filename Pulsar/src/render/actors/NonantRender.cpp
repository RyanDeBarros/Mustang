#include "NonantRender.h"

#include "../Renderer.h"
#include "../CanvasLayer.h"
#include "utils/Data.inl"

template<char i>
UVBounds NonantLines::UVs(float width, float height) const requires (i >= 0 && i <= 8)
{
	float cl = width != 0 ? col_l_width / width : 0.0f;
	float cr = width != 0 ? std::max(1.0f - col_r_width / width, cl) : 1.0f;
	float rb = height != 0 ? row_b_height / height : 0.0f;
	float rt = height != 0 ? std::max(1.0f - row_t_height / height, rb) : 1.0f;
	if constexpr (i == 0) return UVBounds(0.0f, cl, 0.0f, rb);
	else if constexpr (i == 1) return UVBounds(cl, cr, 0.0f, rb);
	else if constexpr (i == 2) return UVBounds(cr, 1.0f, 0.0f, rb);
	else if constexpr (i == 3) return UVBounds(0.0f, cl, rb, rt);
	else if constexpr (i == 4) return UVBounds(cl, cr, rb, rt);
	else if constexpr (i == 5) return UVBounds(cr, 1.0f, rb, rt);
	else if constexpr (i == 6) return UVBounds(0.0f, cl, rt, 1.0f);
	else if constexpr (i == 7) return UVBounds(cl, cr, rt, 1.0f);
	else if constexpr (i == 8) return UVBounds(cr, 1.0f, rt, 1.0f);
	else static_assert(false);
}

template UVBounds NonantLines::UVs<0>(float width, float height) const;
template UVBounds NonantLines::UVs<1>(float width, float height) const;
template UVBounds NonantLines::UVs<2>(float width, float height) const;
template UVBounds NonantLines::UVs<3>(float width, float height) const;
template UVBounds NonantLines::UVs<4>(float width, float height) const;
template UVBounds NonantLines::UVs<5>(float width, float height) const;
template UVBounds NonantLines::UVs<6>(float width, float height) const;
template UVBounds NonantLines::UVs<7>(float width, float height) const;
template UVBounds NonantLines::UVs<8>(float width, float height) const;

NonantRender::NonantRender(TextureHandle texture, const NonantLines& lines, const glm::vec2& pivot, ShaderHandle shader, ZIndex z, bool modulatable, bool visible)
	: RectRender(texture, { 0.0f, 0.0f }, shader == ShaderRegistry::HANDLE_CAP ? Renderer::Shaders().Standard() : shader, z, FickleType(true, modulatable), visible), lines(lines)
{
	nonantWidth = static_cast<float>(m_UVWidth);
	nonantHeight = static_cast<float>(m_UVHeight);
	SetPivot(pivot);
}

void NonantRender::RequestDraw(CanvasLayer* canvas_layer)
{
	auto stride = Render::StrideCountOf(m_Render.model.layout, m_Render.model.layoutMask);
	PackedTransform2D prior = m_Fickler.transformable->self;
	Draw<0>(canvas_layer, stride, prior);
	Draw<1>(canvas_layer, stride, prior);
	Draw<2>(canvas_layer, stride, prior);
	Draw<3>(canvas_layer, stride, prior);
	Draw<4>(canvas_layer, stride, prior);
	Draw<5>(canvas_layer, stride, prior);
	Draw<6>(canvas_layer, stride, prior);
	Draw<7>(canvas_layer, stride, prior);
	Draw<8>(canvas_layer, stride, prior);
	m_Fickler.transformable->self = prior;
}

template<char i>
void NonantRender::Draw(CanvasLayer* canvas_layer, Stride stride, const PackedTransform2D& prior) requires (i >= 0 && i <= 8)
{
	m_Fickler.transformable->self.transform = CallibrateTransform<i>(SetUVs<i>(stride));
	m_Fickler.transformable->self.packedP = { 0.0f, 0.0f };
	m_Fickler.transformable->self.packedRS = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_Fickler.transformable->self.Sync(prior);
	m_Fickler.transformable->SyncChildren();
	RectRender::RequestDraw(canvas_layer);
}

void NonantRender::SetNonantWidth(float width)
{
	nonantWidth = std::max(width, lines.col_l_width + lines.col_r_width);
	nonantXF = nonantWidth / m_UVWidth;
}

void NonantRender::SetNonantHeight(float height)
{
	nonantHeight = std::max(height, lines.row_b_height + lines.row_t_height);
	nonantYF = nonantHeight / m_UVHeight;
}

template<char i>
UVBounds NonantRender::SetUVs(Stride stride) requires (i >= 0 && i <= 8)
{
	auto uvs = lines.UVs<i>(static_cast<float>(m_UVWidth), static_cast<float>(m_UVHeight));
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2] = uvs.getX<0>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3] = uvs.getY<0>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + stride] = uvs.getX<1>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + stride] = uvs.getY<1>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 2 * stride] = uvs.getX<2>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 2 * stride] = uvs.getY<2>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 2 + 3 * stride] = uvs.getX<3>();
	m_Render.vertexBufferData[ActorPrimitive2D::end_attrib_pos + 3 + 3 * stride] = uvs.getY<3>();
	return uvs;
}

template<char i>
Transform2D NonantRender::CallibrateTransform(const UVBounds& uvs) requires (i >= 0 && i <= 8)
{
	Transform2D transform{};
	if constexpr (i % 3 == 0)
	{
		transform.position.x = -m_Pivot.x * nonantWidth;
		transform.scale.x = uvs.x2;
	}
	else if constexpr (i % 3 == 1)
	{
		transform.position.x = lines.col_l_width - m_Pivot.x * nonantWidth;
		transform.scale.x = std::max(0.0f, nonantXF + uvs.x2 - uvs.x1 - 1.0f);
	}
	else if constexpr (i % 3 == 2)
	{
		transform.position.x = nonantWidth - lines.col_r_width - m_Pivot.x * nonantWidth;
		transform.scale.x = 1.0f - uvs.x1;
	}
	else static_assert(false);
	if constexpr (i / 3 == 0)
	{
		transform.position.y = -m_Pivot.y * nonantHeight;
		transform.scale.y = uvs.y2;
	}
	else if constexpr (i / 3 == 1)
	{
		transform.position.y = lines.row_b_height - m_Pivot.y * nonantHeight;
		transform.scale.y = std::max(0.0f, nonantYF + uvs.y2 - uvs.y1 - 1.0f);
	}
	else if constexpr (i / 3 == 2)
	{
		transform.position.y = nonantHeight - lines.row_t_height - m_Pivot.y * nonantHeight;
		transform.scale.y = 1.0f - uvs.y1;
	}
	else static_assert(false);
	return transform;
}
