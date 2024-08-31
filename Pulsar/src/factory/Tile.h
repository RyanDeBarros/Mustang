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
	bool from_stbi;
	
	 bool IsValid() const { return m_ImageBuffer != nullptr; }
	
	Tile() : m_Width(0), m_Height(0), m_BPP(0), from_stbi(false), m_ImageBuffer(nullptr) {}

public:
	Tile(const char* filepath, float svg_scale = 1.0f);
	Tile(unsigned char* heap_image_buffer, int width, int height, int bpp);
	Tile(const Tile&) = delete;
	Tile(Tile&& tile) noexcept;
	Tile& operator=(Tile&& tile) noexcept;
	~Tile();

	 unsigned char* const GetImageBuffer() const { return m_ImageBuffer; }
	 int GetWidth() const { return m_Width; }
	 int GetHeight() const { return m_Height; }
	 int GetBPP() const { return m_BPP; }
};
