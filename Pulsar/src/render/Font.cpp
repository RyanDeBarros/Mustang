#include "Font.h"

#include <toml/toml.hpp>

#include "IO.h"
#include "Logger.inl"
#include "CanvasLayer.h"
#include "AssetLoader.h"

Font::Glyph::Glyph(Font* font, int gIndex, float scale, size_t buffer_pos)
	: gIndex(gIndex), font(font), texture(0), buffer_pos(buffer_pos)
{
	stbtt_GetGlyphHMetrics(&font->font_info, gIndex, &advance_width, &left_bearing);
	int ch_x0, ch_x1, ch_y1;
	stbtt_GetGlyphBitmapBox(&font->font_info, gIndex, scale, scale, &ch_x0, &ch_y0, &ch_x1, &ch_y1);
	width = ch_x1 - ch_x0;
	height = ch_y1 - ch_y0;
}

void Font::Glyph::RenderOnBitmap(unsigned char* bmp, int common_stride, int common_height, bool plus_one)
{
	unsigned char* temp = new unsigned char[width * height];
	stbtt_MakeGlyphBitmap(&font->font_info, temp, width, height, width, font->scale, font->scale, gIndex);
	for (int row = 0; row < height; ++row)
		memcpy(bmp + row * common_stride, temp + row * width, width);
	delete[] temp;
	for (int row = height; row < common_height; ++row)
		memset(bmp + row * common_stride, 0x0, width);
	if (plus_one)
		for (int row = 0; row < common_height; ++row)
			*(bmp + row * common_stride + width) = 0x0;
	location = bmp;
}

void Font::Glyph::RenderOnBitmap(unsigned char* bmp)
{
	stbtt_MakeGlyphBitmap(&font->font_info, bmp, width, height, width, font->scale, font->scale, gIndex);
	location = bmp;
}

Font::Font(const char* font_filepath, float font_size, const UTF::String& common_buffer,
	const TextureSettings& settings, UTF::String* failed_common_chars, const Kerning& kerning)
	: font_size(font_size), texture_settings(settings), font_info{}, kerning(kerning)
{
	unsigned char* font_file;
	size_t font_filesize;
	if (!IO::read_file_uc(font_filepath, font_file, font_filesize))
	{
		Logger::LogErrorFatal("Cannot load font file.");
		return;
	}
	if (!stbtt_InitFont(&font_info, font_file, 0))
	{
		Logger::LogErrorFatal("Cannot init font.");
		return;
	}

	scale = stbtt_ScaleForPixelHeight(&font_info, font_size);
	stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &linegap);
	baseline = static_cast<int>(roundf(ascent * scale));

	std::vector<Font::Codepoint> codepoints;
	auto iter = common_buffer.begin();
	while (iter)
	{
		int codepoint = iter.advance();
		if (glyphs.find(codepoint) != glyphs.end())
			continue;
		int gIndex = stbtt_FindGlyphIndex(&font_info, codepoint);
		if (!gIndex)
		{
			if (failed_common_chars)
				failed_common_chars->push_back(codepoint);
			continue;
		}
		Glyph glyph(this, gIndex, scale, common_width);
		common_width += glyph.width + size_t(1);
		if (glyph.height > common_height)
			common_height = glyph.height;
		glyphs.insert({ codepoint, std::move(glyph) });
		codepoints.push_back(codepoint);
	}
	if (common_width > 0)
	{
		common_bmp = new unsigned char[common_width * common_height];
		for (Font::Codepoint codepoint : codepoints)
		{
			Font::Glyph& glyph = glyphs[codepoint];
			glyph.RenderOnBitmap(common_bmp + glyph.buffer_pos, common_width, common_height, true);
		}
		TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(common_bmp,
			static_cast<int>(common_width), static_cast<int>(common_height), 1, TileDeletionPolicy::FROM_EXTERNAL, false));
		TextureHandle texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
		for (Font::Codepoint codepoint : codepoints)
		{
			glyphs[codepoint].texture = texture;
		}
	}
	auto space = glyphs.find(' ');
	if (space == glyphs.end())
	{
		int space_advance_width, space_left_bearing;
		stbtt_GetCodepointHMetrics(&font_info, ' ', &space_advance_width, &space_left_bearing);
		space_width = static_cast<int>(roundf(space_advance_width * scale));
	}
	else
		space_width = space->second.width;
}

