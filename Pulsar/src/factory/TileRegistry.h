#pragma once

#include <unordered_map>
#include <string>

#include "Typedefs.h"
#include "Tile.h"

struct TileConstructArgs
{
	std::string filepath;

	inline bool operator==(const TileConstructArgs& args) const
	{
		return filepath == args.filepath;
	}
};

template<>
struct std::hash<TileConstructArgs>
{
	inline size_t operator()(const TileConstructArgs& args) const
	{
		return hash<std::string>{}(args.filepath);
	}
};

class TileRegistry
{
	static TileHandle handle_cap;
	static std::unordered_map<TileHandle, Tile> registry;
	static std::unordered_map<TileConstructArgs, TileHandle> lookupMap;

	TileRegistry() = delete;
	TileRegistry(const TileRegistry&) = delete;
	TileRegistry(TileRegistry&&) = delete;
	~TileRegistry() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TileHandle GetHandle(const TileConstructArgs& args);
	static Tile const* Get(TileHandle handle);
	static bool DestroyTile(TileHandle handle);

	static TileHandle RegisterTile(Tile&& emplace_tile);
	static unsigned char const* GetImageBuffer(TileHandle tile);
	static int GetWidth(TileHandle tile);
	static int GetHeight(TileHandle tile);
	static int GetBPP(TileHandle tile);
};