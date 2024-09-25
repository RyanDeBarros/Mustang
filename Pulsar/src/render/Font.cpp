#include "Font.h"

#include <utf/utf8.h>

#include "IO.h"
#include "Logger.inl"
#include "CanvasLayer.h"
#include "AssetLoader.h"

Font::Glyph::Glyph(Font* font, int gIndex, float scale)
	: gIndex(gIndex), font(font)
{
	stbtt_GetGlyphHMetrics(&font->font_info, gIndex, &advance_width, &left_bearing);
	int ch_x0, ch_x1, ch_y1;
	stbtt_GetGlyphBitmapBox(&font->font_info, gIndex, scale, scale, &ch_x0, &ch_y0, &ch_x1, &ch_y1);
	width = ch_x1 - ch_x0;
	height = ch_y1 - ch_y0;
}

void Font::Glyph::RenderOnBitmap(unsigned char* bmp)
{
	stbtt_MakeGlyphBitmap(&font->font_info, bmp, width, height, width, font->scale, font->scale, gIndex);
	location = bmp;
}

Font::Font(const char* font_filepath, float font_size, const std::string& common_buffer, const TextureSettings& settings)
	: font_size(font_size), texture_settings(settings)
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
	int space_advance_width, space_left_bearing;
	stbtt_GetCodepointHMetrics(&font_info, ' ', &space_advance_width, &space_left_bearing);
	space_width = static_cast<int>(roundf(space_advance_width * scale));

	size_t common_width = 0;
	size_t common_height = 0;
	utf8::iterator<std::string::const_iterator> begin(common_buffer.cbegin(), common_buffer.cbegin(), common_buffer.cend());
	utf8::iterator<std::string::const_iterator> end(common_buffer.cend(), common_buffer.cbegin(), common_buffer.cend());
	std::vector<Font::Codepoint> codepoints;
	for (auto& iter = begin; iter != end; ++iter)
	{
		int codepoint = *iter;
		if (glyphs.find(codepoint) != glyphs.end())
			continue;
		int gIndex = stbtt_FindGlyphIndex(&font_info, codepoint);
		// TODO warning message? bool flag in constructor to disable warnings
		if (!gIndex)
			continue;
		Glyph glyph(this, gIndex, scale);
		common_width += glyph.width;
		// TODO better way of packing? that would make UVs much more complex though.
		if (glyph.height > common_height)
			common_height = glyph.height;
		glyphs.insert({ codepoint, std::move(glyph) });
		codepoints.push_back(codepoint);
	}
	if (common_width > 0)
	{
		common_bmp = new unsigned char[common_width * common_height];
		size_t left_x = 0;
		for (Font::Codepoint codepoint : codepoints)
		{
			Font::Glyph& glyph = glyphs[codepoint];
			glyph.RenderOnBitmap(common_bmp + left_x * common_height);
			left_x += glyph.width;
		}
		TileHandle tile = TileRegistry::GetHandle(TileConstructArgs_buffer(common_bmp, common_width, common_height, 1, TileDeletionPolicy::FROM_EXTERNAL, false));
		TextureHandle texture = TextureRegistry::GetHandle(TextureConstructArgs_tile(tile, 0, texture_settings));
		left_x = 0;
		for (Font::Codepoint codepoint : codepoints)
		{
			Font::Glyph& glyph = glyphs[codepoint];
			glyph.texture = texture;
			float uvx1 = static_cast<float>(left_x) / common_width;
			float uvx2 = (static_cast<float>(left_x) + glyph.width) / common_width;
			float uvy = static_cast<float>(glyph.height) / common_height;
			glyph.uvs = std::array<glm::vec2, 4>{
				glm::vec2{ uvx1, 0.0f },
				glm::vec2{ uvx2, 0.0f },
				glm::vec2{ uvx2, uvy },
				glm::vec2{ uvx1, uvy }
			};
			left_x += glyph.width;
		}
	}
}

Font::~Font()
{
	if (common_bmp)
		delete[] common_bmp;
}