// TODO put into separate struct for simplicity
Font::Font(Font&& other) noexcept
	: glyphs(std::move(other.glyphs)), font_info(std::move(other.font_info)), font_size(other.font_size), scale(other.scale),
	ascent(other.ascent), descent(other.descent), linegap(other.linegap), baseline(other.baseline), space_width(other.space_width),
	common_bmp(other.common_bmp), common_width(other.common_width), common_height(other.common_height),
	texture_settings(other.texture_settings), kerning(std::move(other.kerning))
{
	other.common_bmp = nullptr;
}

Font& Font::operator=(Font&& other) noexcept
{
	if (this != &other)
	{
		glyphs = std::move(other.glyphs);
		font_info = std::move(other.font_info);
		font_size = other.font_size;
		scale = other.scale;
		ascent = other.ascent;
		descent = other.descent;
		linegap = other.linegap;
		baseline = other.baseline;
		space_width = other.space_width;
		common_bmp = other.common_bmp;
		common_width = other.common_width;
		common_height = other.common_height;
		texture_settings = other.texture_settings;
		kerning = std::move(other.kerning);
		other.common_bmp = nullptr;
	}
	return *this;
}

Font::~Font()
{
	if (common_bmp)
		delete[] common_bmp;
}

bool Font::Cache(Font::Codepoint codepoint)
{
	if (glyphs.find(codepoint) != glyphs.end())
		return true;
	if (int gIndex = stbtt_FindGlyphIndex(&font_info, codepoint))
	{
		Font::Glyph glyph(this, gIndex, scale, -1);
		unsigned char* bmp = new unsigned char[glyph.Area()];
		glyph.RenderOnBitmap(bmp);
		TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(bmp, glyph.width, glyph.height, 1, TileDeletionPolicy::FROM_NEW, false));
		TextureHandle texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
		glyph.texture = texture;
		glyphs.insert({ codepoint, std::move(glyph) });
		return true;
	}
	return false;
}

void Font::CacheAll(const Font& other)
{
	for (const auto& [codepoint, glyph] : other.glyphs)
		Cache(codepoint);
}

bool Font::Supports(Font::Codepoint codepoint) const
{
	if (glyphs.find(codepoint) != glyphs.end())
		return true;
	return stbtt_FindGlyphIndex(&font_info, codepoint) != 0;
}

int Font::KerningOf(Font::Codepoint c1, Font::Codepoint c2, int g1, int g2, float sc) const
{
	if (g1 == 0)
		return 0;
	auto k = kerning.find({ c1, c2 });
	if (k != kerning.end())
		return static_cast<int>(roundf(k->second * scale * sc));
	else
		return static_cast<int>(roundf(stbtt_GetGlyphKernAdvance(&font_info, g1, g2) * scale * sc));
}

TextRender Font::GetTextRender(const UTF::String& text, ZIndex z)
{
	return TextRender(this, z, text);
}

TextRender Font::GetTextRender(UTF::String&& text, ZIndex z)
{
	return TextRender(this, z, std::move(text));
}

void Font::SetTextureSettings(const TextureSettings& ts)
{
	texture_settings = ts;
	TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(common_bmp,
		static_cast<int>(common_width), static_cast<int>(common_height), 1, TileDeletionPolicy::FROM_EXTERNAL, false));
	TextureHandle common_texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
	TextureRegistry::SetSettings(common_texture, ts);
	for (const auto& [codepoint, glyph] : glyphs)
	{
		if (glyph.texture != common_texture)
			TextureRegistry::SetSettings(glyph.texture, ts);
	}
}

