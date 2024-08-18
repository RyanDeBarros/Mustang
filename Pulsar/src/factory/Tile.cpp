#include "Tile.h"

#include <stb/stb_image.h>

#include "Logger.inl"

Tile::Tile(const char* filepath)
	: m_ImageBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	m_ImageBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 0);
	if (!m_ImageBuffer)
	{
		Logger::LogError("Texture '" + std::string(filepath) + "' could not be loaded!\n" + stbi_failure_reason());
		return;
	}
}

Tile::Tile(unsigned char* heap_image_buffer, int width, int height, int bpp)
	: m_ImageBuffer(heap_image_buffer), m_Width(width), m_Height(height), m_BPP(bpp)
{
}

Tile::Tile(Tile&& tile) noexcept
	: m_ImageBuffer(tile.m_ImageBuffer), m_Width(tile.m_Width), m_Height(tile.m_Height), m_BPP(tile.m_BPP)
{
	tile.m_ImageBuffer = nullptr;
}

Tile& Tile::operator=(Tile&& tile) noexcept
{
	if (this == &tile)
		return *this;
	if (m_ImageBuffer)
		stbi_image_free(m_ImageBuffer);
	m_ImageBuffer = tile.m_ImageBuffer;
	m_Width = tile.m_Width;
	m_Height = tile.m_Height;
	m_BPP = tile.m_BPP;
	tile.m_ImageBuffer = nullptr;
	return *this;
}

Tile::~Tile()
{
	if (m_ImageBuffer)
	{
		stbi_image_free(m_ImageBuffer);
		m_ImageBuffer = nullptr;
	}
}
