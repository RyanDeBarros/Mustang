#include "TileFactory.h"

#include "Macros.h"
#include "Logger.inl"

TileHandle TileFactory::handle_cap;
std::unordered_map<TileHandle, Tile*> TileFactory::factory;
std::unordered_map<TileConstructArgs, TileHandle> TileFactory::lookupMap;

void TileFactory::Init()
{
	handle_cap = 1;
}

void TileFactory::Terminate()
{
	for (auto& [handle, tile] : factory)
	{
		if (tile)
		{
			delete tile;
			tile = nullptr;
		}
	}
	factory.clear();
	lookupMap.clear();
}

Tile const* TileFactory::Get(TileHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_TILE
		Logger::LogWarning("Tile handle (" + std::to_string(handle) + ") does not exist in TileFactory.");
#endif
		return nullptr;
	}
}

TileHandle TileFactory::GetHandle(const TileConstructArgs& args)
{
	auto iter = lookupMap.find(args);
	if (iter != lookupMap.end())
		return iter->second;
	Tile tile(args.filepath.c_str());
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Tile(std::move(tile)));
		lookupMap[args] = handle;
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::RegisterTile(Tile* emplace_tile)
{
	if (emplace_tile->IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, emplace_tile);
		return handle;
	}
	else return 0;
}

unsigned char const* TileFactory::GetImageBuffer(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_ImageBuffer : nullptr;
}

int TileFactory::GetWidth(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_Width : 0;
}

int TileFactory::GetHeight(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_Height : 0;
}

int TileFactory::GetBPP(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_BPP : 0;
}
