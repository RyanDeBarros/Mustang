#pragma once

#include <vector>
#include <unordered_map>

#include "Typedefs.h"
#include "Tile.h"
#include "ShaderFactory.h"
#include "Texture.h"

struct Placement
{
	TileHandle tile;
	int x, y, w, h;
	bool r;

	bool operator==(const Placement& other) const
	{
		return tile == other.tile && x == other.x && y == other.y && w == other.w && h == other.h && r == other.r;
	}
};

class atlas_cast_error : public std::exception
{
};

class Atlas : public Tile
{
	friend class Tile;
	friend class Texture;
	friend class TileFactory;

	int m_Border;
	int m_BufferSize;
	std::vector<Placement> m_Placements;

	bool Equivalent(const std::vector<TileHandle>& tiles, int width, int height, int border) const;
	bool Equivalent(const char* texture_filepath, const std::vector<Placement>& placements, int border) const;

public:
	static constexpr unsigned char BPP = 4;
	static constexpr unsigned char STRIDE_BYTES = sizeof(unsigned char) * BPP;

	Atlas(std::vector<TileHandle>& tiles, int width = -1, int height = -1, int border = 0);
	Atlas(const char* texture_filepath, const std::vector<Placement>& placements, int border);
	Atlas(const char* texture_filepath, std::vector<Placement>&& placements, int border);
	Atlas(const Atlas* const atlas);
	Atlas(const Atlas&) = delete;
	Atlas(Atlas&& atlas) noexcept;
	Atlas& operator=(Atlas&& atlas) noexcept;
	virtual ~Atlas() override;

	bool operator==(const Atlas& other) const;

	inline int GetBorder() const { return m_Border; }
	inline const std::vector<Placement>& GetPlacements() const { return m_Placements; }
	inline const unsigned char* const GetBuffer() const { return m_ImageBuffer; }
	
	class RectRender SampleSubtile(size_t index, const struct TextureSettings& texture_settings = Texture::nearest_settings, ShaderHandle shader = ShaderFactory::standard_shader, ZIndex z = 0, bool visible = true) const;

private:
	void RectPack(std::vector<TileHandle>& tiles, int width = -1, int height = -1);
	void PlaceTiles();
};
