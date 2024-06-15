#include "Atlas.h"

#include <algorithm>

#include "TileFactory.h"

unsigned int id_count = 1;

Atlas::Atlas(const int& width, const int& height)
	: m_AtlasBuffer(nullptr), m_Width(width), m_Height(height), id(id_count++)
{
	m_BufferSize = 4 * m_Width * m_Height;
	m_AtlasBuffer = new unsigned char[m_BufferSize](0);
	insert_pos = 0;
}

Atlas::~Atlas()
{
	if (m_AtlasBuffer)
		delete m_AtlasBuffer;
}

bool Atlas::Insert(const TileHandle& tile)
{
	const Tile* t = TileFactory::GetConstTileRef(tile);
	if (!t)
		return false;

	int area = t->m_Width * t->m_Height;
	if (insert_pos + area > m_BufferSize)
		return false;
	m_Tileset.insert({ tile, insert_pos });
	memcpy_s(m_AtlasBuffer + insert_pos, m_BufferSize - insert_pos, t->m_ImageBuffer, area);
	insert_pos += area;
	return true;
}

bool Atlas::Remove(const TileHandle& tile)
{
	const Tile* t = TileFactory::GetConstTileRef(tile);
	if (!t)
		return false;
	
	auto ts = m_Tileset.find(tile);
	if (ts == m_Tileset.end())
		return false;
	m_Tileset.erase(tile);
	int area = t->m_Width * t->m_Height;
	memmove_s(m_AtlasBuffer + ts->second, m_BufferSize - ts->second, m_AtlasBuffer + ts->second + area,
			std::max(m_BufferSize - ts->second - area, 0));
	insert_pos -= area;
	memset(m_AtlasBuffer + insert_pos, 0, m_BufferSize - insert_pos);
	return true;
}
