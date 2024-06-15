#include "TileMap.h"

TileMap::TileMap(const int& atlas_width, const int& atlas_height)
	: m_Atlas(new Atlas(atlas_width, atlas_height))
{
}

TileMap::TileMap(Atlas* atlas)
	: m_Atlas(atlas)
{
}

TileMap::~TileMap()
{
	if (m_Atlas)
		delete m_Atlas;
}
