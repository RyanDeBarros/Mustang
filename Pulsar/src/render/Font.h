#pragma once

#include <stb/stb_truetype.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <array>

#include "ActorRenderBase.h"
#include "Renderable.h"
#include "registry/Texture.h"
#include "utils/Functor.inl"
#include "utils/UTF.h"

class TextRender;

class Font
{
	friend class TextRender;
public:
	typedef int Codepoint;

private:
	struct Glyph
	{
		int gIndex;
		int width, height;
		int ch_y0;
		int advance_width, left_bearing;
		TextureHandle texture;
		std::array<glm::vec2, 4> uvs;
		Font* font = nullptr;
		unsigned char* location = nullptr;

		Glyph() = default;
		Glyph(Font* font, int gIndex, float scale);

		void RenderOnBitmap(unsigned char* bmp, int common_stride, int common_height, bool plus_one);
		void RenderOnBitmap(unsigned char* bmp);
		size_t Area() const { return static_cast<size_t>(width) * height; }
	};
	friend struct Glyph;

	std::unordered_map<Font::Codepoint, Font::Glyph> glyphs;
	stbtt_fontinfo font_info;
	float font_size;
	float scale = 0.0f;
	int ascent = 0, descent = 0, linegap = 0, baseline = 0;
	int space_width = 0;
	unsigned char* common_bmp = nullptr;
	size_t common_width = 0, common_height = 0;
	TextureSettings texture_settings;

public:
	static constexpr const char8_t* COMMON = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,./<>?;:\'\"\\|[]{}!@#$%^&*()-=_+`~";
	static constexpr const char8_t* ALPHA_NUMERIC = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static constexpr const char8_t* NUMERIC = u8"0123456789";
	static constexpr const char8_t* ALPHA = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static constexpr const char8_t* ALPHA_LOWERCASE = u8"abcdefghijklmnopqrstuvwxyz";
	static constexpr const char8_t* ALPHA_UPPERCASE = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	Font(const char* font_filepath, float font_size, const UTF::String& common_buffer = Font::COMMON,
		const TextureSettings& settings = {}, UTF::String* failed_common_chars = nullptr);
	Font(const Font&) = delete; // TODO copy/move constructors/assignments. utilize common_buffer
	Font(Font&&) = delete;
	~Font();

	bool Cache(Font::Codepoint codepoint);
	void CacheAll(const Font& other);

	TextRender GetTextRender(ZIndex z = 0);

	TextureSettings GetTextureSettings() const { return texture_settings; }
	void SetTextureSettings(const TextureSettings& ts = {});

private:
	int LineHeight(float line_spacing = 1.0f) const;
};

class TextRender : public FickleActor2D
{
	friend class Font;
	Font* font;

	Renderable renderable;
	// m_Status = 0b... transformM updated | transformRS updated | transformP updated | visible
	unsigned char status = 0b111;
	// TODO load dummy text renderable on Pulsar::Init(), and then set static stride based on its vertex layout.
	constexpr static Stride stride = 15;

	Functor<void, TextureSlot> on_draw_callback;
	static Functor<void, TextureSlot> create_on_draw_callback(TextRender* tr);

public:
	TextRender(Font* font, ZIndex z);
	TextRender(const TextRender&) = delete;
	TextRender(TextRender&&) = delete;

	// TODO FormatRegistry
	struct Format
	{
		float line_spacing_mult = 1.0f;
		float num_spaces_in_tab = 4;
		// TODO justification/alignment/underline/strikethrough/etc.
		// background color/drop-shadow/reflection/etc.
	};

	struct Bounds
	{
		int full_width = 0;
		int full_height = 0;
		int lowest_baseline = 0;
		int top_ribbon = 0;
	};

	UTF::String text;
	Format format = {};
	glm::vec2 pivot = { 0.0f, 1.0f };

	void RequestDraw(class CanvasLayer* canvas_layer) override;

	void SetVisible(bool visible) { status = (visible ? status |= 1 : status &= ~1); }
	bool IsVisible() const { return status & 0b1; }

	void FlagProteate() { status |= 0b1110; }
	void FlagTransform() { status |= 0b110; }
	void FlagTransformP() { status |= 0b10; }
	void FlagTransformRS() { status |= 0b100; }
	void FlagModulate() { status |= 0b1000; }

	void ChangeFont(Font* font_) { font = font_; }
	// TODO Block/rich text that tesselates TextRenders properly using bounds.
	Bounds GetBounds() const { return bounds; }
	void UpdateBounds();

private:
	void DrawGlyph(const Font::Glyph& glyph, int x, int y, CanvasLayer* canvas_layer);

	Bounds bounds;
};

struct TR_Notification : public FickleNotification
{
	TextRender* text = nullptr;

	TR_Notification(TextRender* text) : text(text) {}

	void Notify(FickleSyncCode code) override
	{
		switch (code)
		{
		case FickleSyncCode::SyncAll:
			if (text) text->FlagProteate();
			break;
		case FickleSyncCode::SyncT:
			if (text) text->FlagTransform();
			break;
		case FickleSyncCode::SyncP:
			if (text) text->FlagTransformP();
			break;
		case FickleSyncCode::SyncRS:
			if (text) text->FlagTransformRS();
			break;
		case FickleSyncCode::SyncM:
			if (text) text->FlagModulate();
			break;
		}
	}
};