int Font::LineHeight(float line_spacing) const
{
	return static_cast<int>(roundf((ascent - descent + linegap) * scale * line_spacing));
}

std::array<glm::vec2, 4> Font::UVs(const Glyph& glyph) const
{
	if (glyph.buffer_pos != size_t(-1))
	{
		float uvx1 = static_cast<float>(glyph.buffer_pos) / common_width;
		float uvx2 = static_cast<float>(glyph.buffer_pos + glyph.width) / common_width;
		float uvy1 = 0.0f;
		float uvy2 = static_cast<float>(glyph.height) / common_height;
		return std::array<glm::vec2, 4>{
			glm::vec2{ uvx1, uvy1 },
			glm::vec2{ uvx2, uvy1 },
			glm::vec2{ uvx2, uvy2 },
			glm::vec2{ uvx1, uvy2 }
		};
	}
	else return std::array<glm::vec2, 4>{
		glm::vec2{ 0.0f, 0.0f },
		glm::vec2{ 1.0f, 0.0f },
		glm::vec2{ 1.0f, 1.0f },
		glm::vec2{ 0.0f, 1.0f }
	};
}

static bool carriage_return_1(Font::Codepoint codepoint)
{
	return codepoint == '\n' || codepoint == '\r';
}

static bool carriage_return_2(Font::Codepoint r, Font::Codepoint n)
{
	return r == '\r' && n == '\n';
}

Functor<void, TextureSlot> TextRender::create_on_draw_callback(TextRender* tr)
{
	return make_functor<true>([](TextureSlot slot, TextRender* tr) {
		for (VertexBufferCounter i = 0; i < tr->renderable.vertexCount; ++i)
			tr->renderable.vertexBufferData[0 + i * TextRender::stride] = static_cast<GLfloat>(slot);
		}, tr);
}

TextRender::TextRender(Font* font, ZIndex z, const UTF::String& txt)
	: FickleActor2D(FickleType::Protean, z), font(font), on_draw_callback(create_on_draw_callback(this)), text(txt)
{
	Loader::loadRenderable(PulsarSettings::text_standard_filepath(), renderable);
	UpdateBounds();
}

TextRender::TextRender(Font* font, ZIndex z, UTF::String&& txt)
	: FickleActor2D(FickleType::Protean, z), font(font), on_draw_callback(create_on_draw_callback(this)), text(std::move(txt))
{
	Loader::loadRenderable(PulsarSettings::text_standard_filepath(), renderable);
	UpdateBounds();
}

void TextRender::FormatLine(size_t line, LineFormattingInfo& line_formatting) const
{
	line_formatting = {};
	
	if (format.horizontal_align == HorizontalAlign::RIGHT)
		line_formatting.add_x = OuterWidth() - bounds.lines[line].width;
	else if (format.horizontal_align == HorizontalAlign::CENTER)
		line_formatting.add_x = 0.5f * (OuterWidth() - bounds.lines[line].width);
	else if (format.horizontal_align == HorizontalAlign::JUSTIFY_GLYPHS)
	{
		if (bounds.lines[line].width)
			line_formatting.mul_x = static_cast<float>(OuterWidth()) / bounds.lines[line].width;
		line_formatting.space_mul_x = line_formatting.mul_x;
	}
	else if (format.horizontal_align == HorizontalAlign::JUSTIFY)
	{
		int num_spaces = bounds.lines[line].num_spaces + format.num_spaces_in_tab * bounds.lines[line].num_tabs;
		if (num_spaces)
			line_formatting.space_mul_x += static_cast<float>(OuterWidth() - bounds.lines[line].width) / (num_spaces * font->space_width);
	}
}

