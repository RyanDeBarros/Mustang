#pragma once

#include <vector>
#include <unordered_map>

#include "Typedefs.h"

class Atlas
{
	friend class Tile;
	friend class Texture;
	friend void saveAtlas(const class Atlas& atlas, const char* texture_filepath, const char* asset_filepath);

	const unsigned int id;
	unsigned char* m_AtlasBuffer;
	int m_Width, m_Height, m_Border;
	int m_BufferSize;
	std::vector<struct Placement> m_Placements;

public:
	static constexpr unsigned char BPP = 4;
	static constexpr unsigned char STRIDE_BYTES = sizeof(unsigned char) * BPP;

	// TODO insert subtiles, i.e. tile with (x,y,w,h)
	Atlas(std::vector<TileHandle>& tiles, const int& width = -1, const int& height = -1, const int& border = 0);
	Atlas(const Atlas&) = delete;
	~Atlas();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	
	// TODO default shader handle for standard shader 32/8
	class RectRender SampleSubtile(const size_t& index, const struct TextureSettings& texture_settings, const ShaderHandle& shader, const ZIndex& z = 0, const bool& visible = true) const;

private:
	void RectPack(std::vector<TileHandle>& tiles, const int& width = -1, const int& height = -1);
	void PlaceTiles();
};
