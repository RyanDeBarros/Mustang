#pragma once

#include <vector>
#include <unordered_map>

#include "Typedefs.h"
#include "Tile.h"
#include "ShaderFactory.h"
#include "Texture.h"
#include "render/transform/Fickle.inl"

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

class null_pointer_error : public std::exception
{
};

class null_handle_error : public std::exception
{
};

class Atlas
{
	friend class Texture;

	TileHandle m_Tile;
	int m_Border;
	std::vector<Placement> m_Placements;

public:
	static constexpr unsigned char BPP = 4;
	static constexpr unsigned char STRIDE_BYTES = sizeof(unsigned char) * BPP;

	Atlas(std::vector<TileHandle>& tiles, int width = -1, int height = -1, int border = 0);
	Atlas(const std::string& texture_filepath, const std::vector<Placement>& placements, int border);
	Atlas(const std::string& texture_filepath, std::vector<Placement>&& placements, int border);
	Atlas(const Atlas&) = delete;
	Atlas(Atlas&& atlas) noexcept;
	Atlas& operator=(Atlas&& atlas) noexcept;

	bool operator==(const Atlas& other) const;

	inline int GetBorder() const { return m_Border; }
	inline const std::vector<Placement>& GetPlacements() const { return m_Placements; }
	inline unsigned char const* const GetBuffer() const { return TileFactory::GetImageBuffer(m_Tile); }
	
	class RectRender SampleSubtile(size_t index, const struct TextureSettings& texture_settings = Texture::nearest_settings, TextureVersion texture_version = 0, ShaderHandle shader = ShaderFactory::standard_shader, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true) const;
	inline TileHandle GetTileHandle() const { return m_Tile; }
	inline int GetWidth() const { return TileFactory::GetWidth(m_Tile); }
	inline int GetHeight() const { return TileFactory::GetHeight(m_Tile); }

private:
	void RectPack(std::vector<TileHandle>& tiles, int width = -1, int height = -1);
	void PlaceTiles(unsigned char* image_buffer, int atlas_width);
};
