#include "NonantRender.h"

#include "../CanvasLayer.h"
#include "utils/Data.inl"

NonantRender::NonantRender(NonantTile&& ntile, TextureVersion texture_version, const TextureSettings& texture_settings,
	ShaderHandle shader, ZIndex z, bool modulatable, bool visible)
	: ntile(std::move(ntile)), rects{
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(0),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(1),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(2),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(3),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(4),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(5),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(6),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(7),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(8),
			texture_version, texture_settings }), {}, shader, z, FickleType(true, modulatable), visible)
	}, FickleActor2D(FickleType(true, modulatable), z)
{
	m_Fickler.Attach(rects[0].Fickler());
	m_Fickler.Attach(rects[1].Fickler());
	m_Fickler.Attach(rects[2].Fickler());
	m_Fickler.Attach(rects[3].Fickler());
	m_Fickler.Attach(rects[4].Fickler());
	m_Fickler.Attach(rects[5].Fickler());
	m_Fickler.Attach(rects[6].Fickler());
	m_Fickler.Attach(rects[7].Fickler());
	m_Fickler.Attach(rects[8].Fickler());

	RecallibratePosition();
}

void NonantRender::RequestDraw(CanvasLayer* canvas_layer)
{
	rects[0].RequestDraw(canvas_layer);
	rects[1].RequestDraw(canvas_layer);
	rects[2].RequestDraw(canvas_layer);
	rects[3].RequestDraw(canvas_layer);
	rects[4].RequestDraw(canvas_layer);
	rects[5].RequestDraw(canvas_layer);
	rects[6].RequestDraw(canvas_layer);
	rects[7].RequestDraw(canvas_layer);
	rects[8].RequestDraw(canvas_layer);
}

void NonantRender::Reconfigure(const NonantLines_Absolute& lines)
{
	ntile.Reconfigure(lines);
	Recallibrate();
}

void NonantRender::Reconfigure(const NonantLines_Relative& lines)
{
	ntile.Reconfigure(lines);
	Recallibrate();
}

void NonantRender::Recallibrate()
{
	const_cast<Texture*>(TextureRegistry::Get(rects[0].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[1].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[2].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[3].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[4].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[5].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[6].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[7].GetTextureHandle()))->ReTexImage();
	const_cast<Texture*>(TextureRegistry::Get(rects[8].GetTextureHandle()))->ReTexImage();
	
	rects[0].RefreshTexture();
	rects[1].RefreshTexture();
	rects[2].RefreshTexture();
	rects[3].RefreshTexture();
	rects[4].RefreshTexture();
	rects[5].RefreshTexture();
	rects[6].RefreshTexture();
	rects[7].RefreshTexture();
	rects[8].RefreshTexture();

	RecallibratePosition();
}

void NonantRender::SetPivot(const glm::vec2& pvt)
{
	pivot = pvt;
	RecallibratePosition();
}

void NonantRender::SetNonantWidth(float width)
{
	nonantWidth = width;
	float inner_scale_x = rects[1].GetUVWidth() > 0
		? std::max(nonantWidth - rects[0].GetUVWidth() - rects[2].GetUVWidth(), 0.0f) / rects[1].GetUVWidth()
		: 0.0f;

	rects[1].Fickler().Scale()->x = inner_scale_x;
	rects[1].Fickler().transformable->SyncRS();
	rects[4].Fickler().Scale()->x = inner_scale_x;
	rects[4].Fickler().transformable->SyncRS();
	rects[7].Fickler().Scale()->x = inner_scale_x;
	rects[7].Fickler().transformable->SyncRS();

	RecallibratePosition();
}

void NonantRender::SetNonantHeight(float height)
{
	nonantHeight = height;
	float inner_scale_y = rects[3].GetUVHeight() > 0
		? std::max(nonantHeight - rects[0].GetUVHeight() - rects[6].GetUVHeight(), 0.0f) / rects[3].GetUVHeight()
		: 0.0f;

	rects[3].Fickler().Scale()->y = inner_scale_y;
	rects[3].Fickler().transformable->SyncRS();
	rects[4].Fickler().Scale()->y = inner_scale_y;
	rects[4].Fickler().transformable->SyncRS();
	rects[5].Fickler().Scale()->y = inner_scale_y;
	rects[5].Fickler().transformable->SyncRS();

	RecallibratePosition();
}

void NonantRender::SetNonantSize(const glm::vec2& size)
{
	nonantWidth = size.x;
	nonantHeight = size.y;
	float inner_scale_x = rects[1].GetUVWidth() > 0
		? std::max(nonantWidth - rects[0].GetUVWidth() - rects[2].GetUVWidth(), 0.0f) / rects[1].GetUVWidth()
		: 0.0f;
	float inner_scale_y = rects[3].GetUVHeight() > 0
		? std::max(nonantHeight - rects[0].GetUVHeight() - rects[6].GetUVHeight(), 0.0f) / rects[3].GetUVHeight()
		: 0.0f;

	rects[1].Fickler().Scale()->x = inner_scale_x;
	rects[1].Fickler().transformable->SyncRS();
	rects[3].Fickler().Scale()->y = inner_scale_y;
	rects[3].Fickler().transformable->SyncRS();
	rects[4].Fickler().Scale()->x = inner_scale_x;
	rects[4].Fickler().Scale()->y = inner_scale_y;
	rects[4].Fickler().transformable->SyncRS();
	rects[5].Fickler().Scale()->y = inner_scale_y;
	rects[5].Fickler().transformable->SyncRS();
	rects[7].Fickler().Scale()->x = inner_scale_x;
	rects[7].Fickler().transformable->SyncRS();

	RecallibratePosition();
}

void NonantRender::RecallibratePosition()
{
	// TODO add pivot
	float column_r_pos = ntile.ColumnPos(1) + rects[4].Fickler().Scale()->x * rects[4].GetUVWidth();
	float row_t_pos = ntile.RowPos(1) + rects[4].Fickler().Scale()->y * rects[4].GetUVHeight();
	float pvtX = pivot.x * nonantWidth, pvtY = pivot.y * nonantHeight;
	rects[0].Fickler().transformable->Position() = { ntile.ColumnPos(0) - pvtX, ntile.RowPos(0) - pvtY };
	rects[1].Fickler().transformable->Position() = { ntile.ColumnPos(1) - pvtX, rects[0].Fickler().transformable->Position().y };
	rects[2].Fickler().transformable->Position() = { column_r_pos - pvtX, rects[0].Fickler().transformable->Position().y };
	rects[3].Fickler().transformable->Position() = { rects[0].Fickler().transformable->Position().x, ntile.RowPos(1) - pvtY };
	rects[4].Fickler().transformable->Position() = { rects[1].Fickler().transformable->Position().x, rects[3].Fickler().transformable->Position().y };
	rects[5].Fickler().transformable->Position() = { rects[2].Fickler().transformable->Position().x, rects[3].Fickler().transformable->Position().y };
	rects[6].Fickler().transformable->Position() = { rects[0].Fickler().transformable->Position().x, row_t_pos - pvtY };
	rects[7].Fickler().transformable->Position() = { rects[1].Fickler().transformable->Position().x, rects[6].Fickler().transformable->Position().y };
	rects[8].Fickler().transformable->Position() = { rects[2].Fickler().transformable->Position().x, rects[6].Fickler().transformable->Position().y };
	m_Fickler.transformable->SyncP();
}
