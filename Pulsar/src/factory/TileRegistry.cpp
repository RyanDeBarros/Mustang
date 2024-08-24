#include "TileRegistry.h"

#include "Macros.h"
#include "Logger.inl"

TileHandle TileRegistry::handle_cap;
std::unordered_map<TileHandle, Tile> TileRegistry::registry;
std::unordered_map<TileConstructArgs, TileHandle> TileRegistry::lookupMap;

void TileRegistry::Init()
{
	handle_cap = 1;
}

void TileRegistry::Terminate()
{
	registry.clear();
	lookupMap.clear();
}

Tile const* TileRegistry::Get(TileHandle handle)
{
	auto iter = registry.find(handle);
	if (iter != registry.end())
		return &iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_TILE
		Logger::LogWarning("Tile handle (" + std::to_string(handle) + ") does not exist in TileRegistry.");
#endif
		return nullptr;
	}
}

bool TileRegistry::DestroyTile(TileHandle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

TileHandle TileRegistry::GetHandle(const TileConstructArgs& args)
{
	auto iter = lookupMap.find(args);
	if (iter != lookupMap.end())
		return iter->second;
	Tile tile(args.filepath.c_str(), args.svg_scale);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		registry.emplace(handle, std::move(tile));
		lookupMap[args] = handle;
		return handle;
	}
	else return 0;
}

TileHandle TileRegistry::RegisterTile(Tile&& emplace_tile)
{
	if (emplace_tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		registry.emplace(handle, std::move(emplace_tile));
		return handle;
	}
	else return 0;
}

unsigned char const* TileRegistry::GetImageBuffer(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_ImageBuffer : nullptr;
}

int TileRegistry::GetWidth(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_Width : 0;
}

int TileRegistry::GetHeight(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_Height : 0;
}

int TileRegistry::GetBPP(TileHandle tile)
{
	Tile const* t = Get(tile);
	return t ? t->m_BPP : 0;
}
