#include "TileFactory.h"

#include "Logger.h"

TileHandle TileFactory::handle_cap;
std::unordered_map<TileHandle, Tile*> TileFactory::factory;

void TileFactory::Init()
{
	handle_cap = 1;
}

void TileFactory::Terminate()
{
	for (const auto& [handle, tile] : factory)
	{
		if (tile)
			delete tile;
	}
	factory.clear();
}

Tile* TileFactory::Get(const TileHandle& handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("Tile handle (" + std::to_string(handle) + ") does not exist in TileFactory.");
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

TileHandle TileFactory::GetHandle(const Atlas& atlas)
{
	for (const auto& [handle, tile] : factory)
	{
		if (tile->Equivalent(atlas))
			return handle;
	}
	Tile tile(atlas);
	if (tile.IsValid())
	{
		TileHandle handle = handle_cap++;
		factory.emplace(handle, new Tile(std::move(tile)));
		return handle;
	}
	else return 0;
}

const unsigned char* TileFactory::GetImageBuffer(const TileHandle& tile)
{
	Tile* t = Get(tile);
	return t ? t->m_ImageBuffer : nullptr;
}

int TileFactory::GetWidth(const TileHandle& tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Width : 0;
}

int TileFactory::GetHeight(const TileHandle& tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Height : 0;
}

int TileFactory::GetBPP(const TileHandle& tile)
{
	Tile* t = Get(tile);
	return t ? t->m_BPP : 0;
}

std::string TileFactory::GetFilepath(const TileHandle& tile)
{
	Tile* t = Get(tile);
	return t ? t->m_Filepath : "";
}


