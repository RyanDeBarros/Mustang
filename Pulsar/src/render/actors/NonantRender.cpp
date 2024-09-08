#include "NonantRender.h"

#include "../CanvasLayer.h"
#include "utils/Data.inl"

NonantRender::NonantRender(NonantTile&& ntile, TextureVersion texture_version, const TextureSettings& texture_settings,
	ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible)
	: ntile(std::move(ntile)), rects{
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(0),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(1),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(2),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(3),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(4),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(5),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(6),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(7),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible),
		RectRender(TextureRegistry::GetHandle(TextureConstructArgs_tile{ this->ntile.GetTile(8),
			texture_version, texture_settings }), {}, shader, z, fickle_type, visible)
	}, FickleActor2D(fickle_type, z)
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

	RecallibrateFickler();
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
}

void NonantRender::RecallibrateFickler()
{
	set_ptr(rects[0].Fickler().Position(), { this->ntile.ColumnPos(0), this->ntile.RowPos(0) });
	set_ptr(rects[1].Fickler().Position(), { this->ntile.ColumnPos(1), this->ntile.RowPos(0) });
	set_ptr(rects[2].Fickler().Position(), { this->ntile.ColumnPos(2), this->ntile.RowPos(0) });
	set_ptr(rects[3].Fickler().Position(), { this->ntile.ColumnPos(0), this->ntile.RowPos(1) });
	set_ptr(rects[4].Fickler().Position(), { this->ntile.ColumnPos(1), this->ntile.RowPos(1) });
	set_ptr(rects[5].Fickler().Position(), { this->ntile.ColumnPos(2), this->ntile.RowPos(1) });
	set_ptr(rects[6].Fickler().Position(), { this->ntile.ColumnPos(0), this->ntile.RowPos(2) });
	set_ptr(rects[7].Fickler().Position(), { this->ntile.ColumnPos(1), this->ntile.RowPos(2) });
	set_ptr(rects[8].Fickler().Position(), { this->ntile.ColumnPos(2), this->ntile.RowPos(2) });
	m_Fickler.SyncAll();
}