void TextRender::FormatPage(PageFormattingInfo& page_formatting) const
{
	if (format.vertical_align == VerticalAlign::BOTTOM)
		page_formatting.add_y = OuterHeight() - bounds.inner_height;
	else if (format.vertical_align == VerticalAlign::MIDDLE)
		page_formatting.add_y = 0.5f * (OuterHeight() - bounds.inner_height);
	else if (format.vertical_align == VerticalAlign::JUSTIFY)
	{
		int line_height = font->LineHeight(format.line_spacing_mult);
		if (bounds.inner_height != line_height)
			page_formatting.mul_y = static_cast<float>(OuterHeight() - line_height) / (bounds.inner_height - line_height);
		page_formatting.linebreak_mul_y = page_formatting.mul_y;
	}
	else if (format.vertical_align == VerticalAlign::JUSTIFY_LINEBREAKS)
	{
		int line_height = font->LineHeight(format.line_spacing_mult);
		if (bounds.num_linebreaks * line_height != 0)
			page_formatting.linebreak_mul_y += static_cast<float>(OuterHeight() - bounds.inner_height) / (bounds.num_linebreaks * line_height);
	}
}

void TextRender::RequestDraw(CanvasLayer* canvas_layer)
{
	if ((status & 0b1) == 0b0)
		return;
	int line_height = font->LineHeight(format.line_spacing_mult);
	int startX = static_cast<int>(-pivot.x * OuterWidth());
	int row = 0;
	LineFormattingInfo line_formatting{};
	FormatLine(row++, line_formatting);
	int x = startX + line_formatting.add_x;
	PageFormattingInfo page_formatting{};
	FormatPage(page_formatting);
	int y = static_cast<int>(roundf(-font->baseline + (1.0f - pivot.y) * OuterHeight())) + bounds.top_ribbon - page_formatting.add_y;
	// TODO the bounds.top_ribbon ensures a kind of vertical justify. Explore this more in format.

	int prevCodepoint = 0;
	auto iter = text.begin();
	while (iter)
	{
		Font::Codepoint codepoint = iter.advance();

		if (codepoint == ' ')
		{
			x += static_cast<int>(font->space_width * line_formatting.space_mul_x);
			prevCodepoint = 0;
		}
		else if (codepoint == '\t')
		{
			x += static_cast<int>(font->space_width * format.num_spaces_in_tab * line_formatting.space_mul_x);
			prevCodepoint = 0;
		}
		else if (carriage_return_2(codepoint, iter ? iter.codepoint() : 0))
		{
			FormatLine(row++, line_formatting);
			if (x == startX + line_formatting.add_x)
				y -= line_height * page_formatting.linebreak_mul_y;
			else
				y -= line_height * page_formatting.mul_y;
			x = startX + line_formatting.add_x;
			++iter;
			prevCodepoint = 0;
		}
		else if (carriage_return_1(codepoint))
		{
			FormatLine(row++, line_formatting);
			if (x == startX + line_formatting.add_x)
				y -= line_height * page_formatting.linebreak_mul_y;
			else
				y -= line_height * page_formatting.mul_y;
			x = startX + line_formatting.add_x;
			prevCodepoint = 0;
		}
		else if (font->Cache(codepoint))
		{
			const Font::Glyph& glyph = font->glyphs[codepoint];
			x += font->KerningOf(prevCodepoint, codepoint, font->glyphs[prevCodepoint].gIndex, glyph.gIndex, line_formatting.mul_x);
			DrawGlyph(glyph, x, y, canvas_layer);
			x += static_cast<int>(roundf(glyph.advance_width * font->scale * line_formatting.mul_x));
			prevCodepoint = codepoint;
		}
	}
}

