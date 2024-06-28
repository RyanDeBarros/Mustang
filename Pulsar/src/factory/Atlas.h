#pragma once

#include <vector>
#include <unordered_map>

#include "Typedefs.h"
#include "Tile.h"
#include "ShaderFactory.h"
#include "Texture.h"

class Atlas : public Tile
{
	friend class Tile;
	friend class Texture;
	friend class TileFactory;

	int m_Border;
	int m_BufferSize;
	std::vector<struct Placement> m_Placements;

	bool Equivalent(std::vector<TileHandle>& tiles, int width, int height, int border) const;
	bool Equivalent(std::vector<TileHandle>&& tiles, int width, int height, int border) const;

public:
	static constexpr unsigned char BPP = 4;
	static constexpr unsigned char STRIDE_BYTES = sizeof(unsigned char) * BPP;

	// TODO insert subtiles, i.e. tile with (x,y,w,h)
	Atlas(std::vector<TileHandle>& tiles, int width = -1, int height = -1, int border = 0);
	Atlas(std::vector<TileHandle>&& tiles, int width = -1, int height = -1, int border = 0);
	Atlas(Atlas&& atlas) noexcept;
	Atlas(const Atlas&) = delete;
	Atlas(const Atlas* const atlas);
	virtual ~Atlas() override;

	bool operator==(const Atlas& other) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline const unsigned char* const GetBuffer() const { return m_ImageBuffer; }
	
	class RectRender SampleSubtile(const size_t& index, const struct TextureSettings& texture_settings = Texture::nearest_settings, const ShaderHandle& shader = ShaderFactory::standard_shader, const ZIndex& z = 0, const bool& visible = true) const;

private:
	void RectPack(std::vector<TileHandle>& tiles, const int& width = -1, const int& height = -1);
	void PlaceTiles();
};
