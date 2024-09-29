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

template<>
struct std::hash<std::pair<int, int>>
{
	size_t operator()(const std::pair<int, int>& p) const
	{
		return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
	}
};

class TextRender;

class Font
{
	friend class TextRender;
public:
	typedef int Codepoint;
	// TODO internal Kerning registry
	typedef std::unordered_map<std::pair<Font::Codepoint, Font::Codepoint>, int> Kerning;

private:
	struct Glyph
	{
		int gIndex;
		int width, height;
		int ch_y0;
		int advance_width, left_bearing;
		TextureHandle texture;
		size_t buffer_pos = -1;
		Font* font = nullptr;
		unsigned char* location = nullptr;

		Glyph() = default;
		Glyph(Font* font, int gIndex, float scale, size_t buffer_pos);

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
	Kerning kerning; // TODO kerning handle?

public:
	static constexpr const char8_t* COMMON = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,./<>?;:\'\"\\|[]{}!@#$%^&*()-=_+`~";
	static constexpr const char8_t* ALPHA_NUMERIC = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static constexpr const char8_t* NUMERIC = u8"0123456789";
	static constexpr const char8_t* ALPHA = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static constexpr const char8_t* ALPHA_LOWERCASE = u8"abcdefghijklmnopqrstuvwxyz";
	static constexpr const char8_t* ALPHA_UPPERCASE = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	Font(const char* font_filepath, float font_size, const UTF::String& common_buffer = Font::COMMON,
		const TextureSettings& settings = {}, UTF::String* failed_common_chars = nullptr, const Kerning& kerning = Kerning());
	Font(const Font&) = delete; // TODO copy/move constructors/assignments. utilize common_buffer
	Font(Font&&) noexcept;
	Font& operator=(Font&&) noexcept;
	~Font();

	bool Cache(Font::Codepoint codepoint);
	void CacheAll(const Font& other);
	bool Supports(Font::Codepoint codepoint) const;
	int KerningOf(Font::Codepoint c1, Font::Codepoint c2, int g1, int g2, float sc = 1.0f) const;

	TextRender GetTextRender(const UTF::String& text, ZIndex z = 0);
	TextRender GetTextRender(UTF::String&& text = "", ZIndex z = 0);

	TextureSettings GetTextureSettings() const { return texture_settings; }
	void SetTextureSettings(const TextureSettings& ts = {});

private:
	int LineHeight(float line_spacing = 1.0f) const;
	std::array<glm::vec2, 4> UVs(const Glyph& glyph) const;
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
	TextRender(Font* font, ZIndex z, const UTF::String& txt);
	TextRender(Font* font, ZIndex z, UTF::String&& txt);
	TextRender(const TextRender&) = delete;
	TextRender(TextRender&&) = delete;

	enum class HorizontalAlign : char
	{
		LEFT,
		RIGHT,
		CENTER,
		JUSTIFY,
		JUSTIFY_GLYPHS
	};

	enum class VerticalAlign : char
	{
		TOP,
		MIDDLE,
		BOTTOM,
		JUSTIFY,
		JUSTIFY_LINEBREAKS
	};

	// TODO FormatRegistry
	struct Format
	{
		float line_spacing_mult = 1.0f;
		float num_spaces_in_tab = 4;
		HorizontalAlign horizontal_align = HorizontalAlign::LEFT;
		VerticalAlign vertical_align = VerticalAlign::TOP;
		// TODO underline/strikethrough/etc.
		// background color/drop-shadow/reflection/etc.
		int min_width = 0, min_height = 0;
	};

	struct LineInfo
	{
		int width = 0;
		int num_spaces = 0;
		int num_tabs = 0;
	};

	struct LineFormattingInfo
	{
		int add_x = 0;
		float mul_x = 1.0f;
		float space_mul_x = 1.0f;
	};

	struct PageFormattingInfo
	{
		int add_y = 0;
		float mul_y = 1.0f;
		float linebreak_mul_y = 1.0f;
	};

	struct Bounds
	{
		int inner_width = 0;
		int inner_height = 0;
		int lowest_baseline = 0;
		int top_ribbon = 0;
		int bottom_ribbon = 0;
		int num_linebreaks = 0;
		std::vector<LineInfo> lines;
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
	int OuterWidth() const { return bounds.inner_width > format.min_width ? bounds.inner_width : format.min_width; }
	int OuterHeight() const { return bounds.inner_height > format.min_height ? bounds.inner_height : format.min_height; }

	void WarnInvalidCharacters() const;

private:
	void DrawGlyph(const Font::Glyph& glyph, int x, int y, CanvasLayer* canvas_layer);
	void FormatLine(size_t line, LineFormattingInfo& line_formatting) const;
	void FormatPage(PageFormattingInfo& page_formatting) const;

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

struct FontConstructorArgs
{
	float font_size;
	UTF::String common_buffer = Font::COMMON;
	TextureSettings settings = {};
	UTF::String* failed_common_chars = nullptr;

	FontConstructorArgs(float font_size, const UTF::String& common_buffer = Font::COMMON,
		const TextureSettings& settings = {}, UTF::String* failed_common_chars = nullptr)
		: font_size(font_size), common_buffer(common_buffer), settings(settings), failed_common_chars(failed_common_chars) {}

	FontConstructorArgs(float font_size, UTF::String&& common_buffer = Font::COMMON,
		const TextureSettings& settings = {}, UTF::String* failed_common_chars = nullptr)
		: font_size(font_size), common_buffer(std::move(common_buffer)), settings(settings), failed_common_chars(failed_common_chars) {}
};

// TODO FontFamily registry instead of/in addition to Font registry?
class FontFamily
{
	struct FontEntry
	{
		std::string filepath;
		Font::Kerning kerning;
		// TODO is vector okay for this? There would be < 20 different font sizes probably.
		std::vector<std::pair<float, Font>> fonts;

		Font* CreateFont(const FontConstructorArgs& args)
		{
			for (auto it = fonts.begin(); it != fonts.end(); ++it)
			{
				if (it->first == args.font_size)
					return &it->second;
			}
			fonts.push_back({args.font_size,
				Font(filepath.c_str(), args.font_size, args.common_buffer, args.settings, args.failed_common_chars, kerning)});
			return &fonts[fonts.size() - 1].second;
		}
	};

	std::unordered_map<std::string, FontEntry> family;

public:
	FontFamily(const char* filepath);

	Font* GetFont(const char* font_name, const FontConstructorArgs& args)
	{
		auto iter = family.find(font_name);
		return iter != family.end() ? iter->second.CreateFont(args) : nullptr;
	}
};
