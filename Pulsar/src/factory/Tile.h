#pragma once

#include <string>

#include "Atlas.h"

struct TileRect
{
	int x, y, w, h;
};

class Tile
{
	friend class TileFactory;
	friend class Texture;
	friend class Atlas;
	std::string m_Filepath;
	unsigned char* m_ImageBuffer;
	int m_Width, m_Height, m_BPP;
	unsigned int m_AtlasID;

	inline bool Equivalent(const std::string& filepath) const { return m_Filepath == filepath; }
	inline bool Equivalent(const Atlas& atlas) const { return m_AtlasID == atlas.id; }
	inline bool IsValid() const { return m_ImageBuffer != nullptr; }
public:
	Tile(const char* filepath);
	Tile(const class Atlas& atlas);
	Tile(Tile&& tile) noexcept;
	Tile(const Tile&) = delete;
	~Tile();
};
