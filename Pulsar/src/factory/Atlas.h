#pragma once

#include <vector>
#include <unordered_map>

#include "Tile.h"
#include "ShaderRegistry.h"
#include "Texture.h"
#include "render/ActorRenderBase.h"

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

	int GetBorder() const { return m_Border; }
	const std::vector<Placement>& GetPlacements() const { return m_Placements; }
	unsigned char const* const GetBuffer() const { return TileRegistry::GetImageBuffer(m_Tile); }
	
	class RectRender SampleSubtile(size_t index, const struct TextureSettings& texture_settings = Texture::nearest_settings,
		TextureVersion texture_version = 0, ShaderHandle shader = ShaderRegistry::standard_shader, ZIndex z = 0, FickleType fickle_type = FickleType::Protean, bool visible = true) const;
	TileHandle GetTileHandle() const { return m_Tile; }
	int GetWidth() const { return TileRegistry::GetWidth(m_Tile); }
	int GetHeight() const { return TileRegistry::GetHeight(m_Tile); }

private:
	void RectPack(std::vector<TileHandle>& tiles, int width = -1, int height = -1);
	void PlaceTiles(unsigned char* image_buffer, int atlas_width);
};