void TextRender::WarnInvalidCharacters() const
{
	auto iter = text.begin();
	std::stringstream ss;
	while (iter)
	{
		Font::Codepoint codepoint = iter.advance();
		if (codepoint != ' ' && codepoint != '\t' && !carriage_return_2(codepoint, iter ? iter.codepoint() : 0) && !carriage_return_1(codepoint) && !font->Supports(codepoint))
		{
			ss.str("");
			ss << "Cannot render codepoint: U+" << std::hex << std::uppercase << codepoint;
			Logger::LogWarning(ss.str());
		}
	}
}

void TextRender::DrawGlyph(const Font::Glyph& glyph, int x, int y, CanvasLayer* canvas_layer)
{
	PackedTransform2D glyph_transform({ {x, y - glyph.ch_y0}, 0.0f, {1.0f, -1.0f} });
	glyph_transform.Sync(m_Fickler.transformable->self);

	if (status & 0b10)
	{
		// transformP
		for (VertexBufferCounter i = 0; i < renderable.vertexCount; ++i)
		{
			renderable.vertexBufferData[i * stride + 1] = static_cast<GLfloat>(glyph_transform.packedP.x);
			renderable.vertexBufferData[i * stride + 2] = static_cast<GLfloat>(glyph_transform.packedP.y);
		}
	}
	if (status & 0b110)
	{
		// transformRS
		for (VertexBufferCounter i = 0; i < renderable.vertexCount; ++i)
		{
			renderable.vertexBufferData[i * stride + 3] = static_cast<GLfloat>(glyph_transform.packedRS[0][0]);
			renderable.vertexBufferData[i * stride + 4] = static_cast<GLfloat>(glyph_transform.packedRS[0][1]);
			renderable.vertexBufferData[i * stride + 5] = static_cast<GLfloat>(glyph_transform.packedRS[1][0]);
			renderable.vertexBufferData[i * stride + 6] = static_cast<GLfloat>(glyph_transform.packedRS[1][1]);
		}
	}
	if (status & 0b1110)
	{
		// transformM
		Modulate glyph_modulate = m_Fickler.modulatable->self.modulate;
		for (VertexBufferCounter i = 0; i < renderable.vertexCount; ++i)
		{
			renderable.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(glyph_modulate.r);
			renderable.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(glyph_modulate.g);
			renderable.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(glyph_modulate.b);
			renderable.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(glyph_modulate.a);
		}
	}
	// vertex positions
	renderable.vertexBufferData[0 * stride + 11] = static_cast<GLfloat>(0.0f);
	renderable.vertexBufferData[0 * stride + 12] = static_cast<GLfloat>(0.0f);
	renderable.vertexBufferData[1 * stride + 11] = static_cast<GLfloat>(glyph.width);
	renderable.vertexBufferData[1 * stride + 12] = static_cast<GLfloat>(0.0f);
	renderable.vertexBufferData[2 * stride + 11] = static_cast<GLfloat>(glyph.width);
	renderable.vertexBufferData[2 * stride + 12] = static_cast<GLfloat>(glyph.height);
	renderable.vertexBufferData[3 * stride + 11] = static_cast<GLfloat>(0.0f);
	renderable.vertexBufferData[3 * stride + 12] = static_cast<GLfloat>(glyph.height);

	// texture
	renderable.textureHandle = glyph.texture;

	// UVs
	auto uvs = font->UVs(glyph);
	renderable.vertexBufferData[0 * stride + 13] = static_cast<GLfloat>(uvs[0].x);
	renderable.vertexBufferData[0 * stride + 14] = static_cast<GLfloat>(uvs[0].y);
	renderable.vertexBufferData[1 * stride + 13] = static_cast<GLfloat>(uvs[1].x);
	renderable.vertexBufferData[1 * stride + 14] = static_cast<GLfloat>(uvs[1].y);
	renderable.vertexBufferData[2 * stride + 13] = static_cast<GLfloat>(uvs[2].x);
	renderable.vertexBufferData[2 * stride + 14] = static_cast<GLfloat>(uvs[2].y);
	renderable.vertexBufferData[3 * stride + 13] = static_cast<GLfloat>(uvs[3].x);
	renderable.vertexBufferData[3 * stride + 14] = static_cast<GLfloat>(uvs[3].y);

	canvas_layer->DrawRect(renderable, on_draw_callback);
}

