#pragma once

#include <unordered_map>
#include <string>

#include "Typedefs.h"
#include "Tile.h"

class TileFactory
{
	static TileHandle handle_cap;
	static std::unordered_map<TileHandle, Tile*> factory;
	static Tile* Get(const TileHandle&);

	TileFactory() = delete;
	~TileFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TileHandle GetHandle(const char* filepath);
	static TileHandle GetAtlasHandle(std::vector<TileHandle>& tiles, int width = -1, int height = -1, int border = 0);
	static TileHandle GetAtlasHandle(std::vector<TileHandle>&& tiles, int width = -1, int height = -1, int border = 0);
	static TileHandle GetAtlasHandle(const char* texture_filepath, const std::vector<struct Placement>& placements, int border);
	static TileHandle GetAtlasHandle(const class Atlas* const atlas);
	static TileHandle GetSubtileHandle(const TileHandle& full_handle, int x, int y, int w, int h);

	static inline const Tile* GetConstTileRef(const TileHandle& tile) { return Get(tile); }
	static inline Tile* GetTileRef(const TileHandle& tile) { return Get(tile); }
	static const unsigned char* GetImageBuffer(const TileHandle& tile);
	static int GetWidth(const TileHandle& tile);
	static int GetHeight(const TileHandle& tile);
	static int GetBPP(const TileHandle& tile);
	static std::string GetFilepath(const TileHandle& tile);
};
