#include "Font.h"

#include "IO.h"
#include "Logger.inl"
#include "CanvasLayer.h"
#include "AssetLoader.h"

Font::Glyph::Glyph(Font* font, int gIndex, float scale)
	: gIndex(gIndex), font(font), texture(0), uvs({})
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

Font::Font(const char* font_filepath, float font_size, const UTF::String& common_buffer, const TextureSettings& settings, UTF::String* failed_common_chars)
	: font_size(font_size), texture_settings(settings), font_info{}
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
		Glyph glyph(this, gIndex, scale);
		common_width += glyph.width + size_t(1);
		if (glyph.height > common_height)
			common_height = glyph.height;
		glyphs.insert({ codepoint, std::move(glyph) });
		codepoints.push_back(codepoint);
	}
	if (common_width > 0)
	{
		common_bmp = new unsigned char[common_width * common_height];
		size_t buffer_width = 0;
		for (Font::Codepoint codepoint : codepoints)
		{
			Font::Glyph& glyph = glyphs[codepoint];
			glyph.RenderOnBitmap(common_bmp + buffer_width, common_width, common_height, true);
			buffer_width += glyph.width + size_t(1);
		}
		TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(common_bmp,
			static_cast<int>(common_width), static_cast<int>(common_height), 1, TileDeletionPolicy::FROM_EXTERNAL, false));
		TextureHandle texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
		buffer_width = 0;
		for (Font::Codepoint codepoint : codepoints)
		{
			Font::Glyph& glyph = glyphs[codepoint];
			glyph.texture = texture;
			float uvx1 = static_cast<float>(buffer_width) / common_width;
			float uvx2 = static_cast<float>(buffer_width + glyph.width) / common_width;
			float uvy1 = 0.0f;
			float uvy2 = static_cast<float>(glyph.height) / common_height;
			glyph.uvs = std::array<glm::vec2, 4>{
				glm::vec2{ uvx1, uvy1 },
				glm::vec2{ uvx2, uvy1 },
				glm::vec2{ uvx2, uvy2 },
				glm::vec2{ uvx1, uvy2 }
			};
			buffer_width += glyph.width + size_t(1);
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
		Font::Glyph glyph(this, gIndex, scale);
		unsigned char* bmp = new unsigned char[glyph.Area()];
		glyph.RenderOnBitmap(bmp);
		TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(bmp, glyph.width, glyph.height, 1, TileDeletionPolicy::FROM_NEW, false));
		TextureHandle texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
		glyph.texture = texture;
		glyph.uvs = std::array<glm::vec2, 4>{
			glm::vec2{ 0.0f, 0.0f },
			glm::vec2{ 1.0f, 0.0f },
			glm::vec2{ 1.0f, 1.0f },
			glm::vec2{ 0.0f, 1.0f }
		};
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

