#pragma once

#include <string>

struct TileRect
{
	int x, y, w, h;
};

enum class TileDeletionPolicy : unsigned char
{
	FROM_STBI,
	FROM_NEW,
	FROM_EXTERNAL
};

class Tile
{
	friend class TileRegistry;
	friend class Texture;
	friend class NonantTile;
	unsigned char* m_ImageBuffer;
	int m_Width, m_Height, m_BPP;
	TileDeletionPolicy deletion_policy;
	
	bool IsValid() const { return m_ImageBuffer != nullptr; }
	
	Tile() : m_Width(0), m_Height(0), m_BPP(0), deletion_policy(TileDeletionPolicy::FROM_EXTERNAL), m_ImageBuffer(nullptr) {}
	void DeleteBuffer() const;

public:
	Tile(const char* filepath, float svg_scale = 1.0f);
	Tile(unsigned char* heap_image_buffer, int width, int height, int bpp, TileDeletionPolicy deletion_policy);
	Tile(const Tile&) = delete;
	Tile(Tile&& tile) noexcept;
	Tile& operator=(Tile&& tile) noexcept;
	~Tile();

	 unsigned char const* GetImageBuffer() const { return m_ImageBuffer; }
	 int GetWidth() const { return m_Width; }
	 int GetHeight() const { return m_Height; }
	 int GetBPP() const { return m_BPP; }
};
