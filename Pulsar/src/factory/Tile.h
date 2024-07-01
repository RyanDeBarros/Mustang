#pragma once

#include <string>

struct TileRect
{
	int x, y, w, h;
};

class Tile
{
protected:
	friend class TileFactory;
	friend class Texture;
	std::string m_Filepath;
	unsigned char* m_ImageBuffer;
	int m_Width, m_Height, m_BPP;
	
	inline bool Equivalent(const std::string& filepath) const { return m_Filepath == filepath; }
	inline bool IsValid() const { return m_ImageBuffer != nullptr; }
	
	Tile() : m_Width(0), m_Height(0), m_BPP(0), m_ImageBuffer(nullptr) {}

public:
	Tile(const char* filepath);
	Tile(Tile&& tile) noexcept;
	Tile(const Tile&) = delete;
	virtual ~Tile();

	const std::string& GetFilepath() const { return m_Filepath; }
	unsigned char* const GetImageBuffer() const { return m_ImageBuffer; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP() const { return m_BPP; }
};
