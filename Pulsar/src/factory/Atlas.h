#pragma once

#include <unordered_map>

#include "Typedefs.h"

class Atlas
{
	friend class Tile;
	friend class Texture;
	friend void saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath);

	const unsigned int id;
	unsigned char* m_AtlasBuffer;
	AtlasPos insert_pos;
	int m_Width, m_Height;
	int m_BufferSize;
	std::unordered_map<TileHandle, AtlasPos> m_Tileset;

public:
	static constexpr unsigned char BPP = 4;
	static constexpr unsigned char STRIDE_BYTES = sizeof(unsigned char);

	Atlas(const int& width, const int& height);
	Atlas(const Atlas&) = delete;
	~Atlas();

	bool Insert(const TileHandle& tile);
	// TODO insert a subtile, i.e. tile with (x,y,w,h)
	bool Remove(const TileHandle& tile);
};
