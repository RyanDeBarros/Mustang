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
	TextureSettings texture_settings;

public:
	static constexpr const char* COMMON = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,./<>?;:\'\"\\|[]{}!@#$%^&*()-=_+`~ ";
	static constexpr const char* ALPHA_NUMERIC = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
	static constexpr const char* NUMERIC = "0123456789 ";
	static constexpr const char* ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	static constexpr const char* ALPHA_LOWERCASE = "abcdefghijklmnopqrstuvwxyz ";
	static constexpr const char* ALPHA_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	
	Font(const char* font_filepath, float font_size, const UTF::String& common_buffer = Font::COMMON,
		const TextureSettings& settings = {}, UTF::String* failed_common_chars = nullptr);
	Font(const Font&) = delete;
	Font(Font&&) = delete;
	~Font();

	bool Cache(Font::Codepoint codepoint);

	TextRender GetTextRender(ZIndex z = 0);

private:
	int LineHeight(float line_spacing = 1.0f) const;
};

class TextRender : public FickleActor2D
{
	friend class Font;
	Font* font;

	TextRender(Font* font, ZIndex z);
	TextRender(const TextRender&) = delete;
	TextRender(TextRender&&) = delete;

	Renderable renderable;
	// m_Status = 0b... transformM updated | transformRS updated | transformP updated | visible
	unsigned char status = 0b111;
	// TODO load dummy text renderable on Pulsar::Init(), and then set static stride based on its vertex layout.
	constexpr static Stride stride = 15;

	Functor<void, TextureSlot> on_draw_callback;
	static Functor<void, TextureSlot> create_on_draw_callback(TextRender* tr);

public:
	struct Format
	{
		float font_size_mult = 1.0f;
		float line_spacing_mult = 1.0f;
		float num_spaces_in_tab = 4;
		// TODO justification/alignment/underline/etc.
		// TODO background color/text color/etc.
	};

	UTF::String text;
	Format format;

	void RequestDraw(class CanvasLayer* canvas_layer) override;

	void SetVisible(bool visible) { status = (visible ? status |= 1 : status &= ~1); }
	bool IsVisible() const { return status & 0b1; }

	void FlagProteate() { status |= 0b1110; }
	void FlagTransform() { status |= 0b110; }
	void FlagTransformP() { status |= 0b10; }
	void FlagTransformRS() { status |= 0b100; }
	void FlagModulate() { status |= 0b1000; }

	// TODO width() and height() for bounds. this will make it easy to put text renders of different formats next to each other,
	// although it may be possible to abstract that too.

private:
	void DrawGlyph(const Font::Glyph& glyph, int x, int y, CanvasLayer* canvas_layer);
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
