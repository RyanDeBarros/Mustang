#include "TileMap.h"

TileMap::TileMap(const int& atlas_width, const int& atlas_height)
	: m_Atlas(new Atlas(atlas_width, atlas_height))
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
		delete m_Atlas;
}

ActorPrimitive2D* TileMap::operator[](const int& i)
{
	ActorPrimitive2D* primitive = nullptr;
	if (i < cache_i)
		ClearCache();
	while (cache_iter != m_Map.end())
	{
		primitive = cache_iter->second[i - cache_iter_offset];
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

// TODO set atlas tessel in primitive's vertex buffer data upon editing m_Map