void TextRender::UpdateBounds()
{
	int line_height = font->LineHeight(format.line_spacing_mult);
	int x = 0;
	int y = -font->baseline;
	bounds = {};
	int min_ch_y0 = 0;
	int max_ch_y1 = INT_MIN;
	LineInfo line_info{};
	bool first_line = true;

	int prevCodepoint = 0;
	auto iter = text.begin();
	while (iter)
	{
		Font::Codepoint codepoint = iter.advance();

		if (codepoint == ' ')
		{
			x += font->space_width;
			prevCodepoint = 0;
			++line_info.num_spaces;
		}
		else if (codepoint == '\t')
		{
			x += static_cast<int>(font->space_width * format.num_spaces_in_tab);
			prevCodepoint = 0;
			++line_info.num_tabs;
		}
		else if (carriage_return_2(codepoint, iter ? iter.codepoint() : 0))
		{
			if (x > bounds.inner_width)
				bounds.inner_width = x;
			line_info.width = x;
			bounds.lines.push_back(line_info);
			line_info = {};
			if (x == 0)
				++bounds.num_linebreaks;
			x = 0;
			y -= line_height;
			first_line = false;
			max_ch_y1 = INT_MIN;
			++iter;
			prevCodepoint = 0;
		}
		else if (carriage_return_1(codepoint))
		{
			if (x > bounds.inner_width)
				bounds.inner_width = x;
			line_info.width = x;
			bounds.lines.push_back(line_info);
			line_info = {};
			if (x == 0)
				++bounds.num_linebreaks;
			x = 0;
			y -= line_height;
			first_line = false;
			max_ch_y1 = INT_MIN;
			prevCodepoint = 0;
		}
		else if (font->Cache(codepoint))
		{
			const Font::Glyph& glyph = font->glyphs[codepoint];
			x += font->KerningOf(prevCodepoint, codepoint, font->glyphs[prevCodepoint].gIndex, glyph.gIndex);
			x += static_cast<int>(roundf(glyph.advance_width * font->scale));
			prevCodepoint = codepoint;
			if (first_line && glyph.ch_y0 < min_ch_y0)
				min_ch_y0 = glyph.ch_y0;
			if (glyph.ch_y0 + glyph.height > max_ch_y1)
				max_ch_y1 = glyph.ch_y0 + glyph.height;
		}
	}
	if (x > bounds.inner_width)
		bounds.inner_width = x;
	line_info.width = x;
	bounds.lines.push_back(line_info);
	line_info = {};
	if (x == 0)
		++bounds.num_linebreaks;
	bounds.lowest_baseline = -y;
	bounds.top_ribbon = font->ascent * font->scale + min_ch_y0;
	if (max_ch_y1 == INT_MAX)
		max_ch_y1 = 0;
	bounds.bottom_ribbon = max_ch_y1 - font->descent * font->scale;
	bounds.inner_height = bounds.lowest_baseline - font->descent * font->scale - bounds.top_ribbon;
}

static bool read_kern_part(const std::string& p, int& k)
{
	if (p[0] == '\\')
	{
		if (p.size() == 1)
			return false;
		if (p[1] == 'x')
			k = std::stoi(p.substr(2, p.size() - 2), nullptr, 16);
		else if (p[1] == '\\')
			k = '\\';
		else if (p[1] == '\'')
			k = '\'';
		else if (p[1] == '"')
			k = '\"';
		else if (p[1] == '?')
			k = '\?';
		else if (p[1] == 'a')
			k = '\a';
		else if (p[1] == 'b')
			k = '\b';
		else if (p[1] == 'f')
			k = '\f';
		else if (p[1] == 'n')
			k = '\n';
		else if (p[1] == 'r')
			k = '\r';
		else if (p[1] == 't')
			k = '\t';
		else if (p[1] == 'v')
			k = '\v';
		else if (p[1] == '0')
			k = '\0';
		else
			return false;
	}
	else
		k = p[0];
	return true;
}

