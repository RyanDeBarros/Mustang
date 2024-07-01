#include "TileMap.h"

TileMap::TileMap(std::vector<TileHandle>& tiles, const int& atlas_width, const int& atlas_height)
	: m_Atlas(new Atlas(tiles, atlas_width, atlas_height))
{
	ClearCache();
}

TileMap::TileMap(Atlas* atlas)
	: m_Atlas(atlas)
{
	ClearCache();
}

TileMap::~TileMap()
{
	if (m_Atlas)
	{
		delete m_Atlas;
		m_Atlas = nullptr;
	}
}

ActorPrimitive2D* const TileMap::operator[](const int& i)
{
	ActorPrimitive2D* primitive = nullptr;
	if (i < cache_i)
		ClearCache();
	while (cache_iter != m_Map.end())
	{
		primitive = cache_iter->second[static_cast<int>(i - cache_iter_offset)];
		if (primitive)
		{
			cache_i = i;
			return primitive;
		}
		cache_iter_offset += cache_iter->second.RectVectorRef().size();
		cache_iter++;
	}
	return primitive;
}

void TileMap::ClearCache()
{
	cache_i = 0;
	cache_iter = m_Map.begin();
	cache_iter_offset = 0;
}
