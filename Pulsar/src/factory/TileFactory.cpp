#include "TileFactory.h"

#include "Macros.h"
#include "Logger.h"
#include "Atlas.h"
#include "Subtile.h"

TileHandle TileFactory::handle_cap;
std::unordered_map<TileHandle, Tile*> TileFactory::factory;

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
}

Tile* TileFactory::Get(TileHandle handle)
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

TileHandle TileFactory::GetHandle(const char* filepath)
{
	for (const auto& [handle, tile] : factory)
	{
		if (tile->Equivalent(filepath))
			return handle;
	}
	Tile tile(filepath);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Tile(std::move(tile)));
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::GetAtlasHandle(std::vector<TileHandle>& tiles, int width, int height, int border)
{
	for (const auto& [handle, tile] : factory)
	{
		auto at = dynamic_cast<Atlas* const>(tile);
		if (at && at->Equivalent(tiles, width, height, border))
			return handle;
	}
	Atlas tile(tiles, width, height, border);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Atlas(std::move(tile)));
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::GetAtlasHandle(const char* texture_filepath, const std::vector<Placement>& placements, int border)
{
	for (const auto& [handle, tile] : factory)
	{
		auto at = dynamic_cast<Atlas* const>(tile);
		if (at && at->Equivalent(texture_filepath, placements, border))
			return handle;
	}
	Atlas tile(texture_filepath, placements, border);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Atlas(std::move(tile)));
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::GetAtlasHandle(const char* texture_filepath, std::vector<struct Placement>&& placements, int border)
{
	for (const auto& [handle, tile] : factory)
	{
		auto at = dynamic_cast<Atlas* const>(tile);
		if (at && at->Equivalent(texture_filepath, placements, border))
			return handle;
	}
	Atlas tile(texture_filepath, std::move(placements), border);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Atlas(std::move(tile)));
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::GetAtlasHandle(const Atlas* const atlas)
{
	for (const auto& [handle, tile] : factory)
	{
		auto at = dynamic_cast<Atlas* const>(tile);
		if (at && *at == *atlas)
			return handle;
	}
	Atlas tile(atlas);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Atlas(std::move(tile)));
		return handle;
	}
	else return 0;
}

TileHandle TileFactory::GetSubtileHandle(TileHandle full_handle, int x, int y, int w, int h)
{
	Tile* t = Get(full_handle);
	if (!t)
		return 0;

	for (const auto& [handle, tile] : factory)
	{
		auto sub = dynamic_cast<Subtile* const>(tile);
		if (sub && sub->m_Filepath == t->m_Filepath && sub->x == x && sub->y == y && sub->w == w && sub->h == h)
			return handle;
	}

	TileHandle handle = handle_cap++;
	factory.emplace(handle, new Subtile(t, x, y, w, h));
	return handle;
}

const unsigned char* TileFactory::GetImageBuffer(TileHandle tile)
{
	Tile* t = Get(tile);
	return t ? t->m_ImageBuffer : nullptr;
}

int TileFactory::GetWidth(TileHandle tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Width : 0;
}

int TileFactory::GetHeight(TileHandle tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Height : 0;
}

int TileFactory::GetBPP(TileHandle tile)
{
	Tile* t = Get(tile);
	return t ? t->m_BPP : 0;
}

std::string TileFactory::GetFilepath(TileHandle tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Filepath : "";
}


