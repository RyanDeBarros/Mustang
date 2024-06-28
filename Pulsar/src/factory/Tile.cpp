#include "Tile.h"

#include <stb/stb_image.h>

#include "Logger.h"
#include "Atlas.h"

Tile::Tile(const char* filepath)
	: m_Filepath(filepath), m_ImageBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)//, m_AtlasID(0)
{
	m_ImageBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 0);
	if (!m_ImageBuffer)
	{
		Logger::LogError("Texture '" + m_Filepath + "' could not be loaded!\n" + stbi_failure_reason());
		return;
	}
}

//Tile::Tile(const Atlas& atlas)
//	: m_Filepath(""), m_ImageBuffer(atlas.m_AtlasBuffer), m_Width(atlas.m_Width), m_Height(atlas.m_Height), m_BPP(Atlas::BPP)//, m_AtlasID(atlas.id)
//{
//}

Tile::Tile(Tile&& tile) noexcept
	: m_Filepath(tile.m_Filepath), m_ImageBuffer(tile.m_ImageBuffer), m_Width(tile.m_Width), m_Height(tile.m_Height), m_BPP(tile.m_BPP)//, m_AtlasID(tile.m_AtlasID)
{
	tile.m_ImageBuffer = nullptr;
}

Tile::~Tile()
{
	if (m_ImageBuffer/* && m_AtlasID == 0*/)
	{
		stbi_image_free(m_ImageBuffer);
		m_ImageBuffer = nullptr;
	}
}

//bool Tile::Equivalent(const Atlas& atlas) const
//{
//	return m_AtlasID == atlas.id;
//}
