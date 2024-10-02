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

namespace Fonts
{
	typedef int Codepoint;

	static constexpr const char8_t* COMMON = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,./<>?;:\'\"\\|[]{}!@#$%^&*()-=_+`~";
	static constexpr const char8_t* ALPHA_NUMERIC = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static constexpr const char8_t* NUMERIC = u8"0123456789";
	static constexpr const char8_t* ALPHA = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static constexpr const char8_t* ALPHA_LOWERCASE = u8"abcdefghijklmnopqrstuvwxyz";
	static constexpr const char8_t* ALPHA_UPPERCASE = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

typedef std::unordered_map<std::pair<Fonts::Codepoint, Fonts::Codepoint>, int> KerningMap;

struct KerningConstructArgs_map
{
	KerningMap map;

	KerningConstructArgs_map(const KerningMap& map) : map(map) {}
	KerningConstructArgs_map(KerningMap&& map) : map(std::move(map)) {}

	bool operator==(const KerningConstructArgs_map&) const = default;
};

struct KerningConstructArgs_filepath
{
	std::string filepath;

	KerningConstructArgs_filepath(const std::string& filepath) : filepath(filepath) {}
	KerningConstructArgs_filepath(std::string&& filepath) : filepath(std::move(filepath)) {}

	bool operator==(const KerningConstructArgs_filepath&) const = default;
};

namespace std
{
	template<>
	struct hash<KerningMap>
	{
		size_t operator()(const KerningMap& map) const
		{
			// TODO cap the iteration in other container hashes.
			static constexpr size_t max_elements = 20;
			size_t num_elements = 1;
			size_t hash_ = 0;

			for (const auto& [k, v] : map)
			{
				size_t h1 = hash<pair<Fonts::Codepoint, Fonts::Codepoint>>{}(k);
				size_t h2 = hash<int>{}(v);
				hash_ ^= (h1 ^ (h2 << 1));
				if (++num_elements > max_elements)
					break;
			}

			return hash_;
		}
	};

	template<>
	struct hash<KerningConstructArgs_map>
	{
		size_t operator()(const KerningConstructArgs_map& args) const { return hash<KerningMap>{}(args.map); }
	};

	template<>
	struct hash<KerningConstructArgs_filepath>
	{
		size_t operator()(const KerningConstructArgs_filepath& args) const { return hash<std::string>{}(args.filepath); }
	};
}

struct Kerning
{
	std::unordered_map<std::pair<Fonts::Codepoint, Fonts::Codepoint>, int> kern_map;

	Kerning(const KerningConstructArgs_map& args) : kern_map(args.map) {}
	Kerning(const KerningConstructArgs_filepath& args);

	// TODO maybe have a bool template parameter for Registry that requires bool overload on Element? Since there's at least 3 Elements now that just return constexpr true
	constexpr operator bool() const { return true; }
};

typedef Registry<Kerning, KerningHandle, KerningConstructArgs_filepath, KerningConstructArgs_map> KerningRegistry;

struct FontConstructArgs
{
	std::string font_filepath;
	float font_size;
	UTF::String common_buffer = Fonts::COMMON;
	TextureSettings settings = {};
	KerningHandle kerning = 0;

	FontConstructArgs(const std::string& font_filepath, float font_size, const UTF::String& common_buffer = Fonts::COMMON,
		const TextureSettings& settings = {}, KerningHandle kerning = 0)
		: font_filepath(font_filepath), font_size(font_size), common_buffer(common_buffer), settings(settings), kerning(kerning) {}
	FontConstructArgs(std::string&& font_filepath, float font_size, UTF::String&& common_buffer = Fonts::COMMON,
		const TextureSettings& settings = {}, KerningHandle kerning = 0)
		: font_filepath(std::move(font_filepath)), font_size(font_size), common_buffer(std::move(common_buffer)),
		settings(settings), kerning(std::move(kerning)) {}

