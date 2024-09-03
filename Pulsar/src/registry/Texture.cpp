#include "Texture.h"

#include <string>
#include <GL/glew.h>

#include <stb/stb_image.h>

#include "Logger.inl"
#include "Macros.h"

Texture::Texture(const char* filepath, TextureSettings settings, bool temporary_buffer, float svg_scale)
	: m_RID(0), m_Width(0), m_Height(0), m_Tile(0)
{
	Tile const* tile_ref = nullptr;
	if (temporary_buffer)
	{
		tile_ref = new Tile(filepath, svg_scale);
	}
	else
	{
		m_Tile = TileRegistry::GetHandle({ filepath, svg_scale });
		tile_ref = TileRegistry::Get(m_Tile);
	}
	if (!tile_ref)
	{
		Logger::LogError(std::string("Cannot create texture \"") + filepath + "\": Tile ref is null.");
		return;
	}

	m_Width = tile_ref->GetWidth();
	m_Height = tile_ref->GetHeight();
	TexImage(tile_ref, std::string("Cannot create texture \"") + filepath + "\": BPP is not 4, 3, 2, or 1.", settings.lodLevel);
	SetSettings(settings);
	if (temporary_buffer)
		delete tile_ref;
}

Texture::Texture(TileHandle tile, TextureSettings settings)
	: m_RID(0), m_Width(0), m_Height(0), m_Tile(tile)
{
	Tile const* tile_ref = TileRegistry::Get(m_Tile);
	if (!tile_ref)
	{
		Logger::LogError(std::string("Cannot create texture from tile \"") + std::to_string(tile) + "\": Tile ref is null.");
		return;
	}
	
	m_Width = tile_ref->GetWidth();
	m_Height = tile_ref->GetHeight();
	TexImage(tile_ref, std::string("Cannot create texture from tile  \"") + std::to_string(tile) + "\": BPP is not 4, 3, 2, or 1.", settings.lodLevel);
	SetSettings(settings);
}

Texture::Texture(Tile&& tile, TextureSettings settings)
	: m_RID(0), m_Tile(0)
{
	m_Width = tile.GetWidth();
	m_Height = tile.GetHeight();
	TexImage(&tile, "Cannot create texture from tile r-value ref: BPP is not 4, 3, 2, or 1.", settings.lodLevel);
	SetSettings(settings);
}

Texture::Texture(Texture&& texture) noexcept
	: m_RID(texture.m_RID), m_Width(texture.m_Width), m_Height(texture.m_Height), m_Tile(texture.m_Tile)
{
	texture.m_RID = 0;
}

Texture& Texture::operator=(Texture&& texture) noexcept
{
	if (this == &texture)
		return *this;
	if (m_RID != texture.m_RID)
	{
		PULSAR_TRY(glDeleteTextures(1, &m_RID));
	}
	m_RID = texture.m_RID;
	m_Width = texture.m_Width;
	m_Height = texture.m_Height;
	m_Tile = texture.m_Tile;
	texture.m_RID = 0;
	return *this;
}

Texture::~Texture()
{
	if (m_RID)
	{
		PULSAR_TRY(glDeleteTextures(1, &m_RID));
		m_RID = 0;
	}
}

void Texture::TexImage(Tile const* tile, const std::string& err_msg, GLint lod_level)
{
	if (m_RID)
	{
		PULSAR_TRY(glDeleteTextures(1, &m_RID));
	}
	PULSAR_TRY(glGenTextures(1, &m_RID));
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, m_RID));

	switch (tile->m_BPP)
	{
	case 4:
		PULSAR_TRY(glTexImage2D(GL_TEXTURE_2D, lod_level, GL_RGBA8, tile->m_Width, tile->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tile->m_ImageBuffer));
		break;
	case 3:
		PULSAR_TRY(glTexImage2D(GL_TEXTURE_2D, lod_level, GL_RGB8, tile->m_Width, tile->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, tile->m_ImageBuffer));
		break;
	case 2:
		PULSAR_TRY(glTexImage2D(GL_TEXTURE_2D, lod_level, GL_RG8, tile->m_Width, tile->m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, tile->m_ImageBuffer));
		break;
	case 1:
		PULSAR_TRY(glTexImage2D(GL_TEXTURE_2D, lod_level, GL_R8, tile->m_Width, tile->m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, tile->m_ImageBuffer));
		break;
	default:
		Logger::LogError(err_msg);
	}

	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::Bind(TextureSlot slot) const
{
	PULSAR_TRY(glActiveTexture(GL_TEXTURE0 + slot));
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
}

void Texture::Unbind(TextureSlot slot) const
{
	PULSAR_TRY(glActiveTexture(GL_TEXTURE0 + slot));
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::SetSettings(const TextureSettings& settings) const
{
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
	PULSAR_TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(settings.minFilter)));
	PULSAR_TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(settings.magFilter)));
	PULSAR_TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(settings.wrapS)));
	PULSAR_TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(settings.wrapT)));
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

TextureSettings Texture::GetSettings() const
{
	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
	GLint min_filter, mag_filter, wrap_s, wrap_t;
	
	PULSAR_TRY(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter));
	PULSAR_TRY(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter));
	PULSAR_TRY(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s));
	PULSAR_TRY(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t));

	PULSAR_TRY(glBindTexture(GL_TEXTURE_2D, 0));
	// TODO remove lod_level from project. Mipmaps can be integrated in the future.
	return { static_cast<MinFilter>(min_filter), static_cast<MagFilter>(mag_filter), static_cast<TextureWrap>(wrap_s), static_cast<TextureWrap>(wrap_t), 0 };
}

void Texture::ReTexImage(Tile const* tile, GLint lod_level)
{
	m_Width = tile->GetWidth();
	m_Height = tile->GetHeight();
	TextureSettings settings = GetSettings();
	TexImage(tile, "Cannot renew texture from tile pointer: BPP is not 4, 3, 2, or 1.", lod_level);
	SetSettings(settings);
}

void Texture::ReTexImage(GLint lod_level)
{
	if (m_Tile)
	{
		Tile const* tile = TileRegistry::Get(m_Tile);
		ReTexImage(tile, lod_level);
	}
}

const TextureSettings Texture::linear_settings = { MinFilter::Linear, MagFilter::Linear, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge };
const TextureSettings Texture::nearest_settings = { MinFilter::Nearest, MagFilter::Nearest, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge };