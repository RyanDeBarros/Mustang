#pragma once

#include <set>

#include "Typedefs.h"
#include "Tile.h"

struct POTwrapper
{
	TileRect rect;
	int potW, potH;
};

static struct SortTileSet
{
	int operator() (const std::pair<TileHandle, POTwrapper>& p1, const std::pair<TileHandle, POTwrapper>& p2) const;
};

class Atlas
{
	unsigned char* m_AtlasBuffer;
	int m_Width, m_Height;
	std::set<std::pair<TileHandle, POTwrapper>, SortTileSet> m_TileSet;

	static int UpperPot(int n);
	static int LowerPot(int n);
	static inline POTwrapper Wrap(const TileRect& rect);

	inline size_t BufferSize() const { return 4 * m_Width * m_Height; }

public:
	Atlas(const int& width, const int& height);
	~Atlas();

	static void SaveAtlas(const Atlas& atlas, const char* texture_filepath, const char* asset_filepath);
	static Atlas LoadAtlas(const char* asset_filepath);

	void Insert(const TileHandle& tile);
	void Pack();
};