static bool parse_kerning_line(const std::string& p0, const std::string& p1,
	const std::string& p2, std::pair<std::pair<Font::Codepoint, Font::Codepoint>, int>& insert)
{
	if (!read_kern_part(p0, insert.first.first))
		return false;
	if (!read_kern_part(p1, insert.first.second))
		return false;
	insert.second = std::stoi(p2);
	return true;
}

static void parse_kerning(const char* filepath, Font::Kerning& kerning)
{
	std::string content;
	if (IO::read_file(filepath, content))
	{
		char part = 0;
		std::string p0, p1, p2;
		for (auto iter = content.begin(); iter != content.end(); ++iter)
		{
			if (carriage_return_1(*iter))
			{
				std::pair<std::pair<Font::Codepoint, Font::Codepoint>, int> insert;
				if (parse_kerning_line(p0, p1, p2, insert))
					kerning.insert_or_assign(insert.first, insert.second);
				p0.clear();
				p1.clear();
				p2.clear();
				part = 0;
			}
			else if (carriage_return_2(*iter, iter + 1 != content.end() ? *(iter + 1) : 0))
			{
				std::pair<std::pair<Font::Codepoint, Font::Codepoint>, int> insert;
				if (parse_kerning_line(p0, p1, p2, insert))
					kerning.insert_or_assign(insert.first, insert.second);
				p0.clear();
				p1.clear();
				p2.clear();
				part = 0;
				++iter;
			}
			else if (part == 0)
			{
				if (*iter == ' ' || *iter == '\t')
				{
					if (!p0.empty())
						++part;
				}
				else
					p0.push_back(*iter);
			}
			else if (part == 1)
			{
				if (*iter == ' ' || *iter == '\t')
				{
					if (!p1.empty())
						++part;
				}
				else
					p1.push_back(*iter);
			}
			else if (*iter != ' ' && *iter != '\t')
			{
				p2.push_back(*iter);
			}
		}
		if (part == 2)
		{
			std::pair<std::pair<Font::Codepoint, Font::Codepoint>, int> insert;
			if (parse_kerning_line(p0, p1, p2, insert))
				kerning.insert_or_assign(insert.first, insert.second);
		}
	}
}

FontFamily::FontFamily(const char* filepath)
{
	try
	{
		auto file = toml::parse_file(filepath);
		auto verif = Loader::verify_header(file, "font-family");
		if (verif != LOAD_STATUS::OK)
		{
			Logger::LogErrorFatal("Header does not match in font-family assetfile.");
			return;
		}

		auto ff = file["font-family"];
		if (!ff)
			return;

		auto arr = ff["font"].as_array();
		size_t i = 0;
		while (auto table = arr->get_as<toml::table>(i++))
		{
			auto name = (*table)["name"].value<std::string>();
			if (!name)
			{
				Logger::LogWarning("Name missing in font-family assetfile entry.");
				continue;
			}
			auto fp = (*table)["file"].value<std::string>();
			if (!fp)
			{
				Logger::LogWarning("Font filepath missing for font-family entry: " + name.value());
				continue;
			}
			FontEntry font_entry;
			font_entry.filepath = fp.value();
			auto kerning_filepath = (*table)["kerning"].value<std::string>();
			if (kerning_filepath)
				parse_kerning(kerning_filepath.value().c_str(), font_entry.kerning);
			family.insert({ name.value(), std::move(font_entry) });
		}
	}
	catch (const toml::parse_error& err)
	{
		Logger::LogErrorFatal("Cannot parse font-family asset file \"" + std::string(filepath) + "\": " + std::string(err.description()));
	}
}
