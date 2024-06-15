#pragma once

#include <unordered_map>
#include <string>

#include "Typedefs.h"
#include "Tile.h"
#include "Atlas.h"

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
	static TileHandle GetHandle(const Atlas& atlas);

	static inline const Tile* GetConstTileRef(const TileHandle& tile) { return Get(tile); };
	static const unsigned char* GetImageBuffer(const TileHandle& tile);
	static int GetWidth(const TileHandle& tile);
	static int GetHeight(const TileHandle& tile);
	static int GetBPP(const TileHandle& tile);
	static std::string GetFilepath(const TileHandle& tile);
};
