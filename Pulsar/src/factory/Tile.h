#pragma once

#include <string>

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

	inline bool Equivalent(const std::string& filepath) const { return m_Filepath == filepath; }
	inline bool IsValid() const { return m_ImageBuffer != nullptr; }
public:
	Tile(const char* filepath);
	Tile(Tile&& tile) noexcept;
	Tile(const Tile&) = delete;
	~Tile();
};