	bool operator==(const FontConstructArgs&) const = default;
};

template<>
struct std::hash<FontConstructArgs>
{
	size_t operator()(const FontConstructArgs& args) const
	{
		auto h1 = std::hash<std::string>{}(args.font_filepath);
		auto h2 = std::hash<float>{}(args.font_size);
		auto h3 = std::hash<TextureSettings>{}(args.settings);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

class TextRender;

class Font
{
	friend class TextRender;
	
	struct Glyph
	{
		int gIndex = 0;
		int width = 0, height = 0;
		int ch_y0 = 0;
		int advance_width = 0, left_bearing = 0;
		TextureHandle texture = {};
		size_t buffer_pos = -1;
		Font* font = nullptr;
		unsigned char* location = nullptr;

		Glyph() = default;
		Glyph(Font* font, int gIndex, float scale, size_t buffer_pos);

		void RenderOnBitmap(unsigned char* bmp, size_t common_stride, size_t common_height, bool plus_one);
		void RenderOnBitmap(unsigned char* bmp);
		size_t Area() const { return static_cast<size_t>(width) * height; }
	};
	friend struct Glyph;

	std::unordered_map<Fonts::Codepoint, Font::Glyph> glyphs;
	stbtt_fontinfo font_info;
	float font_size;
	float scale = 0.0f;
	int ascent = 0, descent = 0, linegap = 0, baseline = 0;
	int space_width = 0;
	unsigned char* common_bmp = nullptr;
	size_t common_width = 0, common_height = 0;
	TextureSettings texture_settings;
	KerningHandle kerning = 0;

public:
	Font(const FontConstructArgs& args);
	Font(const Font&) = delete; // TODO copy/move constructors/assignments. utilize common_buffer
	Font(Font&&) noexcept;
	Font& operator=(Font&&) noexcept;
	~Font();

	constexpr operator bool() const { return true; }

	bool Cache(Fonts::Codepoint codepoint);
	void CacheAll(const Font& other);
	bool Supports(Fonts::Codepoint codepoint) const;
	int KerningOf(Fonts::Codepoint c1, Fonts::Codepoint c2, int g1, int g2, float sc = 1.0f) const;

	TextRender GetTextRender(const UTF::String& text, ZIndex z = 0);
	TextRender GetTextRender(UTF::String&& text = "", ZIndex z = 0);

	TextureSettings GetTextureSettings() const { return texture_settings; }
	void SetTextureSettings(const TextureSettings& ts = {});

private:
	int LineHeight(float line_spacing = 1.0f) const;
	std::array<glm::vec2, 4> UVs(const Glyph& glyph) const;
};

typedef Registry<Font, FontHandle, FontConstructArgs> FontRegistry;

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
	PageFormattingInfo FormatPage() const;

	struct FormattingData
	{
		int row = 0;
		int x = 0, y = 0;
		Fonts::Codepoint prev_codepoint = 0;
		int startX = 0, line_height = 0;
		LineFormattingInfo line = {};
		PageFormattingInfo page = {};

		void Setup(const TextRender& text_render);
		void NextLine(const TextRender& text_render);
		void AdvanceX(int amount) { x += amount; }
		void AdvanceX(int amount, Fonts::Codepoint codepoint) { x += amount; prev_codepoint = codepoint; }
		void AdvanceX(float amount) { x += static_cast<int>(roundf(amount)); }
		void AdvanceX(float amount, Fonts::Codepoint codepoint) { x += static_cast<int>(roundf(amount)); prev_codepoint = codepoint; }
		void KerningAdvanceX(const TextRender& text_render, const Font::Glyph& glyph, Fonts::Codepoint codepoint);
	};

	FormattingData formatting;

	struct BoundsFormattingData
	{
		int x = 0, y = 0;
		Fonts::Codepoint prev_codepoint = 0;
		int line_height = 0;
		int min_ch_y0 = 0, max_ch_y1 = 0;
		bool first_line = true;
		LineInfo line_info = {};
		LineFormattingInfo line_formatting = {};

		void Setup(const TextRender& text_render);
		void NextLine(TextRender& text_render);
		void LastLine(TextRender& text_render);
		void AdvanceX(int amount) { x += amount; }
		void AdvanceX(int amount, Fonts::Codepoint codepoint) { x += amount; prev_codepoint = codepoint; }
		void AdvanceX(float amount) { x += static_cast<int>(roundf(amount)); }
		void AdvanceX(float amount, Fonts::Codepoint codepoint) { x += static_cast<int>(roundf(amount)); prev_codepoint = codepoint; }
		void KerningAdvanceX(const TextRender& text_render, const Font::Glyph& glyph, Fonts::Codepoint codepoint);
		void UpdateMinCH_Y0(const Font::Glyph& glyph);
		void UpdateMaxCH_Y1(const Font::Glyph& glyph);
	};

	BoundsFormattingData bounds_formatting;

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

struct FontCharacteristics
{
	float font_size;
	unsigned short version = 0;

	FontCharacteristics(float font_size, unsigned short version = 0) : font_size(font_size), version(version) {}

	bool operator==(const FontCharacteristics&) const = default;
};

template<>
struct std::hash<FontCharacteristics>
{
	size_t operator()(const FontCharacteristics& f) const
	{
		return std::hash<float>{}(f.font_size) ^ (std::hash<unsigned short>{}(f.version) << 1);
	}
};

class FontFamily
{
	struct FontEntry
	{
		std::unordered_map<FontCharacteristics, FontHandle> fonts;
		std::string font_filepath;
		UTF::String common_buffer = Fonts::COMMON;
		TextureSettings settings = {};
		KerningHandle kerning;
	};

	std::unordered_map<std::string, FontEntry> family;

public:
	FontFamily(const char* filepath);

	Font* GetFont(const char* font_name, const FontCharacteristics& font_chars);
};