bool Font::Cache(const std::string& str, size_t index)
{
	utf8::iterator<std::string::const_iterator> it(str.cbegin() + index, str.cbegin(), str.cend());
	return Cache(*it);
}
// TODO space is sometimes actually accepted. Added to Font::COMMON?
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

TextRender Font::GetTextRender(ZIndex z)
{
	return TextRender(this, z);
}

int Font::LineHeight(float line_spacing) const
{
	return static_cast<int>(roundf((ascent - descent + linegap) * scale * line_spacing));
}

static bool carriage_return(const std::string& text, size_t index)
{
	return text[index] == '\n' || (text[index] == '\r' && index < text.size() - 1 && text[index + 1] == '\n');
}

Functor<void, TextureSlot> TextRender::create_on_draw_callback(TextRender* tr)
{
	return make_functor<true>([](TextureSlot slot, TextRender* tr) {
		for (VertexBufferCounter i = 0; i < tr->renderable.vertexCount; ++i)
			tr->renderable.vertexBufferData[0 + i * TextRender::stride] = slot;
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
	int x = 0;
	int y = font->baseline;
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (font->Cache(text, i))
		{
			// TODO get_codepoint utility function, not text[i]. replace above Cache(text, i) with that as well.
			const Font::Glyph& glyph = font->glyphs[text[i]];
			if (i > 0)
			{
				auto iter = font->glyphs.find(text[i - 1]);
				if (iter != font->glyphs.end())
				{
					int kern = stbtt_GetGlyphKernAdvance(&font->font_info, iter->second.gIndex, glyph.gIndex);
					x += static_cast<int>(roundf(kern * font->scale));
				}
			}
			DrawGlyph(glyph, x, y, canvas_layer);
			x += static_cast<int>(roundf(glyph.advance_width * font->scale));
		}
		else if (text[i] == ' ')
		{
			x += font->space_width;
		}
		else if (text[i] == '\t')
		{
			x += font->space_width * format.num_spaces_in_tab;
		}
		else if (carriage_return(text, i))
		{
			x = 0;
			y -= line_height;
		}
	}
}

void TextRender::DrawGlyph(const Font::Glyph& glyph, int x, int y, CanvasLayer* canvas_layer)
{
	PackedTransform2D glyph_transform;
	glyph_transform.transform.position = { x, y - glyph.ch_y0 };
	glyph_transform.transform.scale = { format.font_size_mult, -format.font_size_mult };
	glyph_transform.Sync(m_Fickler.transformable->self);
	PackedModulate glyph_modulate;
	glyph_modulate.Sync(m_Fickler.modulatable->self);
	// TODO format.font_size_mult

	//PackedTransform2D glyph_transform = m_Fickler.transformable->self;
	//PackedModulate glyph_modulate = m_Fickler.modulatable->self;

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
		for (VertexBufferCounter i = 0; i < renderable.vertexCount; ++i)
		{
			renderable.vertexBufferData[i * stride + 7 ] = static_cast<GLfloat>(glyph_modulate.packedM.r);
			renderable.vertexBufferData[i * stride + 8 ] = static_cast<GLfloat>(glyph_modulate.packedM.g);
			renderable.vertexBufferData[i * stride + 9 ] = static_cast<GLfloat>(glyph_modulate.packedM.b);
			renderable.vertexBufferData[i * stride + 10] = static_cast<GLfloat>(glyph_modulate.packedM.a);
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

	// TODO pivot for text render.

	canvas_layer->DrawRect(renderable, on_draw_callback);

	/*for (int row = 0; row < glyph.height; ++row)
	{
		for (int col = 0; col < glyph.width; ++col)
		{
			int destX = x + col;
			int destY = y + row + glyph.ch_y0;

			//if (destX >= 0 && destX < fb_width && destY >= 0 && destY < fb_height)
			//{
				//font_bitmap[destY * fb_width + destX] *= 1.0f - glyph_bitmap[row * glyph_width + col] / 255.0f;
			//}

		}
	}*/
}