TextRender Font::GetTextRender(ZIndex z)
{
	return TextRender(this, z);
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

TextRender::TextRender(Font* font, ZIndex z)
	: FickleActor2D(FickleType::Protean, z), font(font), on_draw_callback(create_on_draw_callback(this))
{
	Loader::loadRenderable(PulsarSettings::text_standard_filepath(), renderable);
}

void TextRender::RequestDraw(CanvasLayer* canvas_layer)
{
	if ((status & 0b1) == 0b0)
		return;
	int line_height = font->LineHeight(format.line_spacing_mult);
	int startX = static_cast<int>(-pivot.x * bounds.full_width);
	int x = startX;
	// TODO the bounds.top_ribbon ensures a kind of vertical justify. Explore this more in format.
	int y = static_cast<int>(roundf(-font->baseline + (1.0f - pivot.y) * bounds.full_height)) + bounds.top_ribbon;

	int prevGIndex = 0;
	auto iter = text.begin();
	while (iter)
	{
		Font::Codepoint codepoint = iter.advance();

		if (codepoint == ' ')
		{
			x += font->space_width;
			prevGIndex = 0;
		}
		else if (codepoint == '\t')
		{
			x = static_cast<int>(x + font->space_width * format.num_spaces_in_tab);
			prevGIndex = 0;
		}
		else if (carriage_return_2(codepoint, iter ? iter.codepoint() : 0))
		{
			x = startX;
			y -= line_height;
			++iter;
			prevGIndex = 0;
		}
		else if (carriage_return_1(codepoint))
		{
			x = startX;
			y -= line_height;
			prevGIndex = 0;
		}
		else if (font->Cache(codepoint))
		{
			const Font::Glyph& glyph = font->glyphs[codepoint];
			if (prevGIndex > 0)
			{
				int kern = stbtt_GetGlyphKernAdvance(&font->font_info, prevGIndex, glyph.gIndex);
				x += static_cast<int>(roundf(kern * font->scale));
			}
			DrawGlyph(glyph, x, y, canvas_layer);
			x += static_cast<int>(roundf(glyph.advance_width * font->scale));
			prevGIndex = glyph.gIndex;
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
	renderable.vertexBufferData[0 * stride + 13] = static_cast<GLfloat>(glyph.uvs[0].x);
	renderable.vertexBufferData[0 * stride + 14] = static_cast<GLfloat>(glyph.uvs[0].y);
	renderable.vertexBufferData[1 * stride + 13] = static_cast<GLfloat>(glyph.uvs[1].x);
	renderable.vertexBufferData[1 * stride + 14] = static_cast<GLfloat>(glyph.uvs[1].y);
	renderable.vertexBufferData[2 * stride + 13] = static_cast<GLfloat>(glyph.uvs[2].x);
	renderable.vertexBufferData[2 * stride + 14] = static_cast<GLfloat>(glyph.uvs[2].y);
	renderable.vertexBufferData[3 * stride + 13] = static_cast<GLfloat>(glyph.uvs[3].x);
	renderable.vertexBufferData[3 * stride + 14] = static_cast<GLfloat>(glyph.uvs[3].y);

	canvas_layer->DrawRect(renderable, on_draw_callback);
}

void TextRender::UpdateBounds()
{
	int line_height = font->LineHeight(format.line_spacing_mult);
	int x = 0;
	int y = -font->baseline;
	bounds.full_width = 0;
	int min_ch_y0 = 0;

	int prevGIndex = 0;
	auto iter = text.begin();
	while (iter)
	{
		Font::Codepoint codepoint = iter.advance();

		if (codepoint == ' ')
		{
			x += font->space_width;
			prevGIndex = 0;
		}
		else if (codepoint == '\t')
		{
			x = static_cast<int>(x + font->space_width * format.num_spaces_in_tab);
			prevGIndex = 0;
		}
		else if (carriage_return_2(codepoint, iter ? iter.codepoint() : 0))
		{
			if (x > bounds.full_width)
				bounds.full_width = x;
			x = 0;
			y -= line_height;
			++iter;
			prevGIndex = 0;
		}
		else if (carriage_return_1(codepoint))
		{
			if (x > bounds.full_width)
				bounds.full_width = x;
			x = 0;
			y -= line_height;
			prevGIndex = 0;
		}
		else if (font->Cache(codepoint))
		{
			const Font::Glyph& glyph = font->glyphs[codepoint];
			if (prevGIndex > 0)
			{
				int kern = stbtt_GetGlyphKernAdvance(&font->font_info, prevGIndex, glyph.gIndex);
				x += static_cast<int>(roundf(kern * font->scale));
			}
			x += static_cast<int>(roundf(glyph.advance_width * font->scale));
			prevGIndex = glyph.gIndex;
			if (glyph.ch_y0 < min_ch_y0)
				min_ch_y0 = glyph.ch_y0;
		}
	}
	if (x > bounds.full_width)
		bounds.full_width = x;
	bounds.lowest_baseline = -y;
	bounds.top_ribbon = font->ascent * font->scale + min_ch_y0;
	bounds.full_height = bounds.lowest_baseline - font->descent * font->scale - bounds.top_ribbon;
}
