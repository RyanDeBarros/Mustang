#include "Texture.h"

#include <string>
#include <GL/glew.h>

#include <stb/stb_image.h>

#include "Logger.h"
#include "Macros.h"
#include "Atlas.h"

Texture::Texture(const char* filepath, TextureSettings settings, bool temporary_buffer)
	: m_RID(0), m_Tile(0), m_Settings(settings)//, m_Atlas(nullptr)
{
	Tile* tile_ref = nullptr;
	if (temporary_buffer)
	{
		tile_ref = new Tile(filepath);
	}
	else
	{
		m_Tile = TileFactory::GetHandle(filepath);
		tile_ref = const_cast<Tile*>(TileFactory::GetConstTileRef(m_Tile));
	}
	if (!tile_ref)
	{
		Logger::LogError(std::string("Cannot create texture \"") + filepath + "\": Tile ref is null.");
		return;
	}

	TRY(glGenTextures(1, &m_RID));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));

	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));

	switch (tile_ref->m_BPP)
	{
	case 4:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGBA8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 3:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGB8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 2:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RG8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 1:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_R8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	default:
	{
		Logger::LogError(std::string("Cannot create texture \"") + filepath + "\": BPP is not 4, 3, 2, or 1.");
		TRY(glBindTexture(GL_TEXTURE_2D, 0));
		if (temporary_buffer)
			delete tile_ref;
		return;
	}
	}

	TRY(glBindTexture(GL_TEXTURE_2D, 0));
	if (temporary_buffer)
		delete tile_ref;
}

Texture::Texture(TileHandle tile, TextureSettings settings)
	: m_RID(0), m_Tile(tile), m_Settings(settings)//, m_Atlas(nullptr)
{
	const Tile* const tile_ref = TileFactory::GetConstTileRef(m_Tile);
	if (!tile_ref)
	{
		Logger::LogError(std::string("Cannot create texture from tile \"") + std::to_string(tile) + "\": Tile ref is null.");
		return;
	}

	TRY(glGenTextures(1, &m_RID));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));

	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));

	switch (tile_ref->m_BPP)
	{
	case 4:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGBA8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 3:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGB8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 2:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RG8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	case 1:
	{
		TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_R8, tile_ref->m_Width, tile_ref->m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, tile_ref->m_ImageBuffer));
		break;
	}
	default:
	{
		Logger::LogError(std::string("Cannot create texture from tile  \"") + std::to_string(tile) + "\": BPP is not 4, 3, 2, or 1.");
		TRY(glBindTexture(GL_TEXTURE_2D, 0));
		return;
	}
	}

	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::Texture(Texture&& texture) noexcept
	: m_RID(texture.m_RID), m_Tile(texture.m_Tile), m_Settings(texture.m_Settings)
{
	texture.m_RID = 0;
}

Texture& Texture::operator=(Texture&& texture) noexcept
{
	TRY(glDeleteTextures(1, &m_RID));
	m_RID = texture.m_RID;
	m_Tile = texture.m_Tile;
	m_Settings = texture.m_Settings;
	texture.m_RID = 0;
	return *this;
}

Texture::~Texture()
{
	TRY(glDeleteTextures(1, &m_RID));
	m_RID = 0;
}

void Texture::Bind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
}

void Texture::Unbind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::SetSettings(const TextureSettings& settings)
{
	Bind();
	m_Settings = settings;
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));
	Unbind();
}

const TextureSettings Texture::linear_settings = { MinFilter::Linear, MagFilter::Linear, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge };
const TextureSettings Texture::nearest_settings = { MinFilter::Nearest, MagFilter::Nearest, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge };