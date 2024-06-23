#include "Atlas.h"

#include <algorithm>

#include "TileFactory.h"

unsigned int id_count = 1;

// TODO use factory? so that id is not incremented if atlas is loaded from same file. This would probably be in a different constructor. Perhaps not a full factory class, but a static map that maps atlas filepaths to ids.
Atlas::Atlas(const int& width, const int& height)
	: m_AtlasBuffer(nullptr), m_Width(width), m_Height(height), id(id_count++)
{
	m_BufferSize = Atlas::BPP * m_Width * m_Height; // 4 is BPP
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
	if (!t || t->m_BPP > Atlas::BPP)
		return false;

	int area = Atlas::BPP * t->m_Width * t->m_Height;
	if (insert_pos + area > m_BufferSize)
		return false;
	m_Tileset.insert({ tile, insert_pos });
	if (t->m_BPP == Atlas::BPP)
		memcpy_s(m_AtlasBuffer + insert_pos, m_BufferSize - insert_pos, t->m_ImageBuffer, area);
	else
	{
		int j = 0;
		for (auto i = 0; i < area; i++)
		{
			if (i % Atlas::BPP < t->m_BPP)
			{
				m_AtlasBuffer[insert_pos + i] = t->m_ImageBuffer[j];
				j++;
			}
			else
				m_AtlasBuffer[insert_pos + i] = 255;
		}
	}
	//int area = t->m_Width * t->m_Height;
	//if (insert_pos + area > m_BufferSize)
	//	return false;
	//m_Tileset.insert({ tile, insert_pos });
	//memcpy_s(m_AtlasBuffer + insert_pos, m_BufferSize - insert_pos, t->m_ImageBuffer, area);
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
