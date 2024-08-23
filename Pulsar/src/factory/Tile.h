#pragma once

#include <string>

struct TileRect
{
	int x, y, w, h;
};

class Tile
{
protected:
	friend class TileRegistry;
	friend class Texture;
	unsigned char* m_ImageBuffer;
	int m_Width, m_Height, m_BPP;
	
	inline bool IsValid() const { return m_ImageBuffer != nullptr; }
	
	Tile() : m_Width(0), m_Height(0), m_BPP(0), m_ImageBuffer(nullptr) {}

public:
	Tile(const char* filepath);
	Tile(unsigned char* heap_image_buffer, int width, int height, int bpp);
	Tile(const Tile&) = delete;
	Tile(Tile&& tile) noexcept;
	Tile& operator=(Tile&& tile) noexcept;
	~Tile();

	inline unsigned char* const GetImageBuffer() const { return m_ImageBuffer; }
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetBPP() const { return m_BPP; }
};
