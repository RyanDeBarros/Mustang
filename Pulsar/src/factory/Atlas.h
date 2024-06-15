#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Tile.h"

class Atlas
{
	friend class Tile;
	friend class Texture;
	const unsigned int id;
	unsigned char* m_AtlasBuffer;
	AtlasPos insert_pos;
	int m_Width, m_Height;
	int m_BufferSize;
	std::unordered_map<TileHandle, AtlasPos> m_Tileset;

public:
	Atlas(const int& width, const int& height);
	Atlas(const Atlas&) = delete;
	~Atlas();

	static void SaveAtlas(const Atlas& atlas, const char* texture_filepath, const char* asset_filepath);
	static Atlas LoadAtlas(const char* asset_filepath);

	bool Insert(const TileHandle& tile);
	bool Remove(const TileHandle& tile);
